#include <iostream>
#include "Cpu6502.h"
#include "Buss.h"
#include "Cartridge.h"
#include <iostream>
#include "Ram.h"

int main(int argc, const char** argv) {
    if(argc < 2) {
        std::cout << "No cartridge specified" << std::endl;
        return 0;
    }

    Buss cpuBuss;
    Cpu6502 cpu;
    cpu.registerBuss(&cpuBuss);

    std::shared_ptr<Cartridge> cartridge = std::make_shared<Cartridge>(argv[1]);
    if(!cartridge->successfulLoad()) {
        std::cout << "Could not load cartridge" << std::endl;
        return 0;
    }

    cpuBuss.addDevice(std::make_shared<Ram<0x800, 0x2000>>()); // 2kb internal ram
    cpuBuss.addDevice(std::make_shared<Ram<1, 0x2020>>()); // not implemented
    cpuBuss.addDevice(cartridge->prgRom);

    cpu.reset();

    while(true) {
        cpu.clock();
    }
}