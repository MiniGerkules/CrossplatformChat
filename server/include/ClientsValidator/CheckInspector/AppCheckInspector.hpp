#pragma once

#include <Messages/Message.hpp>
#include <Messages/Types/ConnectionMessageType.hpp>

class AppCheckInspector {
public:
    virtual ~AppCheckInspector() = default;

    virtual bool isCompatibleApp(const Message<ConnectionMessageType>& check,
                                 const Message<ConnectionMessageType>& respond) = 0;
};
