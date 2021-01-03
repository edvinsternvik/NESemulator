#include "Cpu6502.h"
#include "Buss.h"
#include <iostream>

Cpu6502::Cpu6502() {
    using c = Cpu6502;
    m_operations = {
        {"BRK", &c::BRK, &c::IMP, 7}, {"ORA", &c::ORA, &c::IDX, 6}, {"XXX", &c::XXX, &c::IMP, 2}, {"XXX", &c::XXX, &c::IMP, 2}, {"XXX", &c::XXX, &c::IMP, 2}, {"ORA", &c::ORA, &c::ZPA, 3}, {"ASL", &c::ASL, &c::ZPA, 5}, {"XXX", &c::XXX, &c::IMP, 2}, {"PHP", &c::PHP, &c::IMP, 3}, {"ORA", &c::ORA, &c::IMM, 2}, {"ASL", &c::ASL, &c::ACC, 2}, {"XXX", &c::XXX, &c::IMP, 2}, {"XXX", &c::XXX, &c::IMP, 2}, {"ORA", &c::ORA, &c::ABS, 4}, {"ASL", &c::ASL, &c::ABS, 6}, {"XXX", &c::XXX, &c::IMP, 2},
        {"BPL", &c::BPL, &c::REL, 2}, {"ORA", &c::ORA, &c::IDY, 5}, {"XXX", &c::XXX, &c::IMP, 2}, {"XXX", &c::XXX, &c::IMP, 2}, {"XXX", &c::XXX, &c::IMP, 2}, {"ORA", &c::ORA, &c::ZPX, 4}, {"ASL", &c::ASL, &c::ZPX, 6}, {"XXX", &c::XXX, &c::IMP, 2}, {"CLC", &c::CLC, &c::IMP, 2}, {"ORA", &c::ORA, &c::ABY, 4}, {"INC", &c::INC, &c::ACC, 2}, {"XXX", &c::XXX, &c::IMP, 2}, {"XXX", &c::XXX, &c::IMP, 2}, {"ORA", &c::ORA, &c::ABX, 4}, {"ASL", &c::ASL, &c::ABX, 7}, {"XXX", &c::XXX, &c::IMP, 2},
        {"JSR", &c::JSR, &c::ABS, 6}, {"AND", &c::AND, &c::IDX, 6}, {"XXX", &c::XXX, &c::IMP, 2}, {"XXX", &c::XXX, &c::IMP, 2}, {"BIT", &c::BIT, &c::ZPA, 3}, {"AND", &c::AND, &c::ZPA, 3}, {"ROL", &c::ROL, &c::ZPA, 5}, {"XXX", &c::XXX, &c::IMP, 2}, {"PLP", &c::PLP, &c::IMP, 4}, {"AND", &c::AND, &c::IMM, 2}, {"ROL", &c::ROL, &c::ACC, 2}, {"XXX", &c::XXX, &c::IMP, 2}, {"BIT", &c::BIT, &c::ABS, 4}, {"AND", &c::AND, &c::ABS, 4}, {"ROL", &c::ROL, &c::ABS, 6}, {"XXX", &c::XXX, &c::IMP, 2},
        {"BMI", &c::BMI, &c::REL, 2}, {"AND", &c::AND, &c::IDY, 5}, {"XXX", &c::XXX, &c::IMP, 2}, {"XXX", &c::XXX, &c::IMP, 2}, {"BIT", &c::BIT, &c::ZPX, 4}, {"AND", &c::AND, &c::ZPX, 4}, {"ROL", &c::ROL, &c::ZPX, 6}, {"XXX", &c::XXX, &c::IMP, 2}, {"SEC", &c::SEC, &c::IMP, 2}, {"AND", &c::AND, &c::ABY, 4}, {"DEC", &c::DEC, &c::ACC, 2}, {"XXX", &c::XXX, &c::IMP, 2}, {"BIT", &c::BIT, &c::ABX, 4}, {"AND", &c::AND, &c::ABX, 4}, {"ROL", &c::ROL, &c::ABX, 7}, {"XXX", &c::XXX, &c::IMP, 2},
        {"RTI", &c::RTI, &c::IMP, 6}, {"EOR", &c::EOR, &c::IDX, 6}, {"XXX", &c::XXX, &c::IMP, 2}, {"XXX", &c::XXX, &c::IMP, 2}, {"XXX", &c::XXX, &c::IMP, 2}, {"EOR", &c::EOR, &c::ZPA, 3}, {"LSR", &c::LSR, &c::ZPA, 5}, {"XXX", &c::XXX, &c::IMP, 2}, {"PHA", &c::PHA, &c::IMP, 3}, {"EOR", &c::EOR, &c::IMM, 2}, {"LSR", &c::LSR, &c::ACC, 2}, {"XXX", &c::XXX, &c::IMP, 2}, {"JMP", &c::JMP, &c::ABS, 3}, {"EOR", &c::EOR, &c::ABS, 4}, {"LSR", &c::LSR, &c::ABS, 6}, {"XXX", &c::XXX, &c::IMP, 2},
        {"BVC", &c::BVC, &c::REL, 2}, {"EOR", &c::EOR, &c::IDY, 5}, {"XXX", &c::XXX, &c::IMP, 2}, {"XXX", &c::XXX, &c::IMP, 2}, {"XXX", &c::XXX, &c::IMP, 2}, {"EOR", &c::EOR, &c::ZPX, 4}, {"LSR", &c::LSR, &c::ZPX, 6}, {"XXX", &c::XXX, &c::IMP, 2}, {"CLI", &c::CLI, &c::IMP, 2}, {"EOR", &c::EOR, &c::ABY, 4}, {"XXX", &c::XXX, &c::IMP, 2}, {"XXX", &c::XXX, &c::IMP, 2}, {"XXX", &c::XXX, &c::IMP, 2}, {"EOR", &c::EOR, &c::ABX, 4}, {"LSR", &c::LSR, &c::ABX, 7}, {"XXX", &c::XXX, &c::IMP, 2},
        {"RTS", &c::RTS, &c::IMP, 6}, {"ADC", &c::ADC, &c::IDX, 6}, {"XXX", &c::XXX, &c::IMP, 2}, {"XXX", &c::XXX, &c::IMP, 2}, {"XXX", &c::XXX, &c::IMP, 2}, {"ADC", &c::ADC, &c::ZPA, 3}, {"ROR", &c::ROR, &c::ZPA, 5}, {"XXX", &c::XXX, &c::IMP, 2}, {"PLA", &c::PLA, &c::IMP, 4}, {"ADC", &c::ADC, &c::IMM, 2}, {"ROR", &c::ROR, &c::ACC, 2}, {"XXX", &c::XXX, &c::IMP, 2}, {"JMP", &c::JMP, &c::ABI, 6}, {"ADC", &c::ADC, &c::ABS, 4}, {"ROR", &c::ROR, &c::ABS, 6}, {"XXX", &c::XXX, &c::IMP, 2}, 
        {"BVS", &c::BVS, &c::REL, 2}, {"ADC", &c::ADC, &c::IDY, 5}, {"XXX", &c::XXX, &c::IMP, 2}, {"XXX", &c::XXX, &c::IMP, 2}, {"XXX", &c::XXX, &c::IMP, 2}, {"ADC", &c::ADC, &c::ZPX, 4}, {"ROR", &c::ROR, &c::ZPX, 6}, {"XXX", &c::XXX, &c::IMP, 2}, {"SEI", &c::SEI, &c::IMP, 2}, {"ADC", &c::ADC, &c::ABY, 4}, {"XXX", &c::XXX, &c::IMP, 2}, {"XXX", &c::XXX, &c::IMP, 2}, {"XXX", &c::XXX, &c::IMP, 2}, {"ADC", &c::ADC, &c::ABX, 4}, {"ROR", &c::ROR, &c::ABX, 7}, {"XXX", &c::XXX, &c::IMP, 2},
        {"XXX", &c::XXX, &c::IMP, 2}, {"STA", &c::STA, &c::IDX, 6}, {"XXX", &c::XXX, &c::IMP, 2}, {"XXX", &c::XXX, &c::IMP, 2}, {"STY", &c::STY, &c::ZPA, 3}, {"STA", &c::STA, &c::ZPA, 3}, {"STX", &c::STX, &c::ZPA, 3}, {"XXX", &c::XXX, &c::IMP, 2}, {"DEY", &c::DEY, &c::IMP, 2}, {"BIT", &c::BIT, &c::IMM, 2}, {"TXA", &c::TXA, &c::IMP, 2}, {"XXX", &c::XXX, &c::IMP, 2}, {"STY", &c::STY, &c::ABS, 4}, {"STA", &c::STA, &c::ABS, 4}, {"STX", &c::STX, &c::ABS, 4}, {"XXX", &c::XXX, &c::IMP, 2},
        {"BCC", &c::BCC, &c::REL, 2}, {"STA", &c::STA, &c::IDY, 6}, {"XXX", &c::XXX, &c::IMP, 2}, {"XXX", &c::XXX, &c::IMP, 2}, {"STY", &c::STY, &c::ZPX, 4}, {"STA", &c::STA, &c::ZPX, 4}, {"STX", &c::STX, &c::ZPY, 4}, {"XXX", &c::XXX, &c::IMP, 2}, {"TYA", &c::TYA, &c::IMP, 2}, {"STA", &c::STA, &c::ABY, 5}, {"TXS", &c::TXS, &c::IMP, 2}, {"XXX", &c::XXX, &c::IMP, 2}, {"XXX", &c::XXX, &c::IMP, 2}, {"STA", &c::STA, &c::ABX, 5}, {"XXX", &c::XXX, &c::IMP, 2}, {"XXX", &c::XXX, &c::IMP, 2},
        {"LDY", &c::LDY, &c::IMM, 2}, {"LDA", &c::LDA, &c::IDX, 6}, {"LDX", &c::LDX, &c::IMM, 2}, {"XXX", &c::XXX, &c::IMP, 2}, {"LDY", &c::LDY, &c::ZPA, 3}, {"LDA", &c::LDA, &c::ZPA, 3}, {"LDX", &c::LDX, &c::ZPA, 3}, {"XXX", &c::XXX, &c::IMP, 2}, {"TAY", &c::TAY, &c::IMP, 2}, {"LDA", &c::LDA, &c::IMM, 2}, {"TAX", &c::TAX, &c::IMP, 2}, {"XXX", &c::XXX, &c::IMP, 2}, {"LDY", &c::LDY, &c::ABS, 4}, {"LDA", &c::LDA, &c::ABS, 4}, {"LDX", &c::LDX, &c::ABS, 4}, {"XXX", &c::XXX, &c::IMP, 2},
        {"BCS", &c::BCS, &c::REL, 2}, {"LDA", &c::LDA, &c::IDY, 5}, {"XXX", &c::XXX, &c::IMP, 2}, {"XXX", &c::XXX, &c::IMP, 2}, {"LDY", &c::LDY, &c::ZPX, 4}, {"LDA", &c::LDA, &c::ZPX, 4}, {"LDX", &c::LDX, &c::ZPY, 4}, {"XXX", &c::XXX, &c::IMP, 2}, {"CLV", &c::CLV, &c::IMP, 2}, {"LDA", &c::LDA, &c::ABY, 4}, {"TSX", &c::TSX, &c::IMP, 2}, {"XXX", &c::XXX, &c::IMP, 2}, {"LDY", &c::LDY, &c::ABX, 4}, {"LDA", &c::LDA, &c::ABX, 4}, {"LDX", &c::LDX, &c::ABY, 4}, {"XXX", &c::XXX, &c::IMP, 2},
        {"CPY", &c::CPY, &c::IMM, 2}, {"CMP", &c::CMP, &c::IDX, 6}, {"XXX", &c::XXX, &c::IMP, 2}, {"XXX", &c::XXX, &c::IMP, 2}, {"CPY", &c::CPY, &c::ZPA, 3}, {"CMP", &c::CMP, &c::ZPA, 3}, {"DEC", &c::DEC, &c::ZPA, 5}, {"XXX", &c::XXX, &c::IMP, 2}, {"INY", &c::INY, &c::IMP, 2}, {"CMP", &c::CMP, &c::IMM, 2}, {"DEX", &c::DEX, &c::IMP, 2}, {"XXX", &c::XXX, &c::IMP, 2}, {"CPY", &c::CPY, &c::ABS, 4}, {"CMP", &c::CMP, &c::ABS, 4}, {"DEC", &c::DEC, &c::ABS, 6}, {"XXX", &c::XXX, &c::IMP, 2},
        {"BNE", &c::BNE, &c::REL, 2}, {"CMP", &c::CMP, &c::IDY, 5}, {"XXX", &c::XXX, &c::IMP, 2}, {"XXX", &c::XXX, &c::IMP, 2}, {"XXX", &c::XXX, &c::IMP, 2}, {"CMP", &c::CMP, &c::ZPX, 4}, {"DEC", &c::DEC, &c::ZPX, 6}, {"XXX", &c::XXX, &c::IMP, 2}, {"CLD", &c::CLD, &c::IMP, 2}, {"CMP", &c::CMP, &c::ABY, 4}, {"XXX", &c::XXX, &c::IMP, 2}, {"XXX", &c::XXX, &c::IMP, 2}, {"XXX", &c::XXX, &c::IMP, 2}, {"CMP", &c::CMP, &c::ABX, 4}, {"DEC", &c::DEC, &c::ABX, 7}, {"XXX", &c::XXX, &c::IMP, 2},
        {"CPX", &c::CPX, &c::IMM, 2}, {"SBC", &c::SBC, &c::IDX, 6}, {"XXX", &c::XXX, &c::IMP, 2}, {"XXX", &c::XXX, &c::IMP, 2}, {"CPX", &c::CPX, &c::ZPA, 3}, {"SBC", &c::SBC, &c::ZPA, 3}, {"INC", &c::INC, &c::ZPA, 5}, {"XXX", &c::XXX, &c::IMP, 2}, {"INX", &c::INX, &c::IMP, 2}, {"SBC", &c::SBC, &c::IMM, 2}, {"NOP", &c::NOP, &c::IMP, 2}, {"XXX", &c::XXX, &c::IMP, 2}, {"CPX", &c::CPX, &c::ABS, 4}, {"SBC", &c::SBC, &c::ABS, 4}, {"INC", &c::INC, &c::ABS, 6}, {"XXX", &c::XXX, &c::IMP, 2},
        {"BEQ", &c::BEQ, &c::REL, 2}, {"SBC", &c::SBC, &c::IDY, 5}, {"XXX", &c::XXX, &c::IMP, 2}, {"XXX", &c::XXX, &c::IMP, 2}, {"XXX", &c::XXX, &c::IMP, 2}, {"SBC", &c::SBC, &c::ZPX, 4}, {"INC", &c::INC, &c::ZPX, 6}, {"XXX", &c::XXX, &c::IMP, 2}, {"SED", &c::SED, &c::IMP, 2}, {"SBC", &c::SBC, &c::ABY, 4}, {"XXX", &c::XXX, &c::IMP, 2}, {"XXX", &c::XXX, &c::IMP, 2}, {"XXX", &c::XXX, &c::IMP, 2}, {"SBC", &c::SBC, &c::ABX, 4}, {"INC", &c::INC, &c::ABX, 7}, {"XXX", &c::XXX, &c::IMP, 2}   
    };
}

