#pragma once
#include "BussDevice.h"
#include <cstdint>
#include <memory>
#include <vector>

class Buss {
public:
    void addDevice(std::shared_ptr<BussDevice> device, uint16_t startAddr, uint16_t size);
    void changeDevice(unsigned int index, std::shared_ptr<BussDevice> device, uint16_t startAddr, uint16_t size);

    int getNumberOfDevices() const;

    uint8_t read(uint16_t address);
    void write(uint16_t address, const uint8_t& data);

private:
    struct DeviceData {
        uint32_t startAddress, endAddress;
        std::shared_ptr<BussDevice> device;
    };

    std::vector<DeviceData> m_devices;
    uint32_t m_bussSize = 0;
};