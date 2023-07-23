#pragma once

class Connection;

class ConnectionDelegate {
public:
    virtual void ifLostConnection(const Connection &connection, std::string what) = 0;
    virtual void ifDataIsAvailable(const Connection &connection) = 0;
};