void Cpu6502::clock() {
    if(m_state == 0) { // Fetch
        m_ins = read(PC);
        ++PC;
        m_extraCycles = (this->*m_operations[m_ins].addressing)();
    }
    else if(m_state == 1){ // Execute
        m_extraCycles += (this->*m_operations[m_ins].op)();
    }
    ++m_state;

    if(m_state == m_operations[m_ins].cycles + m_extraCycles) { // Check if done with instruction
        m_state = 0;
    }
}

void Cpu6502::registerBuss(Buss* buss) {
    m_buss = buss;
}

uint8_t Cpu6502::read(const uint16_t& address) {
    return m_buss->data[address];
}

void Cpu6502::write(const uint16_t& address, const uint8_t& data) {
    m_buss->data[address] = data;
}

void Cpu6502::push(const uint8_t& data) {
    write(0x0100 | (uint16_t)SP, data);
    SP--;
}

uint8_t Cpu6502::pull() {
    SP++;
    return read(0x0100 | (uint16_t)SP);
}

uint8_t Cpu6502::getOperand() {
    return read(m_operandAddress);
}

uint8_t Cpu6502::getFlag(const Flags& flag) {
    return P & (1 << flag);
}

void Cpu6502::setFlag(const Flags& flag, const bool& value) {
    if(value) P = P | (1 << flag);
    else      P = P & ((1 << flag) ^ 0xFF);
}

