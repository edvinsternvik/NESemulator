#pragma once
#include <inttypes.h>

class Mapper {
public:
    virtual uint8_t read(const uint16_t& address) = 0;
    virtual void write(const uint16_t& address, const uint8_t& data) = 0;
};