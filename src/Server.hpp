#pragma once
#include <memory>
#include <thread>
#include <deque>
#include <map>
#include <boost/asio.hpp>

#include "Connection.hpp"
#include "Message.hpp"
#include "TSQueue.hpp"

#include "Runnable.hpp"

template <typename T>
class Server : public Runnable {
protected:
	TSQueue<OwnedMessage<T>> msgIn;
	std::deque<std::shared_ptr<Connection<T>>> connections;
	std::map<std::string, std::pair<std::shared_ptr<Connection<T>>, std::chrono::steady_clock::time_point>> users;
	std::mutex mapWithUsersMut;

	boost::asio::io_context ioContext;
	boost::asio::ip::tcp::acceptor acceptor;
	std::thread ioThread;

	std::thread broadcastThread;
	std::thread heartbeatCheckerThread;
	std::atomic_bool canContinue = true;

	size_t nextID = 0;

public:
	Server(uint16_t port = 60'000) 
		: acceptor{ ioContext, boost::asio::ip::tcp::endpoint{ boost::asio::ip::tcp::v4(), port} } {
	}
	~Server() {
		stop();
	}

public:
	void run() {
		if (start()) {
			while (true)
				processInputMsgs();
		}
	}

	bool start() {
		try {
			waitForClientConnection();
			ioThread = std::thread([this]() { ioContext.run(); });
			broadcastThread = std::thread(checkBroadcastCalls, std::ref(canContinue));
			heartbeatCheckerThread = std::thread(checkHeartbeat, this);
		} catch (const std::exception& error) {
			std::cout << "[SERVER]: ERROR" << error.what() << "\n";
			canContinue.store(false);
			return false;
		}

		std::cout << "[SERVER]: Started\n";
		return true;
	}

	void stop() {
		ioContext.stop();
		if (ioThread.joinable())
			ioThread.join();

		canContinue.store(false); 
		if (broadcastThread.joinable())
			broadcastThread.join();

		std::cout << "[SERVER]: Stopped\n";
	}

	void waitForClientConnection() {
		acceptor.async_accept(
			[this](boost::system::error_code ec, boost::asio::ip::tcp::socket socket) {
				if (!ec) {
					std::cout << "[SERVER]: New connection -- " << socket.remote_endpoint() << "\n";

					std::shared_ptr<Connection<T>> newConnection = std::make_shared<Connection<T>>(Connection<T>::Owner::server,
						ioContext, std::move(socket), msgIn);

					onClientConnect(newConnection);
					connections.push_back(newConnection);
					connections.back()->connectToClient(nextID++);
				} else {
					std::cout << "[SERVER]: New connection has error -- " << ec.message() << "\n";
				}

				waitForClientConnection();
			}
		);
	}

	bool messageClient(std::shared_ptr<Connection<T>> client, const Message<T>& msg) {
		if (client && client->isConnected()) {
			client->send(msg);
			return true;
		} else {
			return false;
		}
	}

	void messageAllClients(const Message<T>& msg, std::shared_ptr<Connection<T>> ignoredClient = nullptr) {
		std::scoped_lock sq{ mapWithUsersMut };
		for (const auto& [name, pair] : users) {
			if (pair.first && pair.first->isConnected()) {
				if (pair.first != ignoredClient)
					pair.first->send(msg);
			}
		}
	}

	void processInputMsgs(size_t maxNumMsgs = -1) {
		msgIn.wait();

		size_t i = 0;
		while (i < maxNumMsgs && !msgIn.empty()) {
			auto msg = msgIn.front();
			onMessage(msg.remoute, msg.msg);

			msgIn.pop();
			++i;
		}
	}

protected:
	void onClientConnect(std::shared_ptr<Connection<T>> client) {
		std::cout << "[" << client->getID() << "] has been connected!\n";

		Message<PossibleMessageIDs> msg;
		msg.header.id = PossibleMessageIDs::serverResponse;
		messageClient(client, msg);
	}

	void onClientDisconnect(std::shared_ptr<Connection<T>> client) {
		std::cout << "[SERVER]: Client with ID " << client->getID() << " disconnected\n";
		if (client->getName() != "") {
			std::cout << "\t* Send to " << client->getName() << " inf disconnected\n";

			Message<PossibleMessageIDs> msg;
			msg.header.id = PossibleMessageIDs::clientDisconnected;
			msg << client->getName();
			messageAllClients(msg, client);
		}
	}

	void onMessage(std::shared_ptr<Connection<T>> client, Message<T>& msg) {
		switch (msg.header.id) {
		case PossibleMessageIDs::sendName: {
			std::string name;

			msg >> name;
			client->setName(name);
			mapWithUsersMut.lock();
			users[name] = std::make_pair(client, std::chrono::steady_clock::now());
			mapWithUsersMut.unlock();
			connections.erase(std::remove(connections.begin(), connections.end(), client), connections.end());

			Message<PossibleMessageIDs> newClient;
			newClient.header.id = PossibleMessageIDs::newClient;
			newClient << name;
			messageAllClients(newClient, client);
			break;
		}
		case PossibleMessageIDs::sendMessageTo: {
			std::string nameTo;

			msg >> nameTo;
			msg << client->getName();
			std::scoped_lock sq{ mapWithUsersMut };
			if (users.find(nameTo) == users.end() || !messageClient(users[nameTo].first, msg)) {
				Message<PossibleMessageIDs> msg;
				msg.header.id = PossibleMessageIDs::notAvailable;
				messageClient(client, msg);
			}
			break;
		}
		case PossibleMessageIDs::sendMessageAll: {
			messageAllClients(msg, client);
			break;
		}
		case PossibleMessageIDs::sendHeartbeat: {
			if (client->getName() != "") {
				std::scoped_lock sq{ mapWithUsersMut };
				users[client->getName()].second = std::chrono::steady_clock::now();
			}
			break;
		}
		case PossibleMessageIDs::whoOnline: {
			std::cout << client->getName() << " requests who online\n";

			std::vector<std::string> online;
			mapWithUsersMut.lock();
			for (const auto& [name, pair] : users) {
				if (pair.first->isConnected() && pair.first->getName() != client->getName())
					online.push_back(name);
			}
			mapWithUsersMut.unlock();

			Message<PossibleMessageIDs> response;
			response.header.id = PossibleMessageIDs::whoOnline;
			for (std::string name : online)
				response << name;

			std::cout << "Send to " << client->getName() << " the list of users:\n";
			if (online.size() == 0)
				std::cout << "No elems\n";
			else
				std::for_each(online.begin(), online.end(), [](std::string elem) { std::cout << elem << " "; });

			messageClient(client, response);
			break;
		}
		}
	}

private:
	static void checkBroadcastCalls(std::atomic_bool& canContinue) {
		boost::asio::io_context ioContext;

		while (canContinue) {
			std::string ip = Helpers::getIPOfClient(ioContext);
			if (ip != "")
				Helpers::sendMessageToNewClient(ioContext, ip);
		}
	}

	static void checkHeartbeat(Server<T>* server) {
		using namespace std::chrono;
		using namespace std::chrono_literals;
		using User = std::pair<std::string, std::shared_ptr<Connection<T>>>;

		while (server->canContinue.load()) {
			steady_clock::time_point now = steady_clock::now();
			std::vector<User> toDisconnect;

			server->mapWithUsersMut.lock();
			for (const auto& [name, pair] : server->users) {
				if (pair.first) {
					if (!pair.first->isConnected() || duration_cast<seconds>(now - pair.second) > 2s)
						toDisconnect.push_back(std::make_pair(name, pair.first));
				} else {
					server->users.erase(name);
				}
			}
			server->mapWithUsersMut.unlock();

			//clearDisconnectedClients(names);
			if (!toDisconnect.empty()) {
				for (User user : toDisconnect) {
					user.second->disconnect();
					server->onClientDisconnect(user.second);
				}

				server->mapWithUsersMut.lock();
				for (User user : toDisconnect) {
					server->users[user.first].first.reset();
					server->users.erase(user.first);
				}
				server->mapWithUsersMut.unlock();
			}
			
			std::this_thread::sleep_for(100ms);
		}
	}
};
