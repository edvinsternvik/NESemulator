#include "Cartridge.h"
#include "Mappers/Mapper.h"

Cartridge::Cartridge(const char* filepath) {
    m_successfulLoad = false;
}

bool Cartridge::successfulLoad() {
    return m_successfulLoad;
}

uint8_t Cartridge::read(const uint16_t& address) {
    if(m_mapper.get()) {
        return m_mapper->read(address);
    }
    return 0;
}

void Cartridge::write(const uint16_t& address, const uint8_t& data) {
    if(m_mapper.get()) {
        m_mapper->write(address, data);
    }
}