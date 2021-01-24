#pragma once
#include "BussDevice.h"

// n is the actual size, s is the address range (including mirrors)
template<int n, int s>
class Ram : public BussDevice {
public:
    virtual uint8_t read(const uint16_t& address) override {
        if(address < s) {
            return memory[address % n];
        }
        return 0;
    }
    virtual void write(const uint16_t& address, const uint8_t& data) override {
        if(address < s) {
            memory[address % n] = data;
        }
    }
    virtual uint16_t getSize() override { return s; }

private:
    uint8_t memory[n];
};