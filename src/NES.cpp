#include "NES.h"
#include "Ram.h"
#include "APUIOregisters.h"
#include "Controller.h"

NES::NES() {
    cpu = std::make_shared<Cpu6502>();
    ppu = std::make_shared<PPU>();
    m_apuioregisters = std::make_shared<APUIOregisters>();

    cpu->registerBuss(&cpuBuss);
    cpuBuss.addDevice(std::make_shared<Ram<0x800>>(), 0, 0x2000);
    cpuBuss.addDevice(std::make_shared<PPUregisters>(ppu), 0x2000, 0x2000); // Mirrored every 8 bytes
    cpuBuss.addDevice(std::make_shared<OAMDMA>(ppu, cpu), 0x4014, 0x1);
    cpuBuss.addDevice(m_apuioregisters, 0x4000, 0x18); // $4000 - $4018, excluding 0x4014 which will be habdled by OAMDMA
    cpuBuss.addDevice(std::make_shared<Ram<0x1>>(), 0x4018, 0x8); // Disabled
    cpuBuss.addDevice(std::make_shared<Ram<0x1>>(), 0x4020, 0xBFE0); // Temporary placeholder for CartridgeCPU


    ppu->registerBuss(&ppuBuss);
    ppuBuss.addDevice(std::make_shared<Ram<1>>(), 0, 0x2000);
    ppuBuss.addDevice(std::make_shared<Ram<0x0800>>(), 0x2000, 0x1F00); // Nametable + Mirrors
    ppuBuss.addDevice(std::make_shared<Ram<0x0020>>(), 0x3F00, 0x100); // Palette RAM indexes + mirrors
    
}

void NES::reset() {
    cpu->reset();
}

void NES::loadCartridge(std::shared_ptr<Cartridge> cartridge) {
    cpuBuss.changeDevice(cpuBuss.getNumberOfDevices() - 1, cartridge->cartridgeCPU, 0x4020, 0xBFE0);
    ppuBuss.changeDevice(0, cartridge->cartridgePPU, 0, 0x2000);
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