#pragma once
#include "Cpu6502.h"
#include "Buss.h"
#include "Cartridge.h"

class NES {
public:
    NES();
    void reset();

    void loadCartridge(std::shared_ptr<Cartridge> cartridge);

public:
    Buss cpuBuss, ppuBuss;
    Cpu6502 cpu;
};