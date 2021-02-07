#include "Buss.h"
#include "Cartridge.h"

void Buss::addDevice(std::shared_ptr<BussDevice> device) {
    DeviceData newDevice;
    newDevice.device = device;
    newDevice.startAddress = m_nextStartAddress;
    newDevice.size = device->getSize();
    m_devices.push_back(newDevice);

    m_nextStartAddress += newDevice.size;
}

void Buss::changeDevice(unsigned int index, std::shared_ptr<BussDevice> device) {
    if(index >= m_devices.size()) return;

    m_devices[index].device = device; // Change device
    m_devices[index].size = device->getSize();

    // Update the other devices start address
    m_nextStartAddress = m_devices[index].startAddress + m_devices[index].size;
    for(int i = index + 1; i < m_devices.size(); ++i) {
        m_devices[i].startAddress = m_nextStartAddress;
        m_nextStartAddress += m_devices[i].size;
    }
}

int Buss::getNumberOfDevices() const {
    return m_devices.size();
}

uint8_t Buss::read(uint16_t address) {
    address = address % m_nextStartAddress;
    
    for(auto& device : m_devices) {
        if(address < device.size) {
            return device.device->read(address); // address is local to the device
        }
        address -= device.size;
    }

    return 0;
}

void Buss::write(uint16_t address, const uint8_t& data) {
    address = address % m_nextStartAddress;

    for(auto& device : m_devices) {
        if(address < device.size) {
            device.device->write(address, data); // address is local to the device
            break;
        }
        address -= device.size;
    }
}