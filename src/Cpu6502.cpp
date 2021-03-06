#include "Cpu6502.h"
#include "Buss.h"

Cpu6502::Cpu6502() {
    using c = Cpu6502;
    m_operations = {
        {"BRK", &c::BRK, &c::IMP, 7}, {"ORA", &c::ORA, &c::IDX, 6}, {"XXX", &c::XXX, &c::IMP, 2}, {"SLO", &c::SLO, &c::IDX, 8}, {"XXX", &c::XXX, &c::ZPA, 3}, {"ORA", &c::ORA, &c::ZPA, 3}, {"ASL", &c::ASL, &c::ZPA, 5}, {"SLO", &c::SLO, &c::ZPA, 5}, {"PHP", &c::PHP, &c::IMP, 3}, {"ORA", &c::ORA, &c::IMM, 2}, {"ASL", &c::ASL, &c::ACC, 2}, {"XXX", &c::XXX, &c::IMM, 2}, {"XXX", &c::XXX, &c::ABS, 4}, {"ORA", &c::ORA, &c::ABS, 4}, {"ASL", &c::ASL, &c::ABS, 6}, {"SLO", &c::SLO, &c::ABS, 6},
        {"BPL", &c::BPL, &c::REL, 2}, {"ORA", &c::ORA, &c::IDY, 5}, {"XXX", &c::XXX, &c::IMP, 2}, {"SLO", &c::SLO, &c::IDY, 8}, {"XXX", &c::XXX, &c::ZPX, 4}, {"ORA", &c::ORA, &c::ZPX, 4}, {"ASL", &c::ASL, &c::ZPX, 6}, {"SLO", &c::SLO, &c::ZPX, 6}, {"CLC", &c::CLC, &c::IMP, 2}, {"ORA", &c::ORA, &c::ABY, 4}, {"XXX", &c::XXX, &c::IMP, 2}, {"SLO", &c::SLO, &c::ABY, 7}, {"XXX", &c::XXX, &c::ABX, 4}, {"ORA", &c::ORA, &c::ABX, 4}, {"ASL", &c::ASL, &c::ABX, 7}, {"SLO", &c::SLO, &c::ABX, 7},
        {"JSR", &c::JSR, &c::ABS, 6}, {"AND", &c::AND, &c::IDX, 6}, {"XXX", &c::XXX, &c::IMP, 2}, {"RLA", &c::RLA, &c::IDX, 8}, {"BIT", &c::BIT, &c::ZPA, 3}, {"AND", &c::AND, &c::ZPA, 3}, {"ROL", &c::ROL, &c::ZPA, 5}, {"RLA", &c::RLA, &c::ZPA, 5}, {"PLP", &c::PLP, &c::IMP, 4}, {"AND", &c::AND, &c::IMM, 2}, {"ROL", &c::ROL, &c::ACC, 2}, {"XXX", &c::XXX, &c::IMM, 2}, {"BIT", &c::BIT, &c::ABS, 4}, {"AND", &c::AND, &c::ABS, 4}, {"ROL", &c::ROL, &c::ABS, 6}, {"RLA", &c::RLA, &c::ABS, 6},
        {"BMI", &c::BMI, &c::REL, 2}, {"AND", &c::AND, &c::IDY, 5}, {"XXX", &c::XXX, &c::IMP, 2}, {"RLA", &c::RLA, &c::IDY, 8}, {"XXX", &c::XXX, &c::ZPX, 4}, {"AND", &c::AND, &c::ZPX, 4}, {"ROL", &c::ROL, &c::ZPX, 6}, {"RLA", &c::RLA, &c::ZPX, 6}, {"SEC", &c::SEC, &c::IMP, 2}, {"AND", &c::AND, &c::ABY, 4}, {"XXX", &c::XXX, &c::IMP, 2}, {"RLA", &c::RLA, &c::ABY, 7}, {"XXX", &c::XXX, &c::ABX, 4}, {"AND", &c::AND, &c::ABX, 4}, {"ROL", &c::ROL, &c::ABX, 7}, {"RLA", &c::RLA, &c::ABX, 7},
        {"RTI", &c::RTI, &c::IMP, 6}, {"EOR", &c::EOR, &c::IDX, 6}, {"XXX", &c::XXX, &c::IMP, 2}, {"SRE", &c::SRE, &c::IDX, 8}, {"XXX", &c::XXX, &c::ZPA, 3}, {"EOR", &c::EOR, &c::ZPA, 3}, {"LSR", &c::LSR, &c::ZPA, 5}, {"SRE", &c::SRE, &c::ZPA, 5}, {"PHA", &c::PHA, &c::IMP, 3}, {"EOR", &c::EOR, &c::IMM, 2}, {"LSR", &c::LSR, &c::ACC, 2}, {"XXX", &c::XXX, &c::IMM, 2}, {"JMP", &c::JMP, &c::ABS, 3}, {"EOR", &c::EOR, &c::ABS, 4}, {"LSR", &c::LSR, &c::ABS, 6}, {"SRE", &c::SRE, &c::ABS, 6},
        {"BVC", &c::BVC, &c::REL, 2}, {"EOR", &c::EOR, &c::IDY, 5}, {"XXX", &c::XXX, &c::IMP, 2}, {"SRE", &c::SRE, &c::IDY, 8}, {"XXX", &c::XXX, &c::IDX, 4}, {"EOR", &c::EOR, &c::ZPX, 4}, {"LSR", &c::LSR, &c::ZPX, 6}, {"SRE", &c::SRE, &c::ZPX, 6}, {"CLI", &c::CLI, &c::IMP, 2}, {"EOR", &c::EOR, &c::ABY, 4}, {"XXX", &c::XXX, &c::IMP, 2}, {"SRE", &c::SRE, &c::ABY, 7}, {"XXX", &c::XXX, &c::ABX, 4}, {"EOR", &c::EOR, &c::ABX, 4}, {"LSR", &c::LSR, &c::ABX, 7}, {"SRE", &c::SRE, &c::ABX, 7},
        {"RTS", &c::RTS, &c::IMP, 6}, {"ADC", &c::ADC, &c::IDX, 6}, {"XXX", &c::XXX, &c::IMP, 2}, {"RRA", &c::RRA, &c::IDX, 8}, {"XXX", &c::XXX, &c::ZPA, 3}, {"ADC", &c::ADC, &c::ZPA, 3}, {"ROR", &c::ROR, &c::ZPA, 5}, {"RRA", &c::RRA, &c::ZPA, 5}, {"PLA", &c::PLA, &c::IMP, 4}, {"ADC", &c::ADC, &c::IMM, 2}, {"ROR", &c::ROR, &c::ACC, 2}, {"XXX", &c::XXX, &c::IMM, 2}, {"JMP", &c::JMP, &c::ABI, 6}, {"ADC", &c::ADC, &c::ABS, 4}, {"ROR", &c::ROR, &c::ABS, 6}, {"RRA", &c::RRA, &c::ABS, 6}, 
        {"BVS", &c::BVS, &c::REL, 2}, {"ADC", &c::ADC, &c::IDY, 5}, {"XXX", &c::XXX, &c::IMP, 2}, {"RRA", &c::RRA, &c::IDY, 8}, {"XXX", &c::XXX, &c::ZPX, 4}, {"ADC", &c::ADC, &c::ZPX, 4}, {"ROR", &c::ROR, &c::ZPX, 6}, {"RRA", &c::RRA, &c::ZPX, 6}, {"SEI", &c::SEI, &c::IMP, 2}, {"ADC", &c::ADC, &c::ABY, 4}, {"XXX", &c::XXX, &c::IMP, 2}, {"RRA", &c::RRA, &c::ABY, 7}, {"XXX", &c::XXX, &c::ABX, 4}, {"ADC", &c::ADC, &c::ABX, 4}, {"ROR", &c::ROR, &c::ABX, 7}, {"RRA", &c::RRA, &c::ABX, 7},
        {"XXX", &c::XXX, &c::IMM, 2}, {"STA", &c::STA, &c::IDX, 6}, {"XXX", &c::XXX, &c::IMM, 2}, {"SAX", &c::SAX, &c::IDX, 6}, {"STY", &c::STY, &c::ZPA, 3}, {"STA", &c::STA, &c::ZPA, 3}, {"STX", &c::STX, &c::ZPA, 3}, {"SAX", &c::SAX, &c::ZPA, 3}, {"DEY", &c::DEY, &c::IMP, 2}, {"BIT", &c::BIT, &c::IMM, 2}, {"TXA", &c::TXA, &c::IMP, 2}, {"XXX", &c::XXX, &c::IMM, 2}, {"STY", &c::STY, &c::ABS, 4}, {"STA", &c::STA, &c::ABS, 4}, {"STX", &c::STX, &c::ABS, 4}, {"SAX", &c::SAX, &c::ABS, 4},
        {"BCC", &c::BCC, &c::REL, 2}, {"STA", &c::STA, &c::IDY, 6}, {"XXX", &c::XXX, &c::IMP, 2}, {"XXX", &c::XXX, &c::IDY, 6}, {"STY", &c::STY, &c::ZPX, 4}, {"STA", &c::STA, &c::ZPX, 4}, {"STX", &c::STX, &c::ZPY, 4}, {"SAX", &c::SAX, &c::ZPY, 4}, {"TYA", &c::TYA, &c::IMP, 2}, {"STA", &c::STA, &c::ABY, 5}, {"TXS", &c::TXS, &c::IMP, 2}, {"XXX", &c::XXX, &c::ABY, 5}, {"XXX", &c::XXX, &c::ABX, 5}, {"STA", &c::STA, &c::ABX, 5}, {"XXX", &c::XXX, &c::ABY, 5}, {"XXX", &c::XXX, &c::ABY, 5},
        {"LDY", &c::LDY, &c::IMM, 2}, {"LDA", &c::LDA, &c::IDX, 6}, {"LDX", &c::LDX, &c::IMM, 2}, {"LAX", &c::LAX, &c::IDX, 6}, {"LDY", &c::LDY, &c::ZPA, 3}, {"LDA", &c::LDA, &c::ZPA, 3}, {"LDX", &c::LDX, &c::ZPA, 3}, {"LAX", &c::LAX, &c::ZPA, 3}, {"TAY", &c::TAY, &c::IMP, 2}, {"LDA", &c::LDA, &c::IMM, 2}, {"TAX", &c::TAX, &c::IMP, 2}, {"LAX", &c::LAX, &c::IMM, 2}, {"LDY", &c::LDY, &c::ABS, 4}, {"LDA", &c::LDA, &c::ABS, 4}, {"LDX", &c::LDX, &c::ABS, 4}, {"LAX", &c::LAX, &c::ABS, 4},
        {"BCS", &c::BCS, &c::REL, 2}, {"LDA", &c::LDA, &c::IDY, 5}, {"XXX", &c::XXX, &c::IMP, 2}, {"LAX", &c::LAX, &c::IDY, 5}, {"LDY", &c::LDY, &c::ZPX, 4}, {"LDA", &c::LDA, &c::ZPX, 4}, {"LDX", &c::LDX, &c::ZPY, 4}, {"LAX", &c::LAX, &c::ZPY, 4}, {"CLV", &c::CLV, &c::IMP, 2}, {"LDA", &c::LDA, &c::ABY, 4}, {"TSX", &c::TSX, &c::IMP, 2}, {"XXX", &c::XXX, &c::ABY, 4}, {"LDY", &c::LDY, &c::ABX, 4}, {"LDA", &c::LDA, &c::ABX, 4}, {"LDX", &c::LDX, &c::ABY, 4}, {"LAX", &c::LAX, &c::ABY, 4},
        {"CPY", &c::CPY, &c::IMM, 2}, {"CMP", &c::CMP, &c::IDX, 6}, {"XXX", &c::XXX, &c::IMM, 2}, {"DCP", &c::DCP, &c::IDX, 8}, {"CPY", &c::CPY, &c::ZPA, 3}, {"CMP", &c::CMP, &c::ZPA, 3}, {"DEC", &c::DEC, &c::ZPA, 5}, {"DCP", &c::DCP, &c::ZPA, 5}, {"INY", &c::INY, &c::IMP, 2}, {"CMP", &c::CMP, &c::IMM, 2}, {"DEX", &c::DEX, &c::IMP, 2}, {"XXX", &c::XXX, &c::IMM, 2}, {"CPY", &c::CPY, &c::ABS, 4}, {"CMP", &c::CMP, &c::ABS, 4}, {"DEC", &c::DEC, &c::ABS, 6}, {"DCP", &c::DCP, &c::ABS, 6},
        {"BNE", &c::BNE, &c::REL, 2}, {"CMP", &c::CMP, &c::IDY, 5}, {"XXX", &c::XXX, &c::IMP, 2}, {"DCP", &c::DCP, &c::IDY, 8}, {"XXX", &c::XXX, &c::ZPX, 4}, {"CMP", &c::CMP, &c::ZPX, 4}, {"DEC", &c::DEC, &c::ZPX, 6}, {"DCP", &c::DCP, &c::ZPX, 6}, {"CLD", &c::CLD, &c::IMP, 2}, {"CMP", &c::CMP, &c::ABY, 4}, {"XXX", &c::XXX, &c::IMP, 2}, {"DCP", &c::DCP, &c::ABY, 7}, {"XXX", &c::XXX, &c::ABX, 4}, {"CMP", &c::CMP, &c::ABX, 4}, {"DEC", &c::DEC, &c::ABX, 7}, {"DCP", &c::DCP, &c::ABX, 7},
        {"CPX", &c::CPX, &c::IMM, 2}, {"SBC", &c::SBC, &c::IDX, 6}, {"XXX", &c::XXX, &c::IMM, 2}, {"ISB", &c::ISB, &c::IDX, 8}, {"CPX", &c::CPX, &c::ZPA, 3}, {"SBC", &c::SBC, &c::ZPA, 3}, {"INC", &c::INC, &c::ZPA, 5}, {"ISB", &c::ISB, &c::ZPA, 5}, {"INX", &c::INX, &c::IMP, 2}, {"SBC", &c::SBC, &c::IMM, 2}, {"NOP", &c::NOP, &c::IMP, 2}, {"SBC", &c::SBC, &c::IMM, 2}, {"CPX", &c::CPX, &c::ABS, 4}, {"SBC", &c::SBC, &c::ABS, 4}, {"INC", &c::INC, &c::ABS, 6}, {"ISB", &c::ISB, &c::ABS, 6},
        {"BEQ", &c::BEQ, &c::REL, 2}, {"SBC", &c::SBC, &c::IDY, 5}, {"XXX", &c::XXX, &c::IMP, 2}, {"ISB", &c::ISB, &c::IDY, 8}, {"XXX", &c::XXX, &c::ZPX, 4}, {"SBC", &c::SBC, &c::ZPX, 4}, {"INC", &c::INC, &c::ZPX, 6}, {"ISB", &c::ISB, &c::ZPX, 6}, {"SED", &c::SED, &c::IMP, 2}, {"SBC", &c::SBC, &c::ABY, 4}, {"XXX", &c::XXX, &c::IMP, 2}, {"ISB", &c::ISB, &c::ABY, 7}, {"XXX", &c::XXX, &c::ABX, 4}, {"SBC", &c::SBC, &c::ABX, 4}, {"INC", &c::INC, &c::ABX, 7}, {"ISB", &c::ISB, &c::ABX, 7}   
    };

    P = 0x34;
    A = X = Y = 0;
    SP = 0xFD;
}

