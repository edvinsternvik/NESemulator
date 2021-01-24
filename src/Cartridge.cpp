#include "Cartridge.h"
#include "Mappers/NROM.h"
#include <fstream>

#define CARTRIDGE_START_CPU 0x4020
#define CARTRIDGE_START_PPU 0x0000

uint8_t PRGrom::read(const uint16_t& address) {
    if(m_mapper.get()) {
        uint16_t mappedAddress;
        if(m_mapper->mapReadCPU(address + CARTRIDGE_START_CPU, mappedAddress)) { // Mappers use absolute address
            return m_rom[mappedAddress];
        }
    }
    return 0;
}
void PRGrom::write(const uint16_t& address, const uint8_t& data) {
    if(m_mapper.get()) {
        uint16_t mappedAddress;
        if(m_mapper->mapWriteCPU(address + CARTRIDGE_START_CPU, mappedAddress)) { // Mappers use absolute address
            m_rom[mappedAddress] = data;
        }
    }
}

uint8_t CHRrom::read(const uint16_t& address) {
    if(m_mapper.get()) {
        uint16_t mappedAddress;
        if(m_mapper->mapReadPPU(address + CARTRIDGE_START_PPU, mappedAddress)) { // Mappers use absolute address
            return m_rom[mappedAddress];
        }
    }
    return 0;
}

void CHRrom::write(const uint16_t& address, const uint8_t& data) {
    if(m_mapper.get()) {
        uint16_t mappedAddress;
        if(m_mapper->mapWritePPU(address + CARTRIDGE_START_PPU, mappedAddress)) { // Mappers use absolute address
            m_rom[mappedAddress] = data;
        }
    }
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

    prgRom = std::make_shared<PRGrom>();
    chrRom = std::make_shared<CHRrom>();

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
    prgRom->m_rom.reserve(prgRomSize);
    chrRom->m_rom.reserve(chrRomSize);

    uint8_t mapperID = 0;
    mapperID |= header.flags6 >> 4;
    mapperID |= header.flags7 & 0xF0;

    if(!setMapper(mapperID, prgRomSize, chrRomSize)) {
        file.close();
        return; // Return if the mapper isn't supported
    }

    bool hasTrainer = header.flags6 & 0x04;
    if(hasTrainer) { // Skip trainer
        file.seekg(512, std::ios_base::cur);
    }

    file.read((char*)prgRom->m_rom.data(), prgRomSize);
    file.read((char*)chrRom->m_rom.data(), chrRomSize);
    file.close();

    m_successfulLoad = true;
}

bool Cartridge::successfulLoad() {
    return m_successfulLoad;
}

bool Cartridge::setMapper(const uint8_t& mapperID, const uint32_t& prgRomSize, const uint32_t& chrRomSize) {
    std::shared_ptr<Mapper> mapper;
    switch(mapperID) {
    case 0:
        mapper = std::make_unique<NROM>(prgRomSize, chrRomSize);
        break;
    default:
        return false;
    }

    prgRom->m_mapper = mapper;
    chrRom->m_mapper = mapper;

    return true;
}