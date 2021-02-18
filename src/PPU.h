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
    void fetchPatternTableTile(uint8_t nametableByte);
    uint8_t fetchNextPixel();
    void incVramAddrVertical();
    void setPixel(uint8_t pixelValue);
    
public:
    uint32_t windowBuffer[256*240];
    bool frameDone = false;

private:
    union PPUctrl {
        struct Layout {
            uint8_t nametableAddr : 2;
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
            uint8_t ShowSpr : 1;
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
            uint16_t nametable : 2;
            uint16_t fineY : 3;
            uint16_t unused : 1;
        };
        Layout layout;
        uint16_t vramAddr;
    };
    VramAddress m_vramAddress;
    VramAddress m_tvramAddress;

    uint16_t m_scanline;
    uint16_t m_cycle;


    // Shift registers
    uint16_t m_pTableDataLow;
    uint16_t m_pTableDataHigh;
    uint8_t m_paletteAttrData;

    uint32_t m_pixelOffset;
    bool m_nmi;
    bool m_oddFrame;

    Buss* m_buss;

    friend class PPUregisters;
};

class PPUregisters : public BussDevice {
public:
    PPUregisters(std::shared_ptr<PPU> ppu) : m_ppu(ppu) {}

    virtual uint8_t read(const uint16_t& address) override;
    virtual void write(const uint16_t& address, const uint8_t& data) override;

    virtual uint16_t getSize() override { return 0x2000; }

private:
    std::shared_ptr<PPU> m_ppu;
    bool m_addressLatch = false;
    uint8_t m_dataBuffer;
};