void Cpu6502::clock() {
    if(m_cycles == 0) { // Fetch
        if(nmiPin) {
            nmiPin = false;
            nmi();
        }
        else if(irqPin && !getFlag(Flags::I)) {
            irqPin = false;
            interrupt();
        }
        else {
            m_ins = read(PC);
            ++PC;
            m_cycles = this->m_operations[m_ins].cycles;
            m_cycles += (this->*m_operations[m_ins].addressing)();

            m_cycles += (this->*m_operations[m_ins].op)(); // execute
        }
    }
    --m_cycles;
}

void Cpu6502::registerBuss(Buss* buss) {
    m_buss = buss;
}

void Cpu6502::reset() {
    PC = read(0xFFFC);          // Read low byte of reset vector
    PC |= read(0xFFFD) << 8;    // Read high byte of reset vector

    SP -= 3;
    setFlag(Flags::I, true);
    setFlag(Flags::U, true);

    m_cycles = 7;
}

void Cpu6502::suspend(uint32_t cycles) {
    m_cycles += cycles;
}

uint32_t Cpu6502::getCycle() {
    return m_cycles;
}

uint8_t Cpu6502::read(const uint16_t& address) {
    return m_buss->read(address);
}

void Cpu6502::write(const uint16_t& address, const uint8_t& data) {
    m_buss->write(address, data);
}

