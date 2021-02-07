#pragma once
#include "BussDevice.h"
#include <cstdint>
#include <memory>
#include <vector>

class Buss {
public:
    void addDevice(std::shared_ptr<BussDevice> device);
    void changeDevice(unsigned int index, std::shared_ptr<BussDevice> device);

    int getNumberOfDevices() const;

    uint8_t read(uint16_t address);
    void write(uint16_t address, const uint8_t& data);

private:
    struct DeviceData {
        uint16_t startAddress, size;
        std::shared_ptr<BussDevice> device;
    };

    std::vector<DeviceData> m_devices;
    uint32_t m_nextStartAddress = 0;
};