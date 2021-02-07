#pragma once
#include "BussDevice.h"
#include <cstdint>
#include <memory>

class Buss;

class PPU {
public:
    void registerBuss(Buss* buss);

public:
    uint8_t registers[8] = {};

private:
    Buss* m_buss;
};

class PPUregisters : public BussDevice {
public:
    PPUregisters(std::shared_ptr<PPU> ppu) : m_ppu(ppu) {}

    virtual uint8_t read(const uint16_t& address) override {
        return m_ppu->registers[address % 8];
    }
    virtual void write(const uint16_t& address, const uint8_t& data) override {
        m_ppu->registers[address % 8] = data;
    }
    virtual uint16_t getSize() override { return 0x2000; }

private:
    std::shared_ptr<PPU> m_ppu;
};