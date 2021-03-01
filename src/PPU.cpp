#include "PPU.h"
#include "Buss.h"

#define C Colour
PPU::PPU()
    : m_scanline(261), m_cycle(0), m_nmi(false), m_oddFrame(false), m_pixelOffset(0),
        m_palette{
            C(84,   84,  84), C(0,    30, 116), C(8,    16, 144), C(48,    0, 136), C(68,    0, 100), C(92,    0,  48), C(84,    4,   0), C(60,   24,   0), C(32,   42,   0), C(8,    58,   0), C(0,    64,   0), C(0,    60,   0), C(0,    50,  60), C(0,     0,   0),  C(0, 0, 0),  C(0, 0, 0),
            C(152, 150, 152), C(8,    76, 196), C(48,   50, 236), C(92,   30, 228), C(136,  20, 176), C(160,  20, 100), C(152,  34,  32), C(120,  60,   0), C(84,   90,   0), C(40,  114,   0), C(8,   124,   0), C(0,   118,  40), C(0,   102, 120), C(0,     0,   0),  C(0, 0, 0),  C(0, 0, 0),
            C(236, 238, 236), C(76,  154, 236), C(120, 124, 236), C(176,  98, 236), C(228,  84, 236), C(236,  88, 180), C(236, 106, 100), C(212, 136,  32), C(160, 170,   0), C(116, 196,   0), C(76,  208,  32), C(56,  204, 108), C(56,  180, 204), C(60,   60,  60),  C(0, 0, 0),  C(0, 0, 0),
            C(236, 238, 236), C(168, 204, 236), C(188, 188, 236), C(212, 178, 236), C(236, 174, 236), C(236, 174, 212), C(236, 180, 176), C(228, 196, 144), C(204, 210, 120), C(180, 222, 120), C(168, 226, 144), C(152, 226, 180), C(160, 214, 228), C(160, 162, 160),  C(0, 0, 0),  C(0, 0, 0)} {               
}

void PPU::clock() {
    if(m_scanline == 0 && m_cycle == 0 && m_oddFrame) m_cycle = 1;

    if(m_scanline == 261) { // Pre render scanline
        if(m_cycle == 1) {
            m_ppuStatus.layout.verticalBlank = 0;
        }
        else if(m_cycle > 279 && m_cycle < 305) {
            transferVramAddrVertical();
        }
    }

    if(m_scanline < 240 || m_scanline == 261) { // Visible and prerender scanlines
        if(m_cycle == 0); // Idle
        else if(m_cycle < 257) { // Rendering
            if(m_cycle % 8 == 1 && m_cycle != 1) { // cycle 9, 17, 25 etc
                updateShiftRegisters();
            }
            // Rendering is two tiles behind
            uint8_t tileX = m_vramAddress.layout.coarseX - 2;
            uint8_t tileY = m_vramAddress.layout.coarseY;

            uint8_t pixel = fetchNextPixel();
            if(m_scanline < 240) { // Visible scanlines only, render pixels
                setPixel(pixel, m_pixelOffset, tileX, tileY, m_pAttrData1);
                m_pixelOffset++;
            }
        }
        else if(m_cycle == 257) {
            updateShiftRegisters(); // Unused tile fetch
            incVramAddrVertical();
        }
        else if(m_cycle == 258) {
            transferVramAddrHorizontal();
        }
        else if(m_cycle < 321) { // Fetch sprites for next scanline

        }
        else if(m_cycle == 329 || m_cycle == 337) { // Fetch the first two tiles of the next scanline
            // We are not rendering, but we still need to shift the registers
            m_pTableDataHigh <<= 8;
            m_pTableDataLow <<= 8;
            
            updateShiftRegisters();
        }
        else if(m_cycle == 338 || m_cycle == 340) { // Unused nametable fetches
            fetchNametableByte();
        }
    }
    else if(m_scanline == 240) { // Idle
    }
    else if(m_scanline < 261) { // Vertical blanking lines
        if(m_scanline == 241 && m_cycle == 1) {
            m_ppuStatus.layout.verticalBlank = 1;

            if(m_ppuCtrl.layout.genNMI) {
                m_nmi = true;
            }
        }
    }

    m_cycle++;
    
    if(m_cycle == 341) {
        m_cycle = 0;
        
        m_scanline++;
        if(m_scanline > 261) {
            m_scanline = 0;
            m_oddFrame = !m_oddFrame;
            frameDone = true;
            m_pixelOffset = 0;
        }
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
        return m_buss->read(address & 0x3FFF);
    }
    return 0;
}

