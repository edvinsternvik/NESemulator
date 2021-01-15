#pragma once
#include <cstdint>

class Cartridge {
public:
    uint8_t read(const uint16_t& address) { return 0; }
    void write(const uint16_t& address, const uint8_t& data) {}
};