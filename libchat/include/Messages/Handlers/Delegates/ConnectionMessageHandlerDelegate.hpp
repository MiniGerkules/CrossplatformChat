#pragma once

class ConnectionMessageHandlerDelegate {
public:
    virtual ~ConnectionMessageHandlerDelegate() = default;

    virtual void messageToConnect() = 0;
    virtual void messageToDisconnect() = 0;
    virtual void messageToCheckApp() = 0;
};
