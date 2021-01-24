#include "Buss.h"
#include "Cartridge.h"

void Buss::addDevice(std::shared_ptr<BussDevice> device) {
    DeviceData newDevice;
    newDevice.device = device;
    newDevice.startAddress = m_size;
    newDevice.size = device->getSize();
    m_devices.push_back(newDevice);

    m_size += newDevice.size;
}

uint8_t Buss::read(uint16_t address) {
    if(address >= m_size) return 0;
    
    for(auto& device : m_devices) {
        if(address < device.size) {
            return device.device->read(address); // address is local to the device
        }
        address -= device.size;
    }

    return 0;
}

void Buss::write(uint16_t address, const uint8_t& data) {
    if(address >= m_size) return;

    for(auto& device : m_devices) {
        if(address < device.size) {
            device.device->write(address, data); // address is local to the device
            break;
        }
        address -= device.size;
    }
}