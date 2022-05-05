#pragma once

class Runnable {
public:
    Runnable() = default;
    virtual ~Runnable() = default;

    virtual void run() = 0;
};
