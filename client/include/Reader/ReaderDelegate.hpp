#pragma once

class ReaderDelegate {
public:
    virtual ~ReaderDelegate() = default;

    virtual void ifDataAvailable() = 0;
};
