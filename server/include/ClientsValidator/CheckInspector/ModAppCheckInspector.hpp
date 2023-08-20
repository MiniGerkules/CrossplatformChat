#pragma once

#include <VectorExtensions.hpp>
#include <Functions/ModFunction.hpp>

#include "AppCheckInspector.hpp"

class ModAppCheckInspector final : AppCheckInspector {
public:
    bool isCompatibleApp(const Message<ConnectionMessageType>& check,
                         const Message<ConnectionMessageType>& respond) {
        using ChatConsts::NumCheckType;

        auto checkNum = VectorExtensions::convertToType<NumCheckType>(check.data);
        auto respondNum = VectorExtensions::convertToType<NumCheckType>(respond.data);

        return CheckFunctions::modFunction(checkNum) == respondNum;
    }
};
