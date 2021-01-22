#include <iostream>
#include "Cpu6502.h"
#include "Buss.h"
#include "Cartridge.h"
#include <iostream>

int main(int argc, const char** argv) {
    if(argc < 2) {
        std::cout << "No cartridge specified" << std::endl;
        return 0;
    }

    Buss buss;
    Cpu6502 cpu;
    cpu.registerBuss(&buss);

    std::shared_ptr<Cartridge> cartridge = std::make_shared<Cartridge>(argv[1]);
    if(!cartridge->successfulLoad()) {
        std::cout << "Could not load cartridge" << std::endl;
        return 0;
    }

    buss.insertCartridge(cartridge);

    cpu.reset();

    while(true) {
        cpu.clock();
    }
}