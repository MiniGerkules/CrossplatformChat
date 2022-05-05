#pragma once
#include <algorithm>
#include <vector>
#include <cstring>
#include <type_traits>

template <typename T>
struct MessageHeader {
    T id;
    uint32_t size = 0;
};

template <typename T>
struct Message {
    MessageHeader<T> header;
    std::vector<uint8_t> body;

    size_t size() {
        return body.size();
    }

    friend Message& operator<<(Message& msg, const std::string& data) {
        size_t oldSize = msg.body.size();
        msg.body.resize(oldSize + data.length() + 1);
        std::memcpy(msg.body.data() + oldSize, data.c_str(), data.length() + 1);
        msg.header.size = msg.size();

        return msg;
    }

    template <typename DataType>
    friend Message& operator<<(Message& msg, const DataType& data) {
        // Checks that the type of the data is trivially copyable
        static_assert(std::is_standard_layout<DataType>::value, "Type is too complex to be pushed");

        size_t oldSize = msg.body.size();
        msg.body.resize(oldSize + sizeof(DataType));
        std::memcpy(msg.body.data() + oldSize, data, sizeof(DataType));
        msg.header.size = size();

        return msg;
    }

    friend Message& operator>>(Message& msg, std::string& toReturn) {
        if (msg.body.size() == 0)
            return msg;

        auto pos = std::find(msg.body.rbegin() + 1, msg.body.rend(), '\0');
        size_t length;
        if (pos == msg.body.rend())
            length = msg.body.size();
        else
            length = std::distance(pos.base(), msg.body.end());

        std::unique_ptr<char[]> data = std::make_unique<char[]>(length);
        size_t newBodySize = msg.body.size() - length;
        std::memcpy(data.get(), msg.body.data() + newBodySize, length);
        toReturn = data.release();

        msg.body.resize(newBodySize);
        msg.header.size = msg.size();

        return msg;
    }

    template <typename DataType>
    friend Message& operator>>(Message& msg, DataType& toReturn) {
        // Checks that the type of the data is trivially copyable
        static_assert(std::is_standard_layout<DataType>::value, "Type is too complex to be pushed");

        if (msg.body.size() == 0)
            return msg;

        size_t newBodySize = msg.body.size() - sizeof(DataType);
        std::memcpy(&toReturn, msg.body.data() + newBodySize, sizeof(DataType));
        msg.body.resize(newBodySize);
        msg.header.size = msg.size();

        return msg;
    }

    friend std::ostream& operator<<(std::ostream& stream, const Message& msg) {
        stream << "ID: " << header.id << ". Size: " << msg.header.size;
        return stream;
    }
};

template <typename T>
class Connection;

template <typename T>
struct OwnedMessage {
    std::shared_ptr<Connection<T>> remoute;
    Message<T> msg;

    friend std::ostream& operator<<(std::ostream& stream, const OwnedMessage<T>& ownedMsg) {
        stream << ownedMsg.msg;
        return stream;
    }
};