// ---------------------------------------------------------------------------------------
// Addressing Modes:
// ---------------------------------------------------------------------------------------

// Accumulator Addressing: The instruction is just an operation on the accumulator, and so no operand is used.
uint8_t Cpu6502::ACC() {
    return 0;
}

// Immediate Addressing: The second byte of the instruction specifies the operand directly.
uint8_t Cpu6502::IMM() {
    m_operandAddress = PC++;
    return 0;
}

// Absolute Addressing: The second and third bytes specify the 8 lower and higher bits
//      of the address.
uint8_t Cpu6502::ABS() {
    m_operandAddress = read(PC++); // Set low byte
    m_operandAddress |= read(PC++) << 8; // Set high byte
    return 0; 
}

// Zero Page Addressing: The second byte specifies an address in page zero.
uint8_t Cpu6502::ZPA() {
    m_operandAddress = read(PC++); // Set low byte, high byte is 0
    return 0;
}

// Zero Page Indexed Addressing (X): Like zero page addressing, but the contents of the X registered is added
//      to the address.
uint8_t Cpu6502::ZPX() {
    m_operandAddress = read(PC++) + X; // Set low byte, high byte is 0
    m_operandAddress &= 0x00FF;
    return 0;
}

// Zero Page Index Addressing (Y): Like zero page addressing, but the contents of the Y registered is added
//      to the address.
uint8_t Cpu6502::ZPY() {
    m_operandAddress = read(PC++) + Y; // Set low byte, high byte is 0
    m_operandAddress &= 0x00FF;
    return 0;
}

