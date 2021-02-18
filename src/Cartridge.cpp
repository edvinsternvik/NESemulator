#include "Cartridge.h"
#include "Mappers/NROM.h"
#include <fstream>

#define CARTRIDGE_START_CPU 0x4020
#define CARTRIDGE_START_PPU 0x0000

CartridgeCPU::CartridgeCPU(std::shared_ptr<Mapper> mapper) : m_mapper(mapper) {}

uint8_t CartridgeCPU::read(const uint16_t& address) {
    return m_mapper->readCPU(address + CARTRIDGE_START_CPU); // Mappers use absolute address
}

void CartridgeCPU::write(const uint16_t& address, const uint8_t& data) {
    m_mapper->writeCPU(address + CARTRIDGE_START_CPU, data); // Mappers use absolute address
}

CartridgePPU::CartridgePPU(std::shared_ptr<Mapper> mapper) : m_mapper(mapper) {}

uint8_t CartridgePPU::read(const uint16_t& address) {
    return m_mapper->readPPU(address + CARTRIDGE_START_PPU); // Mappers use absolute address
}

void CartridgePPU::write(const uint16_t& address, const uint8_t& data) {
    m_mapper->writePPU(address + CARTRIDGE_START_PPU, data); // Mappers use absolute address
}


Cartridge::Cartridge(const char* filepath) {
    struct iNESheader {
        uint8_t NES[4];
        uint8_t prgRomChunks;
        uint8_t chrRomChunks;
        uint8_t flags6;
        uint8_t flags7;
        uint8_t flags8;
        uint8_t flags9;
        uint8_t flags10;
        uint8_t padding[5];
    };
    iNESheader header;

    m_successfulLoad = false;
    std::ifstream file(filepath, std::ios::in | std::ios::binary);
    if(!file.is_open()) {
        return;
    }

    file.read((char*)&header, sizeof(iNESheader));
    if(!file) {
        file.close();
        return; // Exit if the entire header could not be read
    }
    if(header.NES[0] != 0x4E || header.NES[1] != 0x45 || header.NES[2] != 0x53 || header.NES[3] != 0x1A) {
        file.close();
        return; // Exit if the file doesn't begin with 'N', 'E', 'S', (MS-DOS end-of-file)
    }

    uint32_t prgRomSize = header.prgRomChunks * 0x4000; // Each chunk is 16 KiB
    uint32_t chrRomSize = header.chrRomChunks * 0x2000; // Each chunk is 8 KiB

    uint8_t mapperID = 0;
    mapperID |= header.flags6 >> 4;
    mapperID |= header.flags7 & 0xF0;

    std::shared_ptr<Mapper> mapper = getMapper(mapperID, prgRomSize, chrRomSize);
    if(mapper.get() == nullptr) {
        file.close();
        return; // Return if the mapper isn't supported
    }

    bool hasTrainer = header.flags6 & 0x04;
    if(hasTrainer) { // Skip trainer
        file.seekg(512, std::ios_base::cur);
    }

    cartridgeCPU = std::make_shared<CartridgeCPU>(mapper);
    cartridgePPU = std::make_shared<CartridgePPU>(mapper);
    file.read((char*)mapper->getPrgRomDataPtr(), prgRomSize);
    file.read((char*)mapper->getChrRomDataPtr(), chrRomSize);
    file.close();

    m_successfulLoad = true;
}

bool Cartridge::successfulLoad() {
    return m_successfulLoad;
}

std::shared_ptr<Mapper> Cartridge::getMapper(const uint8_t& mapperID, const uint32_t& prgRomSize, const uint32_t& chrRomSize) {
    std::shared_ptr<Mapper> mapper;
    switch(mapperID) {
    case 0: return std::make_unique<NROM>(prgRomSize, chrRomSize);
    }
    return std::shared_ptr<Mapper>();
}