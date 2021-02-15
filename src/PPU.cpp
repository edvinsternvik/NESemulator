#include "PPU.h"
#include "Buss.h"

PPU::PPU()
    : m_scanline(261), m_cycle(0), m_nametableOffset(0), m_nmi(0) {
}

void PPU::clock() {
}

void PPU::registerBuss(Buss* buss) {
    m_buss = buss;
}

bool PPU::nmi() {
    if(m_nmi) {
        m_nmi = false;
        return true;
    }
    return false;
}

uint8_t PPU::read(const uint16_t& address) {
    if(m_buss) {
        return m_buss->read(address);
    }
    return 0;
}

void PPU::write(const uint16_t& address, const uint8_t& data) {
    if(m_buss) {
        m_buss->write(address, data);
    }
}

uint8_t PPUregisters::read(const uint16_t& address) {
    uint8_t localAddr = address % 8;
    uint8_t data = 0;

    switch(localAddr) {
    case 0: // Write only
        break;
    case 1: // Write only
        break;
    case 2:
        data = m_ppu->PPUstatus.reg & 0xE0; // Low 5 bits are unused
        m_ppu->PPUstatus.verticalBlank = 0;
        m_addressLatch = false;
        break;
    case 3: // Write only
        break;
    case 4:
        break;
    case 5: // Write only
        break;
    case 6: // Write only
        break;
    case 7:
        data = m_dataBuffer;
        m_dataBuffer = m_ppu->read(m_ppu->m_vramAddress);

        // If reading palette data ($3F00 - $3FFF), read directly, bypass the data buffer
        if(m_ppu->m_vramAddress >= 0x3F00) data = m_dataBuffer;

        if(m_ppu->PPUctrl.vramInc) m_ppu->m_vramAddress += 32;
        else m_ppu->m_vramAddress += 1;
        break;
    }

    return data;
}

void PPUregisters::write(const uint16_t& address, const uint8_t& data) {
    uint8_t localAddr = address % 8;

    switch(localAddr) {
    case 0:
        m_ppu->PPUctrl.reg = data;
        break;
    case 1:
        m_ppu->PPUmask.reg = data;
        break;
    case 2: // Read only
        break;
    case 3:
        break;
    case 4:
        break;
    case 5:
        if(!m_addressLatch) {
            // Write horizontal scroll
            m_addressLatch = true;
        }
        else {
            // Write vertical scroll
            m_addressLatch = true;
        }
        break;
    case 6:
        if(!m_addressLatch) {
            m_ppu->m_tvramAddress = (uint16_t)((data & 0x3F) << 8 | (m_ppu->m_tvramAddress & 0x00FF));
            m_addressLatch = true;
        }
        else {
            m_ppu->m_tvramAddress = (m_ppu->m_tvramAddress & 0xFF00) | data;
            m_ppu->m_vramAddress = m_ppu->m_tvramAddress;
            m_addressLatch = false;
        }
        break;
    case 7:
        m_ppu->write(m_ppu->m_vramAddress, data);

        if(m_ppu->PPUctrl.vramInc) m_ppu->m_vramAddress += 32;
        else m_ppu->m_vramAddress += 1;
        break;
    }
}