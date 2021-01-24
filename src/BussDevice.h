#pragma once
#include <cstdint>

class BussDevice {
public:
    virtual uint8_t read(const uint16_t& address) = 0;
    virtual void write(const uint16_t& address, const uint8_t& data) = 0;
    virtual uint16_t getSize() = 0;
};