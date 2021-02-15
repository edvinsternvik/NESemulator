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
    
public:
    uint32_t windowBuffer[256*240];
    bool frameDone = false;

private:
    union {
        struct {
            uint8_t nametableAddr : 2;
            uint8_t vramInc : 1;
            uint8_t spritePatternAddr : 1;
            uint8_t bgPatternTableAddr : 1;
            uint8_t spriteSize : 1;
            uint8_t masterSlave : 1;
            uint8_t genNMI : 1;
        };
        uint8_t reg;
    } PPUctrl;

    union {
        struct {
            uint8_t greyscale : 1;
            uint8_t showBgLeft : 1;
            uint8_t showSprLeft : 1;
            uint8_t showBg : 1;
            uint8_t ShowSpr : 1;
            uint8_t emphasizeRed : 1;
            uint8_t emphasizeGreen : 1;
            uint8_t emphasizeBlue : 1;
        };
        uint8_t reg;
    } PPUmask;

    union {
        struct {
            uint8_t unused : 5;
            uint8_t spriteOveflow : 1;
            uint8_t spriteZeroHit : 1;
            uint8_t verticalBlank : 1;
        };
        uint8_t reg;
    } PPUstatus;

    uint16_t m_scanline;
    uint16_t m_cycle;

    uint16_t m_vramAddress;
    uint16_t m_tvramAddress;

    // Shift registers
    uint16_t m_patternTableData;
    uint8_t m_paletteAttrData;

    uint16_t m_nametableOffset;
    uint32_t m_pixelOffset;
    bool m_nmi;

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