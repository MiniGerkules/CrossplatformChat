#pragma once

class Connection;

class ConnectionDelegate {
public:
    virtual void ifLostConnection(Connection connection) = 0;
    virtual void ifDataIsAvailable(Connection connection) = 0;
};
