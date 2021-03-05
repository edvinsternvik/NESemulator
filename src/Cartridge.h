#pragma once
#include "Mappers/Mapper.h"
#include "BussDevice.h"
#include <cstdint>
#include <memory>
#include <vector>

class Mapper;

class CartridgeCPU : public BussDevice {
public:
    CartridgeCPU(std::shared_ptr<Mapper> mapper);
    virtual uint8_t read(const uint16_t& address) override;
    virtual void write(const uint16_t& address, const uint8_t& data) override;

private:
    std::shared_ptr<Mapper> m_mapper;
};

class CartridgePPU : public BussDevice {
public:
    CartridgePPU(std::shared_ptr<Mapper> mapper);
    virtual uint8_t read(const uint16_t& address) override;
    virtual void write(const uint16_t& address, const uint8_t& data) override;

private:
    std::shared_ptr<Mapper> m_mapper;
};

class Cartridge {
public:
    Cartridge(const char* filepath);

    bool successfulLoad();

private:
    std::shared_ptr<Mapper> getMapper(const uint8_t& mapperID, const uint32_t& prgRomSize, const uint32_t& chrRomSize);

public:
    std::shared_ptr<CartridgeCPU> cartridgeCPU;
    std::shared_ptr<CartridgePPU> cartridgePPU;
    
private:
    bool m_successfulLoad;
};