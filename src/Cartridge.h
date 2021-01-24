#pragma once
#include "Mappers/Mapper.h"
#include "BussDevice.h"
#include <cstdint>
#include <memory>
#include <vector>

class Mapper;

class PRGrom : public BussDevice {
public:
    virtual uint8_t read(const uint16_t& address) override;
    virtual void write(const uint16_t& address, const uint8_t& data) override;
    virtual uint16_t getSize() override {
        return 0xBFE0;  // Size of entire cartridge space
    }

private:
    std::vector<uint8_t> m_rom;
    std::shared_ptr<Mapper> m_mapper;

    friend class Cartridge;
};

class CHRrom : public BussDevice {
public:
    virtual uint8_t read(const uint16_t& address) override;
    virtual void write(const uint16_t& address, const uint8_t& data) override;
    virtual uint16_t getSize() override {
        return 0xBFE0;  // Size of entire cartridge space
    }

private:
    std::vector<uint8_t> m_rom;
    std::shared_ptr<Mapper> m_mapper;

    friend class Cartridge;
};

class Cartridge {
public:
    Cartridge(const char* filepath);

    bool successfulLoad();

private:
    bool setMapper(const uint8_t& mapperID, const uint32_t& prgRomSize, const uint32_t& chrRomSize);

public:
    std::shared_ptr<PRGrom> prgRom;
    std::shared_ptr<CHRrom> chrRom;
    
private:
    bool m_successfulLoad;
};