void PPU::write(const uint16_t& address, const uint8_t& data) {
    if(m_buss) {
        m_buss->write(address & 0x3FFF, data);
    }
}

uint8_t PPU::fetchNametableByte() {
    // vram address layout:
    // yyy NN YYYYY XXXXX - y = fine y scroll, N = nametable, Y = coarse y, X = coarse x
    uint16_t nameTableAddr = m_vramAddress.vramAddr & 0x0FFF; // Don't include fine y scroll
    
    return read(0x2000 + nameTableAddr);
}

void PPU::fetchAttributeData() {
    m_pAttrData1 = m_pAttrData2;

    // X >> 4 <=> X/16, each attribute byte corresponds to 16 tiles
    // Bitmask 0x03FF means we take only the coarse y and coarse x of the vramAddr
    // uint16_t localAttrTableAddr = (m_vramAddress.vramAddr & 0x03FF) >> 4;
    uint16_t localAttrTableAddr = (m_vramAddress.layout.coarseX / 4) + (m_vramAddress.layout.coarseY / 4) * 8;
    
    // Bitmask 0x0C00 means we take only the nametable offset
    // 0x03C0 is the offset from each nametable to their corresponding attribute tables
    uint16_t attrTableOffset = 0x2000 + (m_vramAddress.vramAddr & 0x0C00) + 0x03C0;
    
    m_pAttrData2 = read(attrTableOffset + localAttrTableAddr);
}

void PPU::fetchPatternTableTile(uint8_t nametableByte) {
    uint16_t bgPTableOffset = m_ppuCtrl.layout.bgPatternTableAddr ? 0x1000 : 0x0000;
    uint16_t tileOffset = (uint16_t)nametableByte << 4; // Each pattern table entry is 16 bytes, so nametableByte is multiplied by 16
    uint16_t tileAddress = bgPTableOffset + tileOffset + (uint16_t)m_vramAddress.layout.fineY;

    uint8_t newLow = read(tileAddress);
    uint8_t newHigh = read(tileAddress + 8);
    
    m_pTableDataLow  = (m_pTableDataLow  & 0xFF00) | newLow;
    m_pTableDataHigh = (m_pTableDataHigh & 0xFF00) | newHigh;
}

void PPU::updateShiftRegisters() {
    uint8_t nametableByte = fetchNametableByte();
    fetchAttributeData();
    fetchPatternTableTile(nametableByte);
    incVramAddrHorizontal();
}

// Returns the next pixel from pattern table data shift registers, and then shifts these registers
uint8_t PPU::fetchNextPixel() {
    int8_t pixel = 0;
    pixel |= (m_pTableDataLow & 0x8000) >> 15; // low
    pixel |= (m_pTableDataHigh & 0x8000) >> 14; // high

    m_pTableDataLow <<= 1;
    m_pTableDataHigh <<= 1;

    return pixel;
}

// Shift vram address down one row vertically
void PPU::incVramAddrVertical() {
    if(m_ppuMask.layout.showBg || m_ppuMask.layout.showSpr) {
        // fineY is only 3 bits, so it automatically wraps around to 0 after 7
        m_vramAddress.layout.fineY++;
        if(m_vramAddress.layout.fineY == 0) { // If fineY has wrapped around
            m_vramAddress.layout.coarseY++;

            // There are 32x30 tiles in the nametable, so when coarseY == 30 we have gone over
            // the visible area, and so we have to wrap back around
            if(m_vramAddress.layout.coarseY == 30) {
                m_vramAddress.layout.coarseY = 0;
                m_vramAddress.layout.nametableY = ~m_vramAddress.layout.nametableY; // Swtich nametable vertically
            }
        }
    }
}

// Shift vram address right one tile horizontally
void PPU::incVramAddrHorizontal() {
    if(m_ppuMask.layout.showBg || m_ppuMask.layout.showSpr) {
        // coarseX is only 5 bits, so it automatically wraps around to 0 after 31
        m_vramAddress.layout.coarseX++;

        if(m_vramAddress.layout.coarseX == 0) { // If coarseX has wrapped around
            m_vramAddress.layout.nametableX = ~m_vramAddress.layout.nametableX; // Swtich nametable horizontally
        }
    }
}

