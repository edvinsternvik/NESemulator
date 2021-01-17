#pragma once
#include "Mappers/Mapper.h"
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
    bool setMapper(const uint8_t& mapperID, const uint32_t& prgRomSize, const uint32_t& chrRomSize);

private:
    bool m_successfulLoad;

    std::vector<uint8_t> m_PRGrom;
    std::vector<uint8_t> m_CHRrom;
    std::unique_ptr<Mapper> m_mapper;
};