void Cpu6502::interrupt() {
    push((PC >> 8) & 0x00FF);
    push(PC & 0x00FF);

    // The break bit, which is usually pushed high, is forced low when pushed during an nmi or irq
    setFlag(Flags::B, 0);
    setFlag(Flags::U, 1);
    push(P);
    setFlag(Flags::I, 1);


    uint16_t irqHandlerAddrLow = read(0xFFFE);
    uint16_t irqHandlerAddrHigh = read(0xFFFF);
    PC = (irqHandlerAddrHigh << 8) | irqHandlerAddrLow;

    m_cycles = 7;
}

void Cpu6502::nmi() {
    push((PC >> 8) & 0x00FF);
    push(PC & 0x00FF);

    // The break bit, which is usually pushed high, is forced low when pushed during an nmi or irq
    setFlag(Flags::B, 0);
    setFlag(Flags::U, 1);
    push(P);
    setFlag(Flags::I, 1);

    uint16_t irqHandlerAddrLow = read(0xFFFA);
    uint16_t irqHandlerAddrHigh = read(0xFFFB);
    PC = (irqHandlerAddrHigh << 8) | irqHandlerAddrLow;

    m_cycles = 7;
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
    return (P & (1 << flag)) >> flag;
}

void Cpu6502::setFlag(const Flags& flag, const bool& value) {
    if(value) P = P | (1 << flag);
    else      P = P & ~(1 << flag);
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
    int8_t relAddr = read(PC++);
    m_operandAddress = (uint8_t)relAddr;
    if(((PC & 0x00FF) + relAddr) & 0xFF00) {
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

// Indirect Indexed Addressing / Indirect Y: The second byte of the instruction points to an address in zero page.
//      The contents of that address and the contents of the next address in zero page are the low and high byte respectively of
//      an address. This address is added together with the Y register, if this addition causes a page boundrary to be crossed
//      another clock cycle is needed.
uint8_t Cpu6502::IDY() {
    uint16_t addressPtr = read(PC++);

    uint8_t low = read(addressPtr); // Set low byte
    uint8_t high = read((addressPtr + 1) & 0xFF); // Set high byte

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
    setFlag(Flags::Z, (res & 0x00FF) == 0);
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
    uint8_t operand = getOperand();
    uint8_t res = A & operand;
    setFlag(Flags::Z, res == 0);
    setFlag(Flags::V, operand & 0x40);
    setFlag(Flags::N, operand & 0x80);
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
    PC++;

    push((PC >> 8) & 0x00FF);
    push(PC & 0x00FF);

    setFlag(Flags::B, 1);
    setFlag(Flags::U, 1);
    push(P); // The break bit bit is set to high when read
    setFlag(Flags::B, 0);
    setFlag(Flags::I, 1);

    uint16_t irqHandlerAddrLow = read(0xFFFE);
    uint16_t irqHandlerAddrHigh = read(0xFFFF);
    PC = (irqHandlerAddrHigh << 8) | irqHandlerAddrLow;

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
    push(P | 0x30); // The break bit and the unused bit are set to high when read
    return 0;
}

// PuLl Accumulator
uint8_t Cpu6502::PLA() {
    A = pull();
    setFlag(Flags::Z, A == 0);
    setFlag(Flags::N, A & 0x80);
    return 0;
}

// PuLl Processor status
uint8_t Cpu6502::PLP() {
    P = pull();
    setFlag(Flags::U, true);
    setFlag(Flags::B, false);
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
    setFlag(Flags::Z, (res & 0x00FF) == 0);
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

// ReTurn from Interrupt
uint8_t Cpu6502::RTI() {
    PLP();
    uint8_t pcLow = pull();
    uint8_t pcHigh = pull();
    PC = (pcHigh << 8) | pcLow;

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

// SuBtract with Carry
uint8_t Cpu6502::SBC() {
    uint8_t operand = getOperand();
    operand = ~operand;
    uint16_t res = A + operand + getFlag(Flags::C);
    setFlag(Flags::C, res & 0x0100);
    setFlag(Flags::Z, (res & 0x00FF) == 0);
    setFlag(Flags::V, (~(A ^ operand) & (A ^ res)) & 0x0080);
    setFlag(Flags::N, res & 0x0080);

    A = res & 0x00FF;
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

// Transfer A to X
uint8_t Cpu6502::TAX() {
    X = A;
    setFlag(Flags::Z, X == 0);
    setFlag(Flags::N, X & 0x80);
    return 0;
}

// Transfer A to Y
uint8_t Cpu6502::TAY() {
    Y = A;
    setFlag(Flags::Z, Y == 0);
    setFlag(Flags::N, Y & 0x80);
    return 0;
}

// Transfer Stack ptr to X
uint8_t Cpu6502::TSX() {
    X = SP;
    setFlag(Flags::Z, X == 0);
    setFlag(Flags::N, X & 0x80);
    return 0;
}

// Transfer X to Accumulator
uint8_t Cpu6502::TXA() {
    A = X;
    setFlag(Flags::Z, A == 0);
    setFlag(Flags::N, A & 0x80);
    return 0;
}

// Transfer X to Stack ptr
uint8_t Cpu6502::TXS() {
    SP = X;
    return 0;
}

// Transfer Y to Accumulator
uint8_t Cpu6502::TYA(){
    A = Y;
    setFlag(Flags::Z, Y == 0);
    setFlag(Flags::N, Y & 0x80);
    return 0;
}

// Illegal opcodes

uint8_t Cpu6502::LAX() {
    LDA();
    TAX();
    return 0;
}

uint8_t Cpu6502::SAX() {
    write(m_operandAddress, A & X);
    return 0;
}

uint8_t Cpu6502::DCP() {
    DEC();
    CMP();
    return 0;
}

uint8_t Cpu6502::ISB() {
    INC();
    SBC();
    return 0;
}

uint8_t Cpu6502::SLO() {
    ASL();
    ORA();
    return 0;
}

uint8_t Cpu6502::RLA() {
    ROL();
    AND();
    return 0;
}

uint8_t Cpu6502::SRE() {
    LSR();
    EOR();
    return 0;
}

uint8_t Cpu6502::RRA() {
    ROR();
    ADC();
    return 0;
}

uint8_t Cpu6502::XXX() {
    return 0;
}