#include <iostream>
#include "NES.h"

int main(int argc, const char** argv) {
    if(argc < 2) {
        std::cout << "No cartridge specified" << std::endl;
        return 0;
    }

    std::shared_ptr<Cartridge> cartridge = std::make_shared<Cartridge>(argv[1]);
    if(!cartridge->successfulLoad()) {
        std::cout << "Could not load cartridge" << std::endl;
        return 0;
    }

    NES nes;
    nes.loadCartridge(cartridge);
    while(true) {
        nes.cpu.clock();
    }
}