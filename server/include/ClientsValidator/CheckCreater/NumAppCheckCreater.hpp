#pragma once

#include <random>

#include <VectorExtensions.hpp>

#include "AppCheckCreater.hpp"

class NumAppCheckCreater : public AppCheckCreater {
public:
    Message<ConnectionMessageType> createCheck() override {
        std::random_device dev;
        std::mt19937_64 random(dev());

        auto generatedNum = static_cast<uint64_t>(random());
        auto message = Message<ConnectionMessageType> {
            .header = {
                .typeOption = ConnectionMessageType::CHECK_APP,
                .size = sizeof(generatedNum)
            },
            .data = VectorExtensions::convertToVector(generatedNum)
        };

        return message;
    }
};
