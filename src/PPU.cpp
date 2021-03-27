#include "PPU.h"
#include "Buss.h"
#include "Cpu6502.h"

#define C Colour
PPU::PPU()
    : m_scanline(261), m_cycle(0), m_nmi(false), m_oddFrame(false),
        m_palette{
            C(84,   84,  84), C(0,    30, 116), C(8,    16, 144), C(48,    0, 136), C(68,    0, 100), C(92,    0,  48), C(84,    4,   0), C(60,   24,   0), C(32,   42,   0), C(8,    58,   0), C(0,    64,   0), C(0,    60,   0), C(0,    50,  60), C(0,     0,   0),  C(0, 0, 0),  C(0, 0, 0),
            C(152, 150, 152), C(8,    76, 196), C(48,   50, 236), C(92,   30, 228), C(136,  20, 176), C(160,  20, 100), C(152,  34,  32), C(120,  60,   0), C(84,   90,   0), C(40,  114,   0), C(8,   124,   0), C(0,   118,  40), C(0,   102, 120), C(0,     0,   0),  C(0, 0, 0),  C(0, 0, 0),
            C(236, 238, 236), C(76,  154, 236), C(120, 124, 236), C(176,  98, 236), C(228,  84, 236), C(236,  88, 180), C(236, 106, 100), C(212, 136,  32), C(160, 170,   0), C(116, 196,   0), C(76,  208,  32), C(56,  204, 108), C(56,  180, 204), C(60,   60,  60),  C(0, 0, 0),  C(0, 0, 0),
            C(236, 238, 236), C(168, 204, 236), C(188, 188, 236), C(212, 178, 236), C(236, 174, 236), C(236, 174, 212), C(236, 180, 176), C(228, 196, 144), C(204, 210, 120), C(180, 222, 120), C(168, 226, 144), C(152, 226, 180), C(160, 214, 228), C(160, 162, 160),  C(0, 0, 0),  C(0, 0, 0)} {               
}

void PPU::clock() {
    if(m_scanline == 261) { // Pre render scanline
        if(m_cycle == 1) {
            m_ppuStatus.layout.verticalBlank = 0;
            m_ppuStatus.layout.spriteZeroHit = 0;
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

            if(m_scanline < 240) { // Visible scanlines only, render pixels
                // Render background pixels
                uint8_t bgPaletteIndex = fetchNextBgPaletteIndex();

                int tilePixel = ((m_cycle - 1) % 8) + m_fineX;
                int tileCrossed = tilePixel >= 8 ? 1 : 0;
                uint8_t tileX = m_vramAddress.layout.coarseX + tileCrossed - 2; //Rendering is two tiles behind
                uint8_t tileY = m_vramAddress.layout.coarseY;
                uint8_t attrData = tileCrossed ? m_pAttrData2 : m_pAttrData1;

                uint32_t pixelOffset = m_scanline * 256 + (m_cycle - 1);
                setBgPixel(bgPaletteIndex, pixelOffset, tileX, tileY, attrData);
                

                // Render sprite pixels
                uint8_t sprIndex = 0;
                uint8_t sprPaletteIndex = fetchNextSpritePaletteIndex(sprIndex);
                bool frontPriority = (m_spriteAttr[sprIndex] & 0x20) == 0;
                if(sprIndex == 0 && sprPaletteIndex != 0) m_ppuStatus.layout.spriteZeroHit = 1;
                if((sprPaletteIndex != 0 && frontPriority) || bgPaletteIndex == 0) {
                    setSpritePixel(sprPaletteIndex, pixelOffset, sprIndex);
                }
            }
        }
        else if(m_cycle < 321) {
            if(m_cycle == 257) {
                updateShiftRegisters(); // Unused tile fetch
                incVramAddrVertical();
                transferVramAddrHorizontal();
            }
        }
        else if(m_cycle == 329 || m_cycle == 337) { // Fetch the first two tiles of the next scanline
            // We are not rendering, but we still need to shift the registers
            m_pTableDataBgHigh <<= 8;
            m_pTableDataBgLow <<= 8;
            
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

    // Sprite evaluation
    if(m_scanline < 240) {
        if(m_cycle < 65) { // Clear secondary OAM
            if(m_cycle == 0) {
                std::fill(m_sOam, m_sOam + 32, 0xFF);

                m_spritesFound = 0;
                std::fill(m_activeSprites, m_activeSprites + 8, 0);
            }
        }
        else if(m_cycle < 257) {
            fillSecondaryOam();
        }
        else if(m_cycle < 321) { // Sprite fetches
            if(m_cycle % 8 == 0) { // 264, 272, 280, 288, 296, 304, 312, 320
                fetchSpriteData();
            }
            m_oamAddress = 0;
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
        uint16_t addr = address;
        if(addr == 0x3F10 || addr == 0x3F14 || addr == 0x3F18 || addr == 0x3F1C) {
            addr -= 0x10;
        }
        return m_buss->read(addr & 0x3FFF);
    }
    return 0;
}

void PPU::write(const uint16_t& address, const uint8_t& data) {
    if(m_buss) {
        uint16_t addr = address;
        if(addr == 0x3F10 || addr == 0x3F14 || addr == 0x3F18 || addr == 0x3F1C) {
            addr -= 0x10;
        }
        m_buss->write(addr & 0x3FFF, data);
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
    
    m_pTableDataBgLow  = (m_pTableDataBgLow  & 0xFF00) | newLow;
    m_pTableDataBgHigh = (m_pTableDataBgHigh & 0xFF00) | newHigh;
}

void PPU::updateShiftRegisters() {
    uint8_t nametableByte = fetchNametableByte();
    fetchAttributeData();
    fetchPatternTableTile(nametableByte);
    incVramAddrHorizontal();
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
    }
}

// Returns the next palette index from pattern table data shift registers, and then shifts these registers
uint8_t PPU::fetchNextBgPaletteIndex() {
    int8_t pixel = 0;
    uint16_t bitmask = 0x8000 >> m_fineX;

    pixel |= (m_pTableDataBgLow & bitmask) >> (15 - m_fineX); // low
    pixel |= (m_pTableDataBgHigh & bitmask) >> (14 - m_fineX); // high

    m_pTableDataBgLow <<= 1;
    m_pTableDataBgHigh <<= 1;

    return pixel;
}

// Sets the pixel in the window buffer at pixelOffset. The provided bgPaletteIndex
// should be a number between 0 and 3 specifying a colour in the palette which we get from attributeData
void PPU::setBgPixel(uint8_t paletteIndex, uint32_t pixelOffset, uint8_t tileX, uint8_t tileY, uint8_t attributeData) {
    if(m_ppuMask.layout.showBg) {
        uint8_t colourIndex;
        if(paletteIndex == 0) {
            colourIndex = read(0x3F00); // Bit 0 of every palette is background, located at 0x3F00
        }
        else {
            uint8_t palette = getPalette(tileX, tileY, attributeData);
            uint16_t paletteOffset = 0x3F00 + palette * 4;
            colourIndex = read(paletteOffset + paletteIndex);
        }
        Colour c = m_palette[colourIndex];
        unsigned int pixelVal = (unsigned int)c.r << 16 | (unsigned int)c.g << 8 | (unsigned int)c.b;
        *((unsigned int*)(windowBuffer + pixelOffset)) = pixelVal;
    }
}

// Returns the next palette index from a sprite if there is one active, and also sets the index of the sprite in the spriteIndex argument variable
uint8_t PPU::fetchNextSpritePaletteIndex(uint8_t& spriteIndex) {

    uint8_t spritePaletteIndex = 0;
    spriteIndex = 0;
    for(uint8_t i = 0; i < 8; ++i) {
        if(m_activeSprites[i] > 0) {
            if(spritePaletteIndex == 0) {
                spritePaletteIndex = ((m_pTableDataSprHigh[i] & 0x80) >> 6) | ((m_pTableDataSprLow[i] & 0x80) >> 7);
                spriteIndex = i;
            }
            m_pTableDataSprLow[i]  <<= 1;
            m_pTableDataSprHigh[i] <<= 1;

            m_activeSprites[i]--;
        }
    }

    for(int i = 0; i < 8; ++i) {
        m_spriteX[i]--;
        if(m_spriteX[i] == 0) {
            m_activeSprites[i] = 8;
        }
    }

    return spritePaletteIndex;
}

void PPU::setSpritePixel(uint8_t paletteIndex, uint32_t pixelOffset, uint8_t spriteIndex) {
    if(m_ppuMask.layout.showSpr) {
        uint8_t colourIndex;
        if(paletteIndex == 0) {
            colourIndex = read(0x3F00); // Bit 0 of every palette is background, located at 0x3F00
        }
        else {
            uint8_t palette = (m_spriteAttr[spriteIndex]) & 0x3;
            uint16_t paletteOffset = 0x3F10 + (uint16_t)palette * 4;
            colourIndex = read(paletteOffset + paletteIndex);
        }
        Colour c = m_palette[colourIndex];
        unsigned int pixelVal = (unsigned int)c.r << 16 | (unsigned int)c.g << 8 | (unsigned int)c.b;
        *((unsigned int*)(windowBuffer + pixelOffset)) = pixelVal;
    }
}

void PPU::fillSecondaryOam() {
    if(m_spritesFound < 8) {
        uint8_t nSpriteY = m_oam[m_oamAddress];
        uint8_t sOamAddr = m_spritesFound * 4;
        m_sOam[sOamAddr] = nSpriteY;

        uint8_t spriteHeight = m_ppuCtrl.layout.spriteSize ? 16 : 8;
        if(m_scanline >= nSpriteY && m_scanline < nSpriteY + spriteHeight) { // Sprite is in range
            for(int i = 1; i < 4; ++i) m_sOam[sOamAddr + i] = m_oam[m_oamAddress + i];
            m_spritesFound++;
        }
        m_oamAddress += 4;

        // oamAddress is unsigned, so since it was just incremented by 4, the only way it can be < 4 is if it has
        // wrapped back around. There are 64 sprites, each taking up 4 bytes, so if the oamAddress has wrapped back around, it means
        // that all sprites have been checked*. (*Assuming oamAddress started at 0)
        if(m_oamAddress < 4) {
            m_spritesFound = 9; // To make sure that we don't continue evaluating more sprites after we have wrapped around.
        }
    }
    else if(m_spritesFound == 8) {
        // Set sprite overflow if another sprite found. (Simulate bug maybe?)
    }
}
void PPU::fetchSpriteData() {
    uint8_t spriteNumber = (m_cycle - 257) / 8;

    uint8_t tileIndexNumber = m_sOam[spriteNumber * 4 + 1];
    uint16_t pTableSpriteAddr = 0;
    if(m_ppuCtrl.layout.spriteSize) { // 8x16 sprites
        uint16_t bankAddr = (tileIndexNumber & 1) ? 0x1000 : 0x0000;
        uint16_t localTileAddr = (uint16_t)(tileIndexNumber >> 1) * 16; // Each tile is 16 bytes
        pTableSpriteAddr = bankAddr + localTileAddr;
    }
    else { // 8x8 sprites
        uint16_t bankAddr = m_ppuCtrl.layout.spritePatternAddr ? 0x1000 : 0x0000;
        uint16_t localTileAddr = (uint16_t)tileIndexNumber * 16; // Each tile is 16 bytes
        pTableSpriteAddr = bankAddr + localTileAddr;
    }

    uint8_t deltaSprY = m_scanline - m_sOam[spriteNumber * 4 + 0];

    if(m_sOam[spriteNumber * 4 + 2] & 0x80) { // Flip sprite vertically
        uint8_t sprSize = m_ppuCtrl.layout.spriteSize ? 16 : 8;
        deltaSprY = sprSize - deltaSprY;
    }

    pTableSpriteAddr += deltaSprY;

    m_pTableDataSprLow[spriteNumber] = read(pTableSpriteAddr);
    m_pTableDataSprHigh[spriteNumber] = read(pTableSpriteAddr + 8);

    if(m_sOam[spriteNumber * 4 + 2] & 0x40) { // Flip sprite horizontally
        m_pTableDataSprLow[spriteNumber] = reverseBits(m_pTableDataSprLow[spriteNumber]);
        m_pTableDataSprHigh[spriteNumber] = reverseBits(m_pTableDataSprHigh[spriteNumber]);
    }

    m_spriteX[spriteNumber] = m_sOam[spriteNumber * 4 + 3];
    m_spriteAttr[spriteNumber] = m_sOam[spriteNumber * 4 + 2];
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

uint8_t PPU::reverseBits(uint8_t byte) {
    return  ((byte & 0x80) >> 7) | ((byte & 0x40) >> 5) | ((byte & 0x20) >> 3) | ((byte & 0x10) >> 1) |
            ((byte & 0x08) << 1) | ((byte & 0x04) << 3) | ((byte & 0x02) << 5) | ((byte & 0x01) << 7);
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
        // Only read during vertical or forced blanking
        if(m_ppu->m_ppuStatus.layout.verticalBlank || (!m_ppu->m_ppuMask.layout.showBg && !m_ppu->m_ppuMask.layout.showSpr)) {
            data = m_ppu->m_oam[m_ppu->m_oamAddress];
        }
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
        m_ppu->m_oamAddress = data;
        break;
    case 4:
        // Only write during vertical or forced blanking
        if(m_ppu->m_ppuStatus.layout.verticalBlank || (!m_ppu->m_ppuMask.layout.showBg && !m_ppu->m_ppuMask.layout.showSpr)) {
            m_ppu->m_oam[m_ppu->m_oamAddress] = data;
            m_ppu->m_oamAddress++;
        }
        break;
    case 5:
        if(!m_addressLatch) {
            // Write horizontal scroll
            m_ppu->m_tvramAddress.layout.coarseX = data >> 3;
            m_ppu->m_fineX = data & 0x07;
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

uint8_t OAMDMA::read(const uint16_t& address) {
    return 0;
}

void OAMDMA::write(const uint16_t& address, const uint8_t& data) {
    uint32_t cycles = (m_cpu->getCycle() % 2 == 0) ? 513 : 512;
    m_cpu->suspend(cycles);

    uint16_t pageAddr = ((uint16_t)data) << 8;
    for(int i = 0; i < 256; ++i) {
        uint8_t oamByte = m_cpu->read(pageAddr + i);
        m_ppu->m_oam[(i + m_ppu->m_oamAddress) % 256] = oamByte;
    }
}