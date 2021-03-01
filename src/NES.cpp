#include "NES.h"
#include "Ram.h"
#include "APUIOregisters.h"
#include "Controller.h"

NES::NES() {
    ppu = std::make_shared<PPU>();
    m_apuioregisters = std::make_shared<APUIOregisters>();

    cpu.registerBuss(&cpuBuss);
    cpuBuss.addDevice(std::make_shared<Ram<0x800, 0x2000>>()); // 2kb internal ram
    cpuBuss.addDevice(std::make_shared<PPUregisters>(ppu)); // 8 bytes of exposed ppu registers, mirrored from $2000-@3FFF
    cpuBuss.addDevice(m_apuioregisters); // APU and IO registers
    cpuBuss.addDevice(std::make_shared<Ram<1, 0x8>>()); // Disabled
    cpuBuss.addDevice(std::make_shared<Ram<1, 0xBFE0>>()); // Temporary placeholder for CartridgeCPU

    ppu->registerBuss(&ppuBuss);
    ppuBuss.addDevice(std::make_shared<Ram<1, 0x2000>>()); // Temporary placeholder for CartridgePPU
    ppuBuss.addDevice(std::make_shared<Ram<0x0800, 0x1F00>>()); // Nametable + Mirrors
    ppuBuss.addDevice(std::make_shared<Ram<0x0020, 0x00E0>>()); // Palette RAM indexes + mirrors
}

void NES::reset() {
    cpu.reset();
}

void NES::loadCartridge(std::shared_ptr<Cartridge> cartridge) {
    cpuBuss.changeDevice(cpuBuss.getNumberOfDevices() - 1, cartridge->cartridgeCPU);
    ppuBuss.changeDevice(0, cartridge->cartridgePPU);
}

void NES::connectController1(std::shared_ptr<Controller> controller) {
    m_apuioregisters->connectController(controller, true);
}

void NES::connectController2(std::shared_ptr<Controller> controller) {
    m_apuioregisters->connectController(controller, false);
}

Controller* NES::getController1() {
    return m_apuioregisters->getController1();
}

Controller* NES::getController2() {
    return m_apuioregisters->getController2();
}