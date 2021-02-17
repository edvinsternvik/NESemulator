#include "PPU.h"
#include "Buss.h"

PPU::PPU()
    : m_scanline(261), m_cycle(0), m_nmi(false), m_oddFrame(false) {
}

void PPU::clock() {
    if(m_scanline < 240) { // Visible scanlines
        if(0 < m_cycle && m_cycle < 257) {
            if(PPUmask.showBg) {
                if(m_cycle % 8 == 1) { // Every 8 cycles, starting at the first cycle in the range 1-256 (1, 9, 17 etc)
                    uint8_t nametableByte = fetchNametableByte();
                    fetchPatternTableTile(nametableByte);
                    m_vramAddress.coarseX++;
                }
                
                uint8_t pixel = fetchNextPixel();

                setPixel(pixel);
                m_pixelOffset++;
            }
        }
        else if(m_cycle == 257) {
            if(PPUmask.showBg) {
                incVramAddrVertical();
                m_vramAddress.coarseX = m_tvramAddress.coarseX;
                // Todo - fine x
            }
        }
        else if(m_cycle < 321) {
        }
        else if(m_cycle < 337) {
        }
        else if(m_cycle < 341) {
        }
    }
    else if(m_scanline == 240) { // Post render scanline
    }
    else if(m_scanline < 261) { // Vertical blanking lines
        if(m_scanline == 241 && m_cycle == 1) {
            PPUstatus.verticalBlank = 1;

            if(PPUctrl.genNMI) {
                m_nmi = true;
            }
        }
    }
    else { // Pre render scanline
        if(m_cycle == 1) {
            PPUstatus.verticalBlank = 0;
        }
        else if(m_cycle > 279 && m_cycle < 305) {
            if(PPUmask.showBg) {
                m_vramAddress.fineY = m_tvramAddress.fineY;
                m_vramAddress.coarseY = m_tvramAddress.coarseY;
            }
        }
    }
    m_cycle++;

    if(m_cycle == 341) {
        m_cycle = 0;

        if(m_scanline == 261) {
            m_scanline = 0;
            frameDone = true;
            m_pixelOffset = 0;
        }
        else m_scanline++;
    }
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

uint8_t PPU::fetchNametableByte() {
    // vram address layout:
    // yyy NN YYYYY XXXXX - y = fine y scroll, N = nametable, Y = coarse y, X = coarse x
    uint16_t nameTableAddr = m_vramAddress.vramAddr & 0x0FFF; // Don't include fine y scroll
    
    return read(0x2000 + nameTableAddr);
}

void PPU::fetchPatternTableTile(uint8_t nametableByte) {
    uint16_t bgPTableOffset = PPUctrl.bgPatternTableAddr ? 0x1000 : 0x0000;
    uint16_t tileOffset = (uint16_t)nametableByte * 16; // Each pattern table entry is 16 bytes
    uint16_t tileAddress = bgPTableOffset + tileOffset + (uint16_t)m_vramAddress.fineY;

    uint8_t newLow = read(tileAddress);
    uint8_t newHigh = read(tileAddress + 8);
    
    m_pTableDataLow  = (m_pTableDataLow  & 0xFF00) | newLow;
    m_pTableDataHigh = (m_pTableDataHigh & 0xFF00) | newHigh;
}

uint8_t PPU::fetchNextPixel() {
    int8_t pixel = 0;
    pixel |= (m_pTableDataLow & 0x80) >> 7; // low
    pixel |= (m_pTableDataHigh & 0x80) >> 6; // high

    m_pTableDataLow <<= 1;
    m_pTableDataHigh <<= 1;

    return pixel;
}

void PPU::incVramAddrVertical() {
    if(m_vramAddress.fineY == 7) m_vramAddress.coarseY++;
    m_vramAddress.fineY++;
}

void PPU::setPixel(uint8_t pixelValue) {
    float p = (float)pixelValue / 3.0;
    p *= 255.0;
    unsigned int pixelVal = 0xFF000000 | ((unsigned int)p << 16) | ((unsigned int)p << 8) | (unsigned int)p;
    *((unsigned int*)(windowBuffer + m_pixelOffset)) = pixelVal;
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
        m_dataBuffer = m_ppu->read(m_ppu->m_vramAddress.vramAddr);

        // If reading palette data ($3F00 - $3FFF), read directly, bypass the data buffer
        if(m_ppu->m_vramAddress.vramAddr >= 0x3F00) data = m_dataBuffer;

        if(m_ppu->PPUctrl.vramInc) m_ppu->m_vramAddress.vramAddr += 32;
        else m_ppu->m_vramAddress.vramAddr += 1;
        break;
    }

    return data;
}

void PPUregisters::write(const uint16_t& address, const uint8_t& data) {
    uint8_t localAddr = address % 8;

    switch(localAddr) {
    case 0:
        m_ppu->PPUctrl.reg = data;
        m_ppu->m_tvramAddress.nametable = m_ppu->PPUctrl.nametableAddr;
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
            m_ppu->m_tvramAddress.coarseX = data >> 3;
            // Todo - fine x scroll
            m_addressLatch = true;
        }
        else {
            // Write vertical scroll
            m_ppu->m_tvramAddress.coarseY = data >> 3;
            m_ppu->m_tvramAddress.fineY = data & 0b111;
            m_addressLatch = false;
        }
        break;
    case 6:
        if(!m_addressLatch) {
            m_ppu->m_tvramAddress.vramAddr = (uint16_t)((data & 0x3F) << 8 | (m_ppu->m_tvramAddress.vramAddr & 0x00FF));
            m_addressLatch = true;
        }
        else {
            m_ppu->m_tvramAddress.vramAddr = (m_ppu->m_tvramAddress.vramAddr & 0xFF00) | data;
            m_ppu->m_vramAddress = m_ppu->m_tvramAddress;
            m_addressLatch = false;
        }
        break;
    case 7:
        m_ppu->write(m_ppu->m_vramAddress.vramAddr, data);

        if(m_ppu->PPUctrl.vramInc) m_ppu->m_vramAddress.vramAddr += 32;
        else m_ppu->m_vramAddress.vramAddr += 1;
        break;
    }
}