#include "NES.h"
#include "Ram.h"

NES::NES() {
    ppu = std::make_shared<PPU>();
    
    cpu.registerBuss(&cpuBuss);

    cpuBuss.addDevice(std::make_shared<Ram<0x800, 0x2000>>()); // 2kb internal ram
    cpuBuss.addDevice(std::make_shared<PPUregisters>(ppu)); // 8 bytes of exposed ppu registers, mirrored from $2000-@3FFF
    cpuBuss.addDevice(std::make_shared<Ram<1, 0x20>>()); // not implemented
}

void NES::reset() {
    cpu.reset();
}

void NES::loadCartridge(std::shared_ptr<Cartridge> cartridge) {
    cpuBuss.addDevice(cartridge->prgRom);
    ppuBuss.addDevice(cartridge->chrRom);
}