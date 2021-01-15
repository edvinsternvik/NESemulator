#pragma once
#include <cstdint>
#include <memory>

class Mapper;

class Cartridge {
public:
    Cartridge(const char* filepath);

    bool successfulLoad();
    uint8_t read(const uint16_t& address);
    void write(const uint16_t& address, const uint8_t& data);

private:
    bool m_successfulLoad;
    std::unique_ptr<Mapper> m_mapper;
};