// Absolute Indexed Addressing (X): Like Absolute Addressing but the address is offset by the
//      contents of the X register.
uint8_t Cpu6502::ABX() {
    uint8_t low = read(PC++); // Set low byte
    uint8_t high = read(PC++); // Set high byte
    m_operandAddress = (high << 8) | low;
    m_operandAddress += X;

    if((uint16_t)(low + X) & 0xFF00) { // Offset passes page boundrary
        return 1; // Extra cycle is needed
    }
    return 0;
}

// Absolute Indexed Addressing (Y): Like Absolute Addressing but the address is offset by the
//      contents of the Y register.
uint8_t Cpu6502::ABY() {
    uint8_t low = read(PC++); // Set low byte
    uint8_t high = read(PC++); // Set high byte
    m_operandAddress = (high << 8) | low;
    m_operandAddress += Y;

    if((uint16_t)(low + Y) & 0xFF00) { // Offset passes page boundrary
        return 1; // Extra cycle is needed
    }
    return 0;
}

// Implied Addressing: No operand
uint8_t Cpu6502::IMP() {
    return 0;
}

// Relative Addressing: The second byte of the instruction is an offset from the program counter.
uint8_t Cpu6502::REL() {
    m_operandAddress = read(PC++);
    if(((PC & 0x00FF) + m_operandAddress) & 0xFF00) {
        return 1; // Add one cycle if page boundry is crossed
    }
    return 0;
}

