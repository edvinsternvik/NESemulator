#include "NROM.h"
#include "../Cartridge.h"

NROM::NROM(const uint32_t& PRGromSize, const uint32_t& CHRromSize) : Mapper(PRGromSize, CHRromSize) {
}

uint8_t NROM::readCPU(const uint16_t& address) {
    if(address < 0x8000) return 0;

    else if(m_PRGromSize == 0x4000) {
        return m_prgRom[address & 0x3FFF]; // Mirror $8000-$BFFF to $C000-$FFFF
    }
    else {
        return m_prgRom[address & 0x7FFF];
    }
}

void NROM::writeCPU(const uint16_t& address, const uint8_t& data) {
}

uint8_t NROM::readPPU(const uint16_t& address) {
    if(address >= 0x2000) return 0;

    return m_chrRom[address];
}

void NROM::writePPU(const uint16_t& address, const uint8_t& data) {
    
}