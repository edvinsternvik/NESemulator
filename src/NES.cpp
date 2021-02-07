#include "NES.h"
#include "Ram.h"

NES::NES() {
    ppu = std::make_shared<PPU>();

    cpu.registerBuss(&cpuBuss);
    cpuBuss.addDevice(std::make_shared<Ram<0x800, 0x2000>>()); // 2kb internal ram
    cpuBuss.addDevice(std::make_shared<PPUregisters>(ppu)); // 8 bytes of exposed ppu registers, mirrored from $2000-@3FFF
    cpuBuss.addDevice(std::make_shared<Ram<1, 0x20>>()); // not implemented
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