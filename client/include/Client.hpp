#pragma once

#include <functional>

#include <Messages/Types/RegularMessageType.hpp>
#include <Messages/Types/ConnectionMessageType.hpp>

#include <Messages/Handlers/ConnectionMessageHandler.hpp>

#include <Logger/Logger.hpp>
#include <Connection/ConnectionManager.hpp>

#include "Reader/Reader.hpp"
#include "Displayer/Displayer.hpp"
#include "CheckResponders/AppCheckResponder.hpp"

template <typename Executor>
class Client final : public ConnectionManagerDelegate,
                     public ReaderDelegate,
                     public std::enable_shared_from_this<Client<Executor>> {
//MARK: - Types for mocking delegates of Reader and ConnectionManager to perform initial actions
private:
    class RequestGetter_ final : public ConnectionManagerDelegate {
    public:
        using LostConnection_t = std::function<void(ConnectionManager &, const std::string_view)>;
        using DataAvailable_t = std::function<void(ConnectionManager &)>;

    private:
        LostConnection_t lostConnection_;
        DataAvailable_t dataAvailable_;

    public:
        RequestGetter_(LostConnection_t lostConnection, DataAvailable_t dataAvailable)
                : lostConnection_{ std::move(lostConnection) },
                  dataAvailable_{ std::move(dataAvailable) } {
        }

        void ifLostConnection(ConnectionManager &manager,
                              const std::string_view errorMsg) override {
            lostConnection_(manager, errorMsg);
        }

        void ifDataIsAvailable(ConnectionManager &manager) override {
            dataAvailable_(manager);
        }
    };

    class NameGetter_ final : public ReaderDelegate {
        std::function<void()> nameReader_;

    public:
        NameGetter_(std::function<void()> nameReader) : nameReader_{ std::move(nameReader) } {
        }

        void ifDataAvailable() override { nameReader_(); }
    };

//MARK: - Private fields
private:
    std::string name_;

    Executor &executor_;
    ConnectionManager connection_;

    /// Object for delegate of connection. It helps to get check request from the server.
    std::shared_ptr<RequestGetter_> requestGetter_ = std::make_shared<RequestGetter_>(
        std::bind(&Client::ifLostConnection, this, std::placeholders::_1, std::placeholders::_2),
        std::bind(&Client::getCheckRequest_, this, std::placeholders::_1)
    );

    /// Object for delegate of reader. It helps to get name from the user.
    std::shared_ptr<NameGetter_> nameGetter_ = std::make_shared<NameGetter_>(
        std::bind(&Client::readName_, this)
    );

    std::unique_ptr<Reader> reader_;
    std::unique_ptr<Displayer> displayer_;

    std::unique_ptr<AppCheckResponder> checkResponder_;

//MARK: - Public fields
public:
    std::weak_ptr<Logger> logger;

//MARK: - Overrides methods of ConnectionManagerDelegate interface
public:
    void ifLostConnection(ConnectionManager &manager,
                          const std::string_view errorMsg) override {
        logIfCan_(errorMsg.data(), LoggerMessageType::ERROR);
        displayer_->display(errorMsg);

        manager.close();
        stop();
    }

    void ifDataIsAvailable(ConnectionManager &manager) override {
        auto message = manager.read();
        if (!message.has_value()) return;

        if (MessageType::isMessageType<ConnectionMessageType>(message.value())) {
            logIfCan_("Got connection message from server.", LoggerMessageType::ERROR);
            strangeServer();
        } else if (BasicMessageHandler::isError(message.value().header)) {
            logIfCan_("Got error from server:", LoggerMessageType::ERROR);
            logIfCan_(message.value(), LoggerMessageType::ERROR);
            displayer_->display(message.value());

            stop();
        } else {
            logIfCan_("Got usual message.", LoggerMessageType::INFO);
            displayer_->display(message.value());
        }
    }

//MARK: - Overrides methods of ReaderDelegate interface
public:
    void ifDataAvailable() override {
        auto input = reader_->read();
        if (!input.has_value()) return;

        auto data = convertToVector_(input.value());
        Message<RegularMessageType> message = {
            .header = {
                .typeOption = RegularMessageType::TEXT_MESSAGE,
                .size = static_cast<uint32_t>(data.size())
            },
            .data = std::move(data)
        };

        logIfCan_("Read input from user. Send message.", LoggerMessageType::INFO);
        connection_.send(MessageType::convertToUniversal(std::move(message)));
    }

//MARK: - Constructor and public methods
public:
    Client(Executor &executor,
           std::unique_ptr<Connection> connection,
           std::unique_ptr<Reader> reader,
           std::unique_ptr<Displayer> displayer,
           std::unique_ptr<AppCheckResponder> checkResponder)
            : executor_{ executor }, connection_{ std::move(connection) },
              reader_{ std::move(reader) }, displayer_{ std::move(displayer) },
              checkResponder_{ std::move(checkResponder) } {
        connection_.delegate = std::weak_ptr(requestGetter_);
        reader_->delegate = std::weak_ptr(nameGetter_);

        logIfCan_("Started sending heartbeat.", LoggerMessageType::INFO);
        connection_.startSendingHeartbeat(executor);
    }

    void run() {
        logIfCan_("Client is about to run.", LoggerMessageType::INFO);
        executor_.run();

        // User input will be sended to nameGetter_
        displayer_->display("Welcom to the crossplatform chat! Please, enter "
                            "your name:");
    }

    void stop() {
        auto message = MessageType::convertToUniversal(
            Message<ConnectionMessageType> {
                .header = {
                    .typeOption = ConnectionMessageType::DISCONNECT,
                    .size = 0
                }
            }
        );

        connection_.send(std::move(message));
        connection_.stopSendingHeartbeat();

        displayer_->display("Client is being stopped.");
        logIfCan_("Client is being stopped.", LoggerMessageType::INFO);

        // If disconnect message won't send, it's not problem. Server will close
        // the connection due to the absence of heartbeat messages.
        connection_.close();
        executor_.stop();

        displayer_->display("Bye!");
        logIfCan_("Client is stopped.", LoggerMessageType::INFO);
    }

//MARK: - Callbacks for requestGetter_ and nameGetter_
private:
    void getCheckRequest_(ConnectionManager &manager) {
        auto message = manager.read();
        if (!message.has_value()) return;

        if (ConnectionMessageHandler::isCheckApp(message.value().header)) {
            auto request = MessageType::convertToTyped<ConnectionMessageType>(std::move(message.value()));
            auto response = checkResponder_->createResponse(request.value());
            auto universalResponse = MessageType::convertToUniversal(std::move(response));

            logIfCan_("Got check request. Send response.", LoggerMessageType::INFO);
            connection_.send(universalResponse);

            // Change to the normal connection delegate
            connection_.delegate = this->weak_from_this();
        } else {
            logIfCan_("First of all, didn't get check request.", LoggerMessageType::ERROR);
            strangeServer();
        }
    }

    void readName_() {
        auto name = reader_->read();
        if (!name.has_value()) return;

        name_ = std::move(name.value());

        // Change to the normal reader delegate
        reader_->delegate = this->weak_from_this();
    }

//MARK: - Methods that make work more convenient
private:
    void logIfCan_(const char *message, const LoggerMessageType type) {
        if (auto loggerPtr = logger.lock())
            loggerPtr->log(message, type);
    }

    void logIfCan_(const UniversalMessage &message, const LoggerMessageType type) {
        if (auto loggerPtr = logger.lock()) {
            auto text = MessageType::getTextFrom(message);
            loggerPtr->log(text.data(), type);
        }
    }

    std::vector<uint8_t> convertToVector_(const std::string &str) {
        auto vector = std::vector<uint8_t>(str.length() + 1);
        std::memcpy(vector.data(), str.c_str(), str.length() + 1);

        return vector;
    }

    void strangeServer() {
        logIfCan_("Will stop work of client because of strange behaviour of server.",
                  LoggerMessageType::INFO);
        displayer_->display("The behaviour of server is strange. We will stop "
                            "work of application to protect you from that server.");

        stop();
    }
};
