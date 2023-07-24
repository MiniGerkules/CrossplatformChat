#pragma once

class Connection;

class ConnectionDelegate {
public:
    virtual void ifLostConnection(const std::string &what) = 0;
    virtual void ifDataIsAvailable() = 0;
};
