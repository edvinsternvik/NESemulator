#include "Cartridge.h"
#include "Mappers/NROM.h"
#include <fstream>

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
    m_PRGrom.reserve(prgRomSize);
    m_CHRrom.reserve(chrRomSize);

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

    file.read((char*)m_PRGrom.data(), prgRomSize);
    file.read((char*)m_CHRrom.data(), chrRomSize);
    file.close();

    m_successfulLoad = true;
}

bool Cartridge::successfulLoad() {
    return m_successfulLoad;
}

uint8_t Cartridge::readCPU(const uint16_t& address) {
    if(m_successfulLoad) {
        uint16_t mappedAddress;
        if(m_mapper->mapReadCPU(address, mappedAddress)) {
            return m_PRGrom[mappedAddress];
        }
    }
    return 0;
}

void Cartridge::writeCPU(const uint16_t& address, const uint8_t& data) {
    if(m_successfulLoad) {
        uint16_t mappedAddress;
        if(m_mapper->mapWriteCPU(address, mappedAddress)) {
            m_PRGrom[mappedAddress] = data;
        }
    }
}

uint8_t Cartridge::readPPU(const uint16_t& address) {
    if(m_successfulLoad) {
        uint16_t mappedAddress;
        if(m_mapper->mapReadPPU(address, mappedAddress)) {
            return m_CHRrom[mappedAddress];
        }
    }
    return 0;
}

void Cartridge::writePPU(const uint16_t& address, const uint8_t& data) {
    if(m_successfulLoad) {
        uint16_t mappedAddress;
        if(m_mapper->mapWritePPU(address, mappedAddress)) {
            m_CHRrom[mappedAddress] = data;
        }
    }
}

bool Cartridge::setMapper(const uint8_t& mapperID, const uint32_t& prgRomSize, const uint32_t& chrRomSize) {
    switch(mapperID) {
    case 0:
        m_mapper = std::make_unique<NROM>(prgRomSize, chrRomSize);
        return true;
    }
    return false;
}