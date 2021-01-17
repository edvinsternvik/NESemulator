#pragma once
#include <inttypes.h>

class Mapper {
public:
    Mapper(const uint32_t& PRGromSize, const uint32_t& CHRromSize)
        : m_PRGromSize(PRGromSize), m_CHRromSize(CHRromSize) {}

    virtual bool mapReadCPU(const uint16_t& address, uint16_t& mappedAddress) = 0;
    virtual bool mapWriteCPU(const uint16_t& address, uint16_t& mappedAddress) = 0;

    virtual bool mapReadPPU(const uint16_t& address, uint16_t& mappedAddress) = 0;
    virtual bool mapWritePPU(const uint16_t& address, uint16_t& mappedAddress) = 0;

protected:
    uint32_t m_PRGromSize;
    uint32_t m_CHRromSize;
};