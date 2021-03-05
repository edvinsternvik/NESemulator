#pragma once
#include "BussDevice.h"

template<int n>
class Ram : public BussDevice {
public:
    virtual uint8_t read(const uint16_t& address) override {
        return memory[address % n];
        return 0;
    }
    virtual void write(const uint16_t& address, const uint8_t& data) override {
        memory[address % n] = data;
    }

private:
    uint8_t memory[n];
};