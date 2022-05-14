#pragma once
#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>

#include "TSQueue.hpp"
#include "Message.hpp"
#include "Connection.hpp"
#include "PossibleIDs.hpp"
#include "Helpers.hpp"

#include "Runnable.hpp"

class Client : public Runnable {
protected:
	boost::asio::io_context ioContext;
	boost::asio::ip::tcp::socket socket;
	std::unique_ptr<Connection<PossibleMessageIDs>> connection;
	std::thread ioThread;
	std::thread inputMsgThread;
	std::thread heartbeatThread;

	std::mutex responseMut;
	std::condition_variable responseFromServer;
	std::atomic_bool isAvailable = false;

	Message<PossibleMessageIDs> whoOnline;
	std::string interlocutor;

private:
	TSQueue<OwnedMessage<PossibleMessageIDs>> msgIn;

public:
	Client() : socket{ ioContext } {
	}
	~Client() {
		disconnect();
	}

public:
	void run() override {
		if (!connectToServer()) {
			std::cout << "Unable to connect to the server! Try again later.\n";
			return;
		}

		initClient();
		printHelp();
		chooseInterlocutor();

		while (isConnected()) {
			if (interlocutor == "")
				std::cout << "Interlocutor not selected or not available. Enter '\\x' to get a list of all available users.\n";
			std::string current;
			std::getline(std::cin, current);
			if (interlocutor != "")
				std::cout << "Message sended to [" << interlocutor << "]\n\n";

			if (!isConnected())
				return;

			if (current != "\\x" && isAvailable.load()) {
				Message<PossibleMessageIDs> msg;
				msg.header.id = PossibleMessageIDs::sendMessageTo;
				msg << current << interlocutor;
				connection->send(msg);
			} else {
				std::string toOutput = current == "\\x" ? "" : "User " + interlocutor + " not available now. ";
				std::cout << "\n" << toOutput << "Now you will be given a list of potential interlocutors.\n";
				isAvailable.store(false);
				chooseInterlocutor();
			}
		}

		std::cout << "The connection with the server is lost! The program will be closed.\n";
	}

	void connect(const std::string& host, const uint16_t port) {
		try {
			boost::asio::ip::tcp::resolver resolver{ ioContext };
			boost::asio::ip::tcp::resolver::results_type endpoints = resolver.resolve(host, std::to_string(port));

			connection = std::make_unique<Connection<PossibleMessageIDs>>(
				Connection<PossibleMessageIDs>::Owner::client, ioContext, 
				boost::asio::ip::tcp::socket{ ioContext }, msgIn
			);
			connection->connectToServer(endpoints);

			ioThread = std::thread([this]() { ioContext.run(); });
		} catch (std::exception& error) {
			std::cout << "[CLIENT]: ERROR! " << error.what() << "\n";
		}
	}

	void disconnect() {
		if (isConnected())
			connection->disconnect();

		ioContext.stop();
		if (ioThread.joinable())
			ioThread.join();
		if (inputMsgThread.joinable())
			inputMsgThread.join();

		connection.release();
	}

	bool isConnected() {
		return connection ? connection->isConnected() : false;
	}

protected:
	static void processInputMsgs(Client* client) {
		while (client->isConnected()) {
			client->msgIn.wait();
			Message<PossibleMessageIDs> msg = client->msgIn.front().msg;
			client->msgIn.pop();

			switch (msg.header.id) {
			case PossibleMessageIDs::newClient: {
				std::string name;
				msg >> name;
				std::cout << "\n" << name << " is online!\n";
				break;
			}
			case PossibleMessageIDs::sendMessageTo: {
				std::string name;
				msg >> name;
				std::cout << "\nFrom [" << name << "]: ";

				printMessageData(msg);
				break;
			}
			case PossibleMessageIDs::sendMessageAll:
				std::cout << "[To all]: ";
				printMessageData(msg);
				break;
			case PossibleMessageIDs::whoOnline:
				client->whoOnline = msg;
				client->isAvailable.store(true);
				break;
			case PossibleMessageIDs::notAvailable:
				client->isAvailable = false;
				break;
			case PossibleMessageIDs::serverResponse: {
				std::unique_lock uniqueLock{ client->responseMut };
				client->responseFromServer.notify_all();
				break;
			}
			case PossibleMessageIDs::clientDisconnected: {
				std::string name;
				msg >> name;

				std::cout << "User " << name << " disconnected.\n";
				if (name == client->interlocutor) {
					client->isAvailable.store(false);
					client->interlocutor = "";
				}
				break;
			}
			default:
				break;
			}
		}
	}

