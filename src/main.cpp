#include <iostream>
#include "Cpu6502.h"
#include "Buss.h"

int main() {
    Buss buss;
    Cpu6502 cpu;
    cpu.registerBuss(&buss);
    cpu.PC = 0x0020;
    buss.data[0x0020] = 0xA9;
    buss.data[0x0021] = 123;
    buss.data[0x0022] = 0xA9;
    buss.data[0x0023] = 88;
    while(true) {
        cpu.clock();
    }
}