// Indexed Indirect Addressing / Indirect X: The second byte of the instruction, when added to the contents
//      of the X register in zero page, points to the low byte of the address to the operand. The following
//      byte in zero page memory is the high byte of the address to the operand.
uint8_t Cpu6502::IDX() {
    uint16_t addressPtr = read(PC++);
    addressPtr = (addressPtr + X) & 0x00FF; // Address of address to operand

    m_operandAddress = read(addressPtr); // Set low byte
    m_operandAddress |= read((addressPtr + 1) & 0x00FF) << 8; // Set high byte
    return 0;
}

// Indirect Indexed Addressing / Indirect Y: The second byte of the instruction points, after adding the contents
//      of the Y register, to the address of the operand. If adding the Y register causes the page (high byte) to change,
//      an extra cycle is needed.
uint8_t Cpu6502::IDY() {
    uint16_t addressPtr = read(PC++);

    uint8_t low = read(addressPtr); // Set low byte
    uint8_t high = read(addressPtr + 1); // Set high byte

    m_operandAddress = (high << 8) | low;
    m_operandAddress += Y;

    if((uint16_t)(low + Y) & 0xFF00) { // Offset passes page boundrary
        return 1; // Extra cycle is needed
    }
    return 0;
}

// Absolute Indirect: The second and third bytes of the instruction specify the low and high bytes of
//      an address. That address points to the low byte of the address to the operand, the next byte points
//      to the high byte of the operand. This instruction has a bug in hardware: If the low byte of the indirect address
//      is 0xFF then the high byte of the resulting address wraps around to the same page as the low byte
uint8_t Cpu6502::ABI() {
    uint8_t low = read(PC++); // Set low byte
    uint8_t high = read(PC++); // Set high byte

    uint16_t addressPtr = (high << 8) | low;

    if(low == 0xFF) { // Simulate bug
        m_operandAddress = read(addressPtr & 0xFF00) << 8 | read(addressPtr); 
    }
    else {
        m_operandAddress = read(addressPtr + 1) << 8 | read(addressPtr);
    }
    return 0;
}

