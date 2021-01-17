#include "NROM.h"

NROM::NROM(const uint32_t& PRGromSize, const uint32_t& CHRromSize) : Mapper(PRGromSize, CHRromSize) {
}

bool NROM::mapReadCPU(const uint16_t& address, uint16_t& mappedAddress) {
    if(address < 0x8000) return false;

    if(m_PRGromSize == 0x4000) {
        mappedAddress = address & 0x3FFF; // Mirror $8000-$BFFF to $C000-$FFFF
    }
    else {
        mappedAddress = address & 0x7FFF;
    }
    return true;
}

bool NROM::mapWriteCPU(const uint16_t& address, uint16_t& mappedAddress) {
    return false;
}

bool NROM::mapReadPPU(const uint16_t& address, uint16_t& mappedAddress) {
    if(address >= 0x2000) return false;

    mappedAddress = address;
    return true;
}

bool NROM::mapWritePPU(const uint16_t& address, uint16_t& mappedAddress) {
    return false;
}