void PPU::transferVramAddrVertical() {
    if(m_ppuMask.layout.showBg || m_ppuMask.layout.showSpr) {
        m_vramAddress.layout.nametableY = m_tvramAddress.layout.nametableY;
        m_vramAddress.layout.coarseY = m_tvramAddress.layout.coarseY;
        m_vramAddress.layout.fineY = m_tvramAddress.layout.fineY;
    }
}

void PPU::transferVramAddrHorizontal() {
    if(m_ppuMask.layout.showBg || m_ppuMask.layout.showSpr) {
        m_vramAddress.layout.nametableX = m_tvramAddress.layout.nametableX;
        m_vramAddress.layout.coarseX = m_tvramAddress.layout.coarseX;
        // Todo fineX
    }
}

// Sets the pixel in the window buffer at pixelOffset. The provided pixelValue
// should be a number between 0 and 3 specifying a colour in the palette which we get from attributeData
void PPU::setPixel(uint8_t pixelValue, uint32_t pixelOffset, uint8_t tileX, uint8_t tileY, uint8_t attributeData) {
    if(m_ppuMask.layout.showBg || m_ppuMask.layout.showSpr) {
        uint8_t paletteIndex;
        if(pixelValue == 0) {
            paletteIndex = read(0x3F00); // Bit 0 of every palette is background, located at 0x3F00
        }
        else {
            uint8_t palette = getPalette(tileX, tileY, attributeData);
            uint16_t paletteOffset = 0x3F00 + palette * 4;
            paletteIndex = read(paletteOffset + pixelValue);
        }
        Colour c = m_palette[paletteIndex];
        unsigned int pixelVal = 0xFF000000 | (unsigned int)c.r << 16 | (unsigned int)c.g << 8 | (unsigned int)c.b;
        *((unsigned int*)(windowBuffer + pixelOffset)) = pixelVal;
    }
}

uint8_t PPU::getPalette(uint8_t tileX, uint8_t tileY, uint8_t attributeData) {
    // Attribute data layout:
    //      bits 0-1: Palette for top left quadrant of 4x4 tile area
    //      bits 2-3: Palette for top right quadrant of 4x4 tile area
    //      bits 4-5: Palette for bottom left quadrant of 4x4 tile area
    //      bits 6-7: Palette for bottom right quadrant of 4x4 tile area
    uint8_t shift = ((tileX / 2) % 2) * 2 + ((tileY / 2) % 2) * 4;
    return (attributeData >> shift) & 0x03;
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
        data = m_ppu->m_ppuStatus.reg & 0xE0; // Low 5 bits are unused
        m_ppu->m_ppuStatus.layout.verticalBlank = 0;
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

        if(m_ppu->m_ppuCtrl.layout.vramInc) m_ppu->m_vramAddress.vramAddr += 32;
        else m_ppu->m_vramAddress.vramAddr += 1;
        break;
    }

    return data;
}

void PPUregisters::write(const uint16_t& address, const uint8_t& data) {
    uint8_t localAddr = address % 8;

    switch(localAddr) {
    case 0:
        m_ppu->m_ppuCtrl.reg = data;
        m_ppu->m_tvramAddress.layout.nametableX = m_ppu->m_ppuCtrl.layout.nametableAddrX;
        m_ppu->m_tvramAddress.layout.nametableY = m_ppu->m_ppuCtrl.layout.nametableAddrY;
        break;
    case 1:
        m_ppu->m_ppuMask.reg = data;
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
            m_ppu->m_tvramAddress.layout.coarseX = data >> 3;
            // Todo - fine x scroll
            m_addressLatch = true;
        }
        else {
            // Write vertical scroll
            m_ppu->m_tvramAddress.layout.coarseY = data >> 3;
            m_ppu->m_tvramAddress.layout.fineY = data & 0b111;
            m_addressLatch = false;
        }
        break;
    case 6:
        if(!m_addressLatch) {
            m_ppu->m_tvramAddress.vramAddr = ((uint16_t)(data & 0x3F) << 8) | ((uint16_t)(m_ppu->m_tvramAddress.vramAddr) & 0x00FF);
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

        if(m_ppu->m_ppuCtrl.layout.vramInc) m_ppu->m_vramAddress.vramAddr += 32;
        else m_ppu->m_vramAddress.vramAddr += 1;
        break;
    }
}