// ---------------------------------------------------------------------------------------
// Instructions:
// ---------------------------------------------------------------------------------------

// ADd with Carry
uint8_t Cpu6502::ADC() {
    uint8_t operand = getOperand();
    uint16_t res = A + operand + getFlag(Flags::C);
    setFlag(Flags::C, res & 0x0100);
    setFlag(Flags::Z, res & 0x00FF == 0);
    setFlag(Flags::V, (~(A ^ operand) & (A ^ res)) & 0x0080);
    setFlag(Flags::N, res & 0x0080);

    A = res & 0x00FF;
    return 0;
}

// Bitwise AND
uint8_t Cpu6502::AND() {
    A = A & getOperand();
    setFlag(Z, A == 0);
    setFlag(N, A & 0x80);
    return 0;
}

// Arithmetic Shift Left
uint8_t Cpu6502::ASL() {
    uint16_t res = 0;
    if(m_operations[m_ins].addressing == &Cpu6502::ACC) {
        res = A << 1;
        A = res & 0x00FF;
    }
    else {
        res = getOperand() << 1;
        write(m_operandAddress, res & 0x00FF);
    }

    setFlag(Flags::C, res & 0x0100);
    setFlag(Flags::Z, (res & 0x00FF) == 0);
    setFlag(Flags::N, res & 0x0080);
    return 0;
}

// Branch on Carry Clear
uint8_t Cpu6502::BCC() {
    if(!getFlag(Flags::C)) {
        PC += (int8_t)m_operandAddress;
        return 1; // Add extra cycle if branched
    }
    return 0;
}

// Branch on Carry Set
uint8_t Cpu6502::BCS() {
    if(getFlag(Flags::C)) {
        PC += (int8_t)m_operandAddress;
        return 1; // Add extra cycle if branched
    }
    return 0;
}

// Branch on EQual
uint8_t Cpu6502::BEQ() {
    if(getFlag(Flags::Z)) {
        PC += (int8_t)m_operandAddress;
        return 1; // Add extra cycle if branched
    }
    return 0;
}

// test BITs
uint8_t Cpu6502::BIT() {
    uint8_t res = A & getOperand();
    setFlag(Flags::Z, res == 0);
    setFlag(Flags::V, res & 0x40);
    setFlag(Flags::N, res & 0x80);
    return 0;
}

// Branch on MInus
uint8_t Cpu6502::BMI() {
    if(getFlag(Flags::N)) {
        PC += (int8_t)m_operandAddress;
        return 1; // Add extra cycle if branched
    }
    return 0;
}

// Branch on Not Equal
uint8_t Cpu6502::BNE() {
    if(!getFlag(Flags::Z)) {
        PC += (int8_t)m_operandAddress;
        return 1; // Add extra cycle if branched
    }
    return 0;
}

// Branch on PLus
uint8_t Cpu6502::BPL() {
    if(!getFlag(Flags::N)) {
        PC += (int8_t)m_operandAddress;
        return 1; // Add extra cycle if branched
    }
    return 0;
}

uint8_t Cpu6502::BRK() {
    return 0;
}

// Branch on oVerflow Clear
uint8_t Cpu6502::BVC() {
    if(!getFlag(Flags::V)) {
        PC += (int8_t)m_operandAddress;
        return 1; // Add extra cycle if branched
    }
    return 0;
}

