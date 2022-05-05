#pragma once
#include <memory>
#include <string>
#include <exception>
#include <boost/asio.hpp>

#include "Message.hpp"
#include "TSQueue.hpp"

template <typename T>
class Connection : public std::enable_shared_from_this<Connection<T>> {
public:
	enum class Owner {
		server, client
	};

protected:
	boost::asio::io_context& ioContext; 
	boost::asio::ip::tcp::socket socket;
	Owner ownerType;
	TSQueue<Message<T>> msgOut;
	TSQueue<OwnedMessage<T>>& msgIn;
	std::string name;
	size_t id = 0;

private:
	Message<T> tempMsg;

public:
	Connection(Owner owner, boost::asio::io_context& ioContext, boost::asio::ip::tcp::socket socket, 
		TSQueue<OwnedMessage<T>>& msgIn) : ioContext{ ioContext }, msgIn{ msgIn }, socket{ std::move(socket) } {
		ownerType = owner;
	}

	~Connection() {
		socket.close();
		msgOut.clear();
	}

	size_t getID() const {
		return id;
	}

	std::string getName() const {
		return name;
	}

	void setName(std::string name) {
		if (name[0] != '\0')
			this->name = name;
		else
			throw std::invalid_argument("The name is empty!");
	}

	void connectToClient(size_t id = 0) {
		if (ownerType == Owner::server) {
			if (socket.is_open()) {
				this->id = id;
				readHeader();
			}
		} else {
			std::cout << "[CLIENT]: ERROR! Client try to connect to client! Connection ID = " << id << "\n";
		}
	}

	void connectToServer(const boost::asio::ip::tcp::resolver::results_type& endpoints) {
		if (ownerType == Owner::client) {
			boost::asio::async_connect(socket, endpoints,
				[this](boost::system::error_code ec, boost::asio::ip::tcp::endpoint endpoint) {
					if (!ec)
						readHeader();
				}
			);
		} else {
			std::cout << "[SERVER]: ERROR! Server try to connect to server! Connection ID = " << id << "\n";
		}
	}

	void disconnect() {
		if (isConnected())
			boost::asio::post(ioContext, [this]() { socket.close(); });
	}

	bool isConnected() const {
		return socket.is_open();
	}

	void send(const Message<T>& msg) {
		boost::asio::post(ioContext,
			[this, msg]() {
				bool queueWasEmpty = msgOut.empty();
				msgOut.push(msg);
				if (queueWasEmpty)
					writeHeader();
			}
		);
	}

private:
	void readHeader() {
		boost::asio::async_read(socket, boost::asio::buffer(&tempMsg.header, sizeof(MessageHeader<T>)),
			[this](boost::system::error_code ec, size_t length) {
				if (!ec) {
					if (tempMsg.header.size > 0) {
						tempMsg.body.resize(tempMsg.header.size);
						readBody();
					} else {
						addToInQueue();
					}
				} else {
					std::cout << "[" << id << "]: ERROR! Can't read header!\n";
					socket.close();
				}
			}
		);
	}

	void readBody() {
		boost::asio::async_read(socket, boost::asio::buffer(tempMsg.body.data(), tempMsg.body.size()),
			[this](boost::system::error_code ec, size_t size) {
				if (!ec) {
					addToInQueue();
				} else {
					std::cout << "[" << id << "]: ERROR! Can't read body!\n";
					socket.close();
				}
			}
		);
	}

	void writeHeader() {
		boost::asio::async_write(socket, boost::asio::buffer(&msgOut.front().header, sizeof(MessageHeader<T>)),
			[this](boost::system::error_code ec, size_t length) {
				if (!ec) {
					if (msgOut.front().header.size > 0) {
						writeBody();
					} else {
						msgOut.pop();
						if (!msgOut.empty())
							writeHeader();
					}
				} else {
					std::cout << "[" << id << "]: ERROR! Can't write header!\n";
					socket.close();
				}
			}
		);
	}

	void writeBody() {
		boost::asio::async_write(socket, boost::asio::buffer(msgOut.front().body.data(), msgOut.front().body.size()),
			[this](boost::system::error_code ec, size_t size) {
				if (!ec) {
					msgOut.pop();
					if (!msgOut.empty())
						writeHeader();
				}
				else {
					std::cout << "[" << id << "]: ERROR! Can't write body!\n";
					socket.close();
				}
			}
		);
	}

	void addToInQueue() {
		if (ownerType == Owner::server)
			msgIn.push({ this->shared_from_this(), tempMsg });
		else
			msgIn.push({ nullptr, tempMsg });

		readHeader();
	}
};
