#pragma once

class IEncoder {
public:
    virtual int readDelta() = 0;
    virtual bool is_pressed() = 0;
    virtual ~IEncoder() {}
};