// Branch on oVerflow Set
uint8_t Cpu6502::BVS() {
    if(getFlag(Flags::V)) {
        PC += (int8_t)m_operandAddress;
        return 1; // Add extra cycle if branched
    }
    return 0;
}

// CLear Carry
uint8_t Cpu6502::CLC() {
    setFlag(Flags::C, 0);
    return 0;
}

// CLear Decimal
uint8_t Cpu6502::CLD() {
    setFlag(Flags::D, 0);
    return 0;
}

// CLear Interrupt
uint8_t Cpu6502::CLI() {
    setFlag(Flags::I, 0);
    return 0;
}

// Clear oVerflow
uint8_t Cpu6502::CLV() {
    setFlag(Flags::V, 0);
    return 0;
}

// CoMPare accumulator
uint8_t Cpu6502::CMP() {
    uint8_t operand = getOperand();
    uint8_t res = A - operand;
    setFlag(Flags::C, A >= operand);
    setFlag(Flags::Z, A == operand);
    setFlag(Flags::N, res & 0x0080);
    return 0;
}

// ComPare X
uint8_t Cpu6502::CPX() {
    uint8_t operand = getOperand();
    uint8_t res = X - operand;
    setFlag(Flags::C, X >= operand);
    setFlag(Flags::Z, X == operand);
    setFlag(Flags::N, res & 0x0080);
    return 0;
}

// ComPare Y
uint8_t Cpu6502::CPY() {
    uint8_t operand = getOperand();
    uint8_t res = Y - operand;
    setFlag(Flags::C, Y >= operand);
    setFlag(Flags::Z, Y == operand);
    setFlag(Flags::N, res & 0x0080);
    return 0;
}

// DECrement memory
uint8_t Cpu6502::DEC() {
    uint8_t res = getOperand() - 1;
    write(m_operandAddress, res);
    setFlag(Flags::Z, res == 0);
    setFlag(Flags::N, res & 0x80);
    return 0;
}

// DEcrement X
uint8_t Cpu6502::DEX() {
    --X;
    setFlag(Flags::Z, X == 0);
    setFlag(Flags::N, X & 0x80);
    return 0;
}

// DEcrement Y
uint8_t Cpu6502::DEY() {
    --Y;
    setFlag(Flags::Z, Y == 0);
    setFlag(Flags::N, Y & 0x80);
    return 0;
}

// bitwise Exclusive OR
uint8_t Cpu6502::EOR() {
    A = A ^ getOperand();
    setFlag(Flags::Z, A == 0);
    setFlag(Flags::N, A & 0x80);
    return 0;
}

// INCrement memory
uint8_t Cpu6502::INC() {
    uint8_t res = getOperand() + 1;
    write(m_operandAddress, res);
    setFlag(Flags::Z, res == 0);
    setFlag(Flags::N, res & 0x80);
    return 0;
}

// INcrement X
uint8_t Cpu6502::INX() {
    ++X;
    setFlag(Flags::Z, X == 0);
    setFlag(Flags::N, X & 0x80);
    return 0;
}

// INcrement Y
uint8_t Cpu6502::INY() {
    ++Y;
    setFlag(Flags::Z, Y == 0);
    setFlag(Flags::N, Y & 0x80);
    return 0;
}

// JuMP
uint8_t Cpu6502::JMP() {
    PC = m_operandAddress;
    return 0;
}

// Jump to SubRoutine
uint8_t Cpu6502::JSR() {
    uint16_t retAddr = PC - 1;
    uint8_t low = retAddr & 0x00FF;
    uint8_t high = retAddr >> 8;
    push(high);
    push(low);
    PC = m_operandAddress;
    return 0;
}

// LoaD Accumulator
uint8_t Cpu6502::LDA() {
    A = getOperand();
    setFlag(Flags::Z, A == 0);
    setFlag(Flags::N, A & 0x80);
    return 0;
}

// LoaD X
uint8_t Cpu6502::LDX() {
    X = getOperand();
    setFlag(Flags::Z, X == 0);
    setFlag(Flags::N, X & 0x80);
    return 0;
}

// LoaD Y
uint8_t Cpu6502::LDY() {
    Y = getOperand();
    setFlag(Flags::Z, Y == 0);
    setFlag(Flags::N, Y & 0x80);
    return 0;
}

