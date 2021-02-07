#pragma once
#include "Mapper.h"

class NROM : public Mapper {
public:
    NROM(const uint32_t& PRGromSize, const uint32_t& CHRromSize);

    virtual uint8_t readCPU(const uint16_t& address) override;
    virtual void writeCPU(const uint16_t& address, const uint8_t& data) override;

    virtual uint8_t readPPU(const uint16_t& address) override;
    virtual void writePPU(const uint16_t& address, const uint8_t& data) override;
};