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
		if (!connect("127.0.0.1", 60'000)) {
			std::cout << "Can't connect to server!";
			return;
		}

		std::atomic_bool isAvailable = false;
		Message<PossibleMessageIDs> whoOnline;
		inputMsgThread = std::thread(processInputMsgs, std::ref(msgIn), std::ref(whoOnline), std::ref(isAvailable));

		initClient();
		printHelp();
		chooseInterlocutor(isAvailable, whoOnline);

		while (true) {
			if (!isConnected()) {
				std::cout << "Server down!!! The program close.\n";
				break;
			}

			if (isAvailable.load() == true) {
				std::string current;

				std::cout << "\nMessage to [" << interlocutor << "]:\n";
				std::getline(std::cin, current);

				if (current != "\\x") {
					Message<PossibleMessageIDs> msg;
					msg.header.id = PossibleMessageIDs::sendMessageTo;
					msg << current << interlocutor;
					connection->send(msg);
				} else {
					isAvailable.store(false);
					chooseInterlocutor(isAvailable, whoOnline);
				}
			} else {
				std::cout << "User " << interlocutor << " is no longer available. Choose another interlocutor.";
				chooseInterlocutor(isAvailable, whoOnline);
			}
		}
	}

	bool connect(const std::string& host, const uint16_t port) {
		try {
			boost::asio::ip::tcp::resolver resolver{ ioContext };
			boost::asio::ip::tcp::resolver::results_type endpoints = resolver.resolve(host, std::to_string(port));

			connection = std::make_unique<Connection<PossibleMessageIDs>>(
				Connection<PossibleMessageIDs>::Owner::client, ioContext, boost::asio::ip::tcp::socket{ ioContext }, msgIn
			);
			connection->connectToServer(endpoints);

			ioThread = std::thread([this]() { ioContext.run(); });
		}
		catch (std::exception& error) {
			std::cout << "[CLIENT]: ERROR! " << error.what() << "\n";
			return false;
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
	static void processInputMsgs(TSQueue<OwnedMessage<PossibleMessageIDs>>& msgIn, Message<PossibleMessageIDs>& whoOnline,
		std::atomic_bool& isAvailable) {
		while (true) {
			if (!msgIn.empty()) {
				Message<PossibleMessageIDs> msg = msgIn.front().msg;
				msgIn.pop();

				switch (msg.header.id) {
				case PossibleMessageIDs::sendMessageTo: {
					std::string name;
					msg >> name;
					std::cout << "From [" << name << "]: ";

					printMessageData(msg);
					break;
				}
				case PossibleMessageIDs::sendMessageAll:
					std::cout << "[To all]: ";
					printMessageData(msg);
					break;
				case PossibleMessageIDs::whoOnline:
					whoOnline = msg;
					isAvailable.store(true);
					break;
				case PossibleMessageIDs::notAvailable:
					isAvailable = false;
					break;
				}
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
	}

private:
	std::string getName() {
		std::string name;

		std::cout << "Welcome to the crossplatform chat!\n";
		do {
			std::cout << "Input your name -- ";
			std::getline(std::cin, name);
			if (name.length() != 0) {
				std::cout << "Hello " << name << "!\n";
				break;
			} else {
				std::cout << "The name is empty! Try again.\n";
			}
		} while (true);
		
		return name;
	}

	void printHelp() {
		std::cout << "If you want to leave the chat enter '\\x'.\n";
	}

	void initClient() {
		std::string name = getName();
		{
			Message<PossibleMessageIDs> msg;
			msg.header.id = PossibleMessageIDs::sendName;
			msg << name;
			connection->send(msg);
		}
	}

	bool chooseInterlocutor(const std::atomic_bool& isAvailable, Message<PossibleMessageIDs>& whoOnline) {
		Message<PossibleMessageIDs> msg;
		msg.header.id = PossibleMessageIDs::whoOnline;
		connection->send(msg);

		while (isAvailable.load() != true) {
			using namespace std::chrono_literals;
			std::this_thread::sleep_for(100ms);
		}

		return getInterlocutor(whoOnline);
	}

	bool getInterlocutor(Message<PossibleMessageIDs> whoOnline) {
		if (whoOnline.body.size() == 0) {
			std::cout << "Nobody is online. Try to enter '\\x' or close the porgram.\n";

			return false;
		} else {
			std::cout << "Select the user you want to chat with:\n";
			std::vector<std::string> names = getPossibleInterlocutors(whoOnline);
			Helpers::printOptions(names);
			interlocutor = names[Helpers::chooseOption(1, names.size()) - 1];

			return true;
		}
	}

	std::vector<std::string> getPossibleInterlocutors(Message<PossibleMessageIDs>& msg) {
		std::vector<std::string> names;

		std::string current;
		while (msg.body.size() != 0) {
			msg >> current;
			names.push_back(current);
		}

		return names;
	}
};
