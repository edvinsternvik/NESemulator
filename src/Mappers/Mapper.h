#pragma once
#include <inttypes.h>
#include <vector>

class Mapper {
public:
    Mapper(const uint32_t& PRGromSize, const uint32_t& CHRromSize)
        : m_PRGromSize(PRGromSize), m_CHRromSize(CHRromSize) {
        m_prgRom.reserve(m_PRGromSize);
        m_chrRom.reserve(m_CHRromSize);
    }

    virtual uint8_t readCPU(const uint16_t& address) = 0;
    virtual void writeCPU(const uint16_t& address, const uint8_t& data) = 0;

    virtual uint8_t readPPU(const uint16_t& address) = 0;
    virtual void writePPU(const uint16_t& address, const uint8_t& data) = 0;

    uint8_t* getPrgRomDataPtr() { return m_prgRom.data(); }
    uint8_t* getChrRomDataPtr() { return m_chrRom.data(); }

protected:
    uint32_t m_PRGromSize;
    uint32_t m_CHRromSize;
    std::vector<uint8_t> m_prgRam;
    std::vector<uint8_t> m_prgRom;
    std::vector<uint8_t> m_chrRam;
    std::vector<uint8_t> m_chrRom;
};