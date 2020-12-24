#pragma once
#include <cstdint>
#include <vector>
#include <string>

class Buss;

class Cpu6502 {
public:
    Cpu6502();

    void clock();
    void registerBuss(Buss* buss);

public:
    uint8_t A, P, X, Y, SP;
    uint16_t PC;

private:
    uint8_t read(const uint16_t& address);
    void write(const uint8_t& data);

private:
    uint8_t m_ins;
    uint8_t m_state = 0;
    Buss* m_buss;

    uint8_t m_operandAddress; // Stores the operand used for the current operation

    struct Instruction {
        std::string name;
        uint8_t (Cpu6502::*op)();
        uint8_t (Cpu6502::*addressing)();
        uint8_t cycles;
    };

    std::vector<Instruction> m_operations;

    uint8_t ACC(); uint8_t IMM(); uint8_t ABS(); uint8_t ZPA(); uint8_t ZPX(); uint8_t ZPY(); uint8_t ABX(); uint8_t ABY();
    uint8_t IMP(); uint8_t REL(); uint8_t IDX(); uint8_t IDY(); uint8_t ABI();

    uint8_t ADC(); uint8_t AND(); uint8_t ASL(); uint8_t BCC(); uint8_t BCS(); uint8_t BEQ(); uint8_t BIT(); uint8_t BMI();
    uint8_t BNE(); uint8_t BPL(); uint8_t BRK(); uint8_t BVC(); uint8_t BVS(); uint8_t CLC(); uint8_t CLD(); uint8_t CLI();
    uint8_t CLV(); uint8_t CMP(); uint8_t CPX(); uint8_t CPY(); uint8_t DEC(); uint8_t DEX(); uint8_t DEY(); uint8_t EOR();
    uint8_t INC(); uint8_t INX(); uint8_t INY(); uint8_t JMP(); uint8_t JSR(); uint8_t LDA(); uint8_t LDX(); uint8_t LDY();
    uint8_t LSR(); uint8_t NOP(); uint8_t ORA(); uint8_t PHA(); uint8_t PHP(); uint8_t PLA(); uint8_t PLP(); uint8_t ROL();
    uint8_t ROR(); uint8_t RTI(); uint8_t RTS(); uint8_t SBC(); uint8_t SEC(); uint8_t SED(); uint8_t SEI(); uint8_t STA();
    uint8_t STX(); uint8_t STY(); uint8_t TAX(); uint8_t TAY(); uint8_t TSX(); uint8_t TXA(); uint8_t TXS(); uint8_t TYA();

    uint8_t XXX();
};