#pragma once
#include <cstdint>
#include <memory>

class Cartridge;

class Buss {
public:
    void insertCartridge(std::shared_ptr<Cartridge> cartridge);

    uint8_t read(const uint16_t& address);
    void write(const uint16_t& address, const uint8_t& data);

private:
    uint8_t m_ram[0x0800];
    std::shared_ptr<Cartridge> m_cartridge;
};