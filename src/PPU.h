#pragma once
#include "BussDevice.h"
#include <cstdint>
#include <memory>

class Buss;

class PPU {
public:
    PPU();
    void clock();

    void registerBuss(Buss* buss);
    bool nmi(); // Returns 1 if NMI is generated

private:
    uint8_t read(const uint16_t& address);
    void write(const uint16_t& address, const uint8_t& data);

    uint8_t fetchNametableByte();
    void fetchAttributeData();
    void fetchPatternTableTile(uint8_t nametableByte);
    void updateShiftRegisters();
    void incVramAddrVertical();
    void incVramAddrHorizontal();
    void transferVramAddrVertical();
    void transferVramAddrHorizontal();
    uint8_t fetchNextBgPaletteIndex();
    void setBgPixel(uint8_t paletteIndex, uint32_t pixelOffset, uint8_t tileX, uint8_t tileY, uint8_t attributeData);
    uint8_t fetchNextSpritePaletteIndex(uint8_t& spriteIndex);
    void setSpritePixel(uint8_t paletteIndex, uint32_t pixelOffset, uint8_t spriteIndex);
    void fillSecondaryOam();
    void fetchSpriteData();
    uint8_t getPalette(uint8_t tileX, uint8_t tileY, uint8_t attributeData);
    uint8_t reverseBits(uint8_t byte);
    
public:
    uint32_t windowBuffer[256*240];
    bool frameDone = false;

private:
    union PPUctrl {
        struct Layout {
            uint8_t nametableAddrX : 1;
            uint8_t nametableAddrY : 1;
            uint8_t vramInc : 1;
            uint8_t spritePatternAddr : 1;
            uint8_t bgPatternTableAddr : 1;
            uint8_t spriteSize : 1;
            uint8_t masterSlave : 1;
            uint8_t genNMI : 1;
        };
        Layout layout;
        uint8_t reg;
    };

    union PPUmask {
        struct Layout {
            uint8_t greyscale : 1;
            uint8_t showBgLeft : 1;
            uint8_t showSprLeft : 1;
            uint8_t showBg : 1;
            uint8_t showSpr : 1;
            uint8_t emphasizeRed : 1;
            uint8_t emphasizeGreen : 1;
            uint8_t emphasizeBlue : 1;
        };
        Layout layout;
        uint8_t reg;
    };

    union PPUstatus {
        struct Layout {
            uint8_t unused : 5;
            uint8_t spriteOveflow : 1;
            uint8_t spriteZeroHit : 1;
            uint8_t verticalBlank : 1;
        };
        Layout layout;
        uint8_t reg;
    };

    PPUctrl m_ppuCtrl;
    PPUmask m_ppuMask;
    PPUstatus m_ppuStatus;

    union VramAddress {
        struct Layout {
            uint16_t coarseX : 5;
            uint16_t coarseY : 5;
            uint16_t nametableX : 1;
            uint16_t nametableY : 1;
            uint16_t fineY : 3;
            uint16_t unused : 1;
        };
        Layout layout;
        uint16_t vramAddr;
    };
    VramAddress m_vramAddress;
    VramAddress m_tvramAddress;
    uint8_t m_fineX;

    uint8_t m_oam[64 * 4];
    uint8_t m_sOam[8 * 4];
    uint8_t m_oamAddress;

    uint16_t m_scanline;
    uint16_t m_cycle;

    // Shift registers
    uint8_t m_pTableDataSprLow[8];
    uint8_t m_pTableDataSprHigh[8];
    uint16_t m_pTableDataBgLow;
    uint16_t m_pTableDataBgHigh;

    uint8_t m_pAttrData1;
    uint8_t m_pAttrData2;

    uint8_t m_spriteX[8];
    uint8_t m_spriteAttr[8];

    bool m_nmi;
    bool m_oddFrame;
    uint8_t m_spritesFound;
    uint8_t m_activeSprites[8];
    int m_spriteZeroIndex;

    Buss* m_buss;

    struct Colour {
        Colour(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b) {}
        uint8_t r, g, b;
    };
    const Colour m_palette[16*4];

    friend class PPUregisters;
    friend class OAMDMA;
};

class PPUregisters : public BussDevice {
public:
    PPUregisters(std::shared_ptr<PPU> ppu) : m_ppu(ppu) {}

    virtual uint8_t read(const uint16_t& address) override;
    virtual void write(const uint16_t& address, const uint8_t& data) override;

private:
    std::shared_ptr<PPU> m_ppu;
    bool m_addressLatch = false;
    uint8_t m_dataBuffer;
};

class Cpu6502;

class OAMDMA : public BussDevice {
public:
    OAMDMA(std::shared_ptr<PPU> ppu, std::shared_ptr<Cpu6502> cpu) : m_ppu(ppu), m_cpu(cpu) {}

    virtual uint8_t read(const uint16_t& address) override;
    virtual void write(const uint16_t& address, const uint8_t& data) override;

private:
    std::shared_ptr<PPU> m_ppu;
    std::shared_ptr<Cpu6502> m_cpu;
};