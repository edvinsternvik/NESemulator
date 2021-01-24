#include "NES.h"
#include "Ram.h"

NES::NES() {
    cpu.registerBuss(&cpuBuss);

    cpuBuss.addDevice(std::make_shared<Ram<0x800, 0x2000>>()); // 2kb internal ram
    cpuBuss.addDevice(std::make_shared<Ram<1, 0x2020>>()); // not implemented
}

void NES::reset() {
    cpu.reset();
}

void NES::loadCartridge(std::shared_ptr<Cartridge> cartridge) {
    cpuBuss.addDevice(cartridge->prgRom);
    ppuBuss.addDevice(cartridge->chrRom);
}