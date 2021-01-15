#include "Buss.h"
#include "Cartridge.h"

void Buss::insertCartridge(std::shared_ptr<Cartridge> cartridge) {
    m_cartridge = cartridge;
}

uint8_t Buss::read(const uint16_t& address) {
    if(address < 0x2000) {
        // address & 0000011111111111, because $0800-$1FFF are mirrors of $0000-$07FF.
        return m_ram[address & 0x07FF];
    }
    else if(address >= 0x4020 && address <= 0xFFFF) {
        if(m_cartridge.get()) {
            return m_cartridge->read(address - 0x4020);
        }
    }
    return 0;
}

void Buss::write(const uint16_t& address, const uint8_t& data) {
    if(address < 0x2000) {
        // address & 0000011111111111, because $0800-$1FFF are mirrors of $0000-$07FF.
        m_ram[address & 0x07FF] = data;
    }
    else if(address >= 0x4020 && address <= 0xFFFF) {
        if(m_cartridge.get()) {
            m_cartridge->write(address - 0x4020, data);
        }
    }
}