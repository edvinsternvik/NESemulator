#pragma once
#include "Cpu6502.h"
#include "PPU.h"
#include "Buss.h"
#include "Cartridge.h"

class APUIOregisters;
class Controller;

class NES {
public:
    NES();
    void reset();

    void loadCartridge(std::shared_ptr<Cartridge> cartridge);
    void connectController1(std::shared_ptr<Controller> controller);
    void connectController2(std::shared_ptr<Controller> controller);

    Controller* getController1();
    Controller* getController2();

public:
    Buss cpuBuss, ppuBuss;
    std::shared_ptr<Cpu6502> cpu;
    std::shared_ptr<PPU> ppu;

private:
    std::shared_ptr<APUIOregisters> m_apuioregisters; 
};