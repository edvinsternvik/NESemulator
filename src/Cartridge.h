#pragma once
#include <cstdint>
#include <memory>
#include <vector>

class Mapper;

class Cartridge {
public:
    Cartridge(const char* filepath);

    bool successfulLoad();
    uint8_t readCPU(const uint16_t& address);
    void writeCPU(const uint16_t& address, const uint8_t& data);

    uint8_t readPPU(const uint16_t& address);
    void writePPU(const uint16_t& address, const uint8_t& data);

private:
    bool m_successfulLoad;

    std::vector<uint8_t> m_PRGrom;
    std::vector<uint8_t> m_CHRrom;
    std::unique_ptr<Mapper> m_mapper;
};