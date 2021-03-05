#include "Buss.h"
#include "Cartridge.h"

void Buss::addDevice(std::shared_ptr<BussDevice> device, uint16_t startAddr, uint16_t size) {
    // Create device
    DeviceData newDevice;
    newDevice.device = device;
    newDevice.startAddress = startAddr;
    newDevice.endAddress = startAddr + size;
    m_devices.push_back(newDevice);

    m_bussSize = std::max(m_bussSize, newDevice.endAddress);
}

void Buss::changeDevice(unsigned int index, std::shared_ptr<BussDevice> device, uint16_t startAddr, uint16_t size) {
    if(index >= m_devices.size()) return;

    m_devices[index].device = device; // Change device
    m_devices[index].startAddress = startAddr;
    m_devices[index].endAddress = startAddr + size;

    m_bussSize = std::max(m_bussSize, m_devices[index].endAddress);
}

int Buss::getNumberOfDevices() const {
    return m_devices.size();
}

// If two devices overlap, the first one that was added will be used
uint8_t Buss::read(uint16_t address) {
    address = address % m_bussSize;
    
    for(auto& device : m_devices) {
        if(device.startAddress <= address && address < device.endAddress) {
            return device.device->read(address - device.startAddress); // address is local to the device
        }
    }

    return 0;
}

// If two devices overlap, the first one that was added will be used
void Buss::write(uint16_t address, const uint8_t& data) {
    address = address % m_bussSize;

    for(auto& device : m_devices) {
        if(device.startAddress <= address && address < device.endAddress) {
            device.device->write(address - device.startAddress, data); // address is local to the device
            return;
        }
    }
}