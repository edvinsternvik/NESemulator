#include "Cartridge.h"
#include "Mappers/Mapper.h"

Cartridge::Cartridge(const char* filepath) {
    m_successfulLoad = false;
}

bool Cartridge::successfulLoad() {
    return m_successfulLoad;
}

uint8_t Cartridge::readCPU(const uint16_t& address) {
    if(m_mapper.get()) {
        uint16_t mappedAddress;
        if(m_mapper->mapReadCPU(address, mappedAddress)) {
            return m_PRGrom[mappedAddress];
        }
    }
    return 0;
}

void Cartridge::writeCPU(const uint16_t& address, const uint8_t& data) {
    if(m_mapper.get()) {
        uint16_t mappedAddress;
        if(m_mapper->mapWriteCPU(address, mappedAddress)) {
            m_PRGrom[mappedAddress] = data;
        }
    }
}

uint8_t Cartridge::readPPU(const uint16_t& address) {
    if(m_mapper.get()) {
        uint16_t mappedAddress;
        if(m_mapper->mapReadPPU(address, mappedAddress)) {
            return m_CHRrom[mappedAddress];
        }
    }
    return 0;
}

void Cartridge::writePPU(const uint16_t& address, const uint8_t& data) {
    if(m_mapper.get()) {
        uint16_t mappedAddress;
        if(m_mapper->mapWritePPU(address, mappedAddress)) {
            m_CHRrom[mappedAddress] = data;
        }
    }
}