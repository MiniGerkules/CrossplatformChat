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
	std::map<std::string, std::shared_ptr<Connection<T>>> users;

	boost::asio::io_context ioContext;
	boost::asio::ip::tcp::acceptor acceptor;
	std::thread ioThread;

	size_t nextClienID = 0;

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
			ioThread = std::thread{ [this]() { ioContext.run(); } };
		} catch (const std::exception& error) {
			std::cout << "[SERVER]: ERROR" << error.what() << "\n";
			return false;
		}

		std::cout << "[SERVER]: Started\n";
		return true;
	}

	void stop() {
		ioContext.stop();
		if (ioThread.joinable())
			ioThread.join();

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
					connections.back()->connectToClient(nextClienID++);
					std::cout << "[" << connections.back()->getID() << "] has been connected!\n";
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
			onClientDisconnect(client);

			if (client->getName() == "")
				connections.erase(std::remove(connections.begin(), connections.end(), client), connections.end());
			else
				users.erase(client->getName());
			client.reset();

			return false;
		}
	}

	void messageAllClients(const Message<T>& msg, std::shared_ptr<Connection<T>> ignoredClient = nullptr) {
		std::vector<std::string> names;
		bool needClear = false;

		for (auto& client : connections) {
			if (client && client->isConnected()) {
				if (client != ignoredClient)
					client->send(msg);
			} else {
				onClientDisconnect(client);
				if (client->getName() != "")
					names.push_back(client->getName());
				client.reset();
				needClear = true;
			}
		}

		if (needClear) {
			connections.erase(std::remove(connections.begin(), connections.end(), nullptr), connections.end());
			for (std::string name : names)
				users.erase(name);
		}
	}

	void processInputMsgs(size_t maxNumMsgs = -1) {
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
		std::cout << "[SERVER]: Client with ID " << client->getID() << " connected\n";
	}

	void onClientDisconnect(std::shared_ptr<Connection<T>> client) {
		std::cout << "[SERVER]: Client with ID " << client->getID() << " disconnected\n";
	}

	void onMessage(std::shared_ptr<Connection<T>> client, Message<T>& msg) {
		switch (msg.header.id) {
		case PossibleMessageIDs::sendName: {
			std::string name;

			msg >> name;
			std::cout << "Client send name -- " << name << "\n";

			client->setName(name);
			users[name] = client;
			break;
		}
		case PossibleMessageIDs::sendMessageTo: {
			std::string nameTo;
			
			msg >> nameTo;
			std::cout << client->getName() << " send message to " << nameTo;

			msg << client->getName();
			if (users.find(nameTo) != users.end() && !messageClient(users[nameTo], msg)) {
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
		case PossibleMessageIDs::whoOnline: {
			std::cout << client->getName() << " requests who online\n";

			std::vector<std::string> online;
			for (const auto& [name, connection] : users) {
				if (connection->isConnected() && connection->getName() != client->getName())
					online.push_back(name);
			}

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
};