	static void printMessageData(Message<PossibleMessageIDs>& msg) {
		std::vector<std::string> data;
		std::string current;
		while (msg.body.size() != 0) {
			msg >> current;
			data.push_back(current);
		}

		std::for_each(data.crbegin(), data.crend(), [](std::string elem) { std::cout << elem; });
		std::cout << "\n\n";
	}

private:
	std::string getServerIP() {
		boost::asio::io_context ioContext;

		Helpers::sendMessageToNewClient(ioContext);
		std::string ip = Helpers::getIPOfClient(ioContext);
		
		return ip;
	}

	bool connectToServer() {
		std::cout << "Try to connect to server...\n";

		std::string ip = getServerIP();
		std::cout << ip << std::endl;
		if (ip == "")
			return false;

		connect(ip, 60'000);
		connection->waitForReadiness();

		return connection->isConnected();
	}

	std::string getName() {
		std::string name;

		std::cout << "Welcome to the crossplatform chat!\n";
		do {
			std::cout << "Input your name -- ";
			std::getline(std::cin, name);
			if (name.length() != 0)
				break;
			else
				std::cout << "The name is empty! Try again.\n";
		} while (true);
		
		return name;
	}

	void printHelp() {
		std::cout << "If you want to select another interlocutor, enter '\\x'.\n\n";
	}

	void initClient() {
		inputMsgThread = std::thread(processInputMsgs, this);
		{
			std::unique_lock ul{ responseMut };
			responseFromServer.wait(ul);
		}
		heartbeatThread = std::thread(sendHeartbeat, this);

		std::cout << "Connecting to the server successfully!\n\n";
		std::string name = getName();
		{
			Message<PossibleMessageIDs> msg;
			msg.header.id = PossibleMessageIDs::sendName;
			msg << name;
			connection->send(msg);
		}
	}

	static void sendHeartbeat(Client* client) {
		Message<PossibleMessageIDs> msg;
		msg.header.id = PossibleMessageIDs::sendHeartbeat;
		
		while (client->isConnected()) {
			using namespace std::chrono_literals;

			client->connection->send(msg);
			std::this_thread::sleep_for(1s);
		}
	}

	bool chooseInterlocutor() {
		Message<PossibleMessageIDs> msg;
		msg.header.id = PossibleMessageIDs::whoOnline;
		connection->send(msg);

		while (!isAvailable.load()) {
			using namespace std::chrono_literals;
			std::this_thread::sleep_for(100ms);
		}

		return getInterlocutor();
	}

	bool getInterlocutor() {
		if (whoOnline.body.size() == 0) {
			std::cout << "Nobody is online.\n\n";

			return false;
		} else {
			std::cout << "Select the user you want to chat with:\n";
			std::vector<std::string> names = getPossibleInterlocutors();
			Helpers::printOptions(names);
			interlocutor = names[Helpers::chooseOption(1, names.size()) - 1];
			std::cout << "\nMessage will be sended to [" << interlocutor << "]\n\n";

			return true;
		}
	}

	std::vector<std::string> getPossibleInterlocutors() {
		std::vector<std::string> names;

		std::string current;
		while (whoOnline.body.size() != 0) {
			whoOnline >> current;
			names.push_back(current);
		}

		return names;
	}
};
