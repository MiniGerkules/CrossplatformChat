#pragma once
#include <memory>
#include <string>
#include <exception>
#include <boost/asio.hpp>
#include <functional>

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
	boost::asio::deadline_timer timer;
	std::function<void()> ifConnectionLost;

	std::mutex waitMut;
	std::condition_variable isReady;
	std::atomic_bool canContinue;

	Owner ownerType;
	TSQueue<Message<T>> msgOut;
	TSQueue<OwnedMessage<T>>& msgIn;

	std::string name;
	size_t id = 0;

private:
	Message<T> tempMsg;

public:
	Connection(Owner owner, boost::asio::io_context& ioContext, boost::asio::ip::tcp::socket socket,
		TSQueue<OwnedMessage<T>>& msgIn) : ifConnectionLost{ ifConnectionLost }, ioContext { ioContext },
		msgIn{ msgIn }, socket{ std::move(socket) }, timer{ ioContext }
	{
		ownerType = owner;
		canContinue.store(false);
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
		}
	}

	void connectToServer(const boost::asio::ip::tcp::resolver::results_type& endpoints) {
		if (ownerType == Owner::client) {
			boost::asio::async_connect(socket, endpoints,
				[this](boost::system::error_code ec, boost::asio::ip::tcp::endpoint endpoint) {
					if (!ec) {
						timer.cancel();
						readHeader();
					} else {
						socket.close();
					}

					canContinue.store(true);
					std::unique_lock uniqueLock{ waitMut };
					isReady.notify_all();
				}
			);

			timer.expires_from_now(boost::posix_time::milliseconds(2'000));
			timer.async_wait([this](const boost::system::error_code& ec) {
					if (!ec) {
						socket.close();
					}

					canContinue.store(true);
					std::unique_lock uniqueLock{ waitMut };
					isReady.notify_all();
				}
			);
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

	void waitForReadiness() {
		std::unique_lock uniqueLock{ waitMut };
		isReady.wait(uniqueLock, [this]() { return canContinue.load(); });
	}

private:
	void readHeader() {
		boost::asio::async_read(socket, boost::asio::buffer(&tempMsg.header, sizeof(MessageHeader<T>)),
			[this](boost::system::error_code ec, size_t length) {
				if (!ec) {
					tempMsg.body.clear();
					if (tempMsg.header.size > 0) {
						tempMsg.body.resize(tempMsg.header.size);
						readBody();
					} else {
						addToInQueue();
					}
				} else {
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
					socket.close();
				}
			}
		);
	}

	void addToInQueue() {
		pushInQueue();
		readHeader();
	}

private:
	void pushInQueue() {
		if (ownerType == Owner::server)
			msgIn.push({ this->shared_from_this(), tempMsg });
		else
			msgIn.push({ nullptr, tempMsg });
	}
};