// Logical Shift Right
uint8_t Cpu6502::LSR() {
    uint16_t res = 0;
    if(m_operations[m_ins].addressing == &Cpu6502::ACC) {
        setFlag(Flags::C, A & 0x01);
        res = A >> 1;
        A = res & 0x00FF;
    }
    else {
        uint8_t operand = getOperand();
        setFlag(Flags::C, operand & 0x01);
        res = operand >> 1;
        write(m_operandAddress, res & 0x00FF);
    }

    setFlag(Flags::Z, (res & 0x00FF) == 0);
    setFlag(Flags::N, res & 0x0080);
    return 0;
}

uint8_t Cpu6502::NOP() {
    return 0;
}

// OR Accumulator
uint8_t Cpu6502::ORA() {
    A = A | getOperand();
    setFlag(Flags::Z, A == 0);
    setFlag(Flags::N, A & 0x80);
    return 0;
}

// PusH Accumulator
uint8_t Cpu6502::PHA() {
    push(A);
    return 0;
}

// PusH Processor status
uint8_t Cpu6502::PHP() {
    push(P);
    return 0;
}

// PuLl Accumulator
uint8_t Cpu6502::PLA() {
    A = pull();
    return 0;
}

// PuLl Processor status
uint8_t Cpu6502::PLP() {
    P = pull();
    return 0;
}

// ROtate Left
uint8_t Cpu6502::ROL() {
    uint16_t res;
    if(m_operations[m_ins].addressing == &Cpu6502::ACC) {
        res = ((uint16_t)A << 1);
        if(getFlag(Flags::C)) res |= 0x0001;
        A = res & 0x00FF;
    }
    else {
        res = ((uint16_t)getOperand() << 1);
        if(getFlag(Flags::C)) res |= 0x0001;
        write(m_operandAddress, res & 0x00FF);
    }
    setFlag(Flags::C, res & 0x0100);
    setFlag(Flags::Z, res & 0x00FF == 0);
    setFlag(Flags::N, res & 0x0080);
    return 0;
}

// ROtate Right
uint8_t Cpu6502::ROR() {
    uint8_t res;
    if(m_operations[m_ins].addressing == &Cpu6502::ACC) {
        res = ((getFlag(Flags::C) << 8) | A) >> 1;
        setFlag(Flags::C, A & 0x01);
        A = res;
    }
    else {
        int operand = getOperand();
        res = ((getFlag(Flags::C) << 8) | operand) >> 1;
        setFlag(Flags::C, operand & 0x01);
        write(m_operandAddress, res);
    }
    setFlag(Flags::Z, res == 0);
    setFlag(Flags::N, res & 0x80);
    return 0;
}

uint8_t Cpu6502::RTI() {
    return 0;
}

// ReTurn Subroutine
uint8_t Cpu6502::RTS() {
    uint8_t low = pull();
    uint8_t high = pull();
    uint16_t retAddr = (high << 8) | low;
    PC = retAddr + 1;
    return 0;
}

uint8_t Cpu6502::SBC() {
    return 0;
}

// Set Carry
uint8_t Cpu6502::SEC() {
    setFlag(Flags::C, 1);
    return 0;
}

// Set Decimal
uint8_t Cpu6502::SED() {
    setFlag(Flags::D, 1);
    return 0;
}

// Set Interrupt
uint8_t Cpu6502::SEI() {
    setFlag(Flags::I, 1);
    return 0;
}

// STore Accumulator
uint8_t Cpu6502::STA() {
    write(m_operandAddress, A);
    return 0;
}

// STore X
uint8_t Cpu6502::STX() {
    write(m_operandAddress, X);
    return 0;
}

// STore Y
uint8_t Cpu6502::STY() {
    write(m_operandAddress, Y);
    return 0;
}

uint8_t Cpu6502::TAX() {
    return 0;
}

uint8_t Cpu6502::TAY() {
    return 0;
}

uint8_t Cpu6502::TSX() {
    return 0;
}

uint8_t Cpu6502::TXA() {
    return 0;
}

uint8_t Cpu6502::TXS() {
    return 0;
}

uint8_t Cpu6502::TYA(){
    return 0;
}

uint8_t Cpu6502::XXX() {
    return 0;
}