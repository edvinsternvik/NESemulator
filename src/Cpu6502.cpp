#include "Cpu6502.h"
#include "Buss.h"
#include <iostream>

Cpu6502::Cpu6502() {
    using c = Cpu6502;
    m_operations = {
        {"BRK", &c::BRK, &c::IMP, 7}, {"ORA", &c::ORA, &c::IDX, 6}, {"XXX", &c::XXX, &c::IMP, 2}, {"XXX", &c::XXX, &c::IMP, 2}, {"TSB", &c::TSB, &c::ZPA, 5}, {"ORA", &c::ORA, &c::ZPA, 3}, {"ASL", &c::ASL, &c::ZPA, 5}, {"RMB", &c::RMB, &c::ZPA, 5}, {"PHP", &c::PHP, &c::IMP, 3}, {"ORA", &c::ORA, &c::IMM, 2}, {"ASL", &c::ASL, &c::ACC, 2}, {"XXX", &c::XXX, &c::IMP, 2}, {"TSB", &c::TSB, &c::ABS, 6}, {"ORA", &c::ORA, &c::ABS, 4}, {"ASL", &c::ASL, &c::ABS, 6}, {"BBR", &c::BBR, &c::ZPA, 5},
        {"BPL", &c::BPL, &c::REL, 2}, {"ORA", &c::ORA, &c::INY, 5}, {"ORA", &c::ORA, &c::IND, 5}, {"XXX", &c::XXX, &c::IMP, 2}, {"TRB", &c::TRB, &c::ZPA, 5}, {"ORA", &c::ORA, &c::ZPX, 4}, {"ASL", &c::ASL, &c::ZPX, 6}, {"RMB", &c::RMB, &c::ZPA, 5}, {"CLC", &c::CLC, &c::IMP, 2}, {"ORA", &c::ORA, &c::ABY, 4}, {"INC", &c::INC, &c::ACC, 2}, {"XXX", &c::XXX, &c::IMP, 2}, {"TRB", &c::TRB, &c::ABS, 6}, {"ORA", &c::ORA, &c::ABX, 4}, {"ASL", &c::ASL, &c::ABX, 7}, {"BBR", &c::BBR, &c::ZPA, 5},
        {"JSR", &c::JSR, &c::ABS, 6}, {"AND", &c::AND, &c::IDX, 6}, {"XXX", &c::XXX, &c::IMP, 2}, {"XXX", &c::XXX, &c::IMP, 2}, {"BIT", &c::BIT, &c::ZPA, 3}, {"AND", &c::AND, &c::ZPA, 3}, {"ROL", &c::ROL, &c::ZPA, 5}, {"RMB", &c::RMB, &c::ZPA, 5}, {"PLP", &c::PLP, &c::IMP, 4}, {"AND", &c::AND, &c::IMM, 2}, {"ROL", &c::ROL, &c::ACC, 2}, {"XXX", &c::XXX, &c::IMP, 2}, {"BIT", &c::BIT, &c::ABS, 4}, {"AND", &c::AND, &c::ABS, 4}, {"ROL", &c::ROL, &c::ABS, 6}, {"BBR", &c::BBR, &c::ZPA, 5},
        {"BMI", &c::BMI, &c::REL, 2}, {"AND", &c::AND, &c::INY, 5}, {"AND", &c::AND, &c::IND, 5}, {"XXX", &c::XXX, &c::IMP, 2}, {"BIT", &c::BIT, &c::ZPX, 4}, {"AND", &c::AND, &c::ZPX, 4}, {"ROL", &c::ROL, &c::ZPX, 6}, {"RMB", &c::RMB, &c::ZPA, 5}, {"SEC", &c::SEC, &c::IMP, 2}, {"AND", &c::AND, &c::ABY, 4}, {"DEC", &c::DEC, &c::ACC, 2}, {"XXX", &c::XXX, &c::IMP, 2}, {"BIT", &c::BIT, &c::ABX, 4}, {"AND", &c::AND, &c::ABX, 4}, {"ROL", &c::ROL, &c::ABX, 7}, {"BBR", &c::BBR, &c::ZPA, 5},
        {"RTI", &c::RTI, &c::IMP, 6}, {"EOR", &c::EOR, &c::IDX, 6}, {"XXX", &c::XXX, &c::IMP, 2}, {"XXX", &c::XXX, &c::IMP, 2}, {"XXX", &c::XXX, &c::IMP, 2}, {"EOR", &c::EOR, &c::ZPA, 3}, {"LSR", &c::LSR, &c::ZPA, 5}, {"RMB", &c::RMB, &c::ZPA, 5}, {"PHA", &c::PHA, &c::IMP, 3}, {"EOR", &c::EOR, &c::IMM, 2}, {"LSR", &c::LSR, &c::ACC, 2}, {"XXX", &c::XXX, &c::IMP, 2}, {"JMP", &c::JMP, &c::ABS, 3}, {"EOR", &c::EOR, &c::ABS, 4}, {"LSR", &c::LSR, &c::ABS, 6}, {"BBR", &c::BBR, &c::ZPA, 5},
        {"BVC", &c::BVC, &c::REL, 2}, {"EOR", &c::EOR, &c::INY, 5}, {"EOR", &c::EOR, &c::IND, 5}, {"XXX", &c::XXX, &c::IMP, 2}, {"XXX", &c::XXX, &c::IMP, 2}, {"EOR", &c::EOR, &c::ZPX, 4}, {"LSR", &c::LSR, &c::ZPX, 6}, {"RMB", &c::RMB, &c::ZPA, 5}, {"CLI", &c::CLI, &c::IMP, 2}, {"EOR", &c::EOR, &c::ABY, 4}, {"PHY", &c::PHY, &c::IMP, 3}, {"XXX", &c::XXX, &c::IMP, 2}, {"XXX", &c::XXX, &c::IMP, 2}, {"EOR", &c::EOR, &c::ABX, 4}, {"LSR", &c::LSR, &c::ABX, 7}, {"BBR", &c::BBR, &c::ZPA, 5},
        {"RTS", &c::RTS, &c::IMP, 6}, {"ADC", &c::ADC, &c::IDX, 6}, {"XXX", &c::XXX, &c::IMP, 2}, {"XXX", &c::XXX, &c::IMP, 2}, {"STZ", &c::STZ, &c::ZPA, 3}, {"ADC", &c::ADC, &c::ZPA, 3}, {"ROR", &c::ROR, &c::ZPA, 5}, {"RMB", &c::RMB, &c::ZPA, 5}, {"PLA", &c::PLA, &c::IMP, 4}, {"ADC", &c::ADC, &c::IMM, 2}, {"ROR", &c::ROR, &c::ACC, 2}, {"XXX", &c::XXX, &c::IMP, 2}, {"JMP", &c::JMP, &c::ABS, 6}, {"ADC", &c::ADC, &c::ABS, 4}, {"ROR", &c::ROR, &c::ABS, 6}, {"BBR", &c::BBR, &c::ZPA, 5}, 
        {"BVS", &c::BVS, &c::REL, 2}, {"ADC", &c::ADC, &c::INY, 5}, {"ADC", &c::ADC, &c::IND, 5}, {"XXX", &c::XXX, &c::IMP, 2}, {"STZ", &c::STZ, &c::ZPX, 4}, {"ADC", &c::ADC, &c::ZPX, 4}, {"ROR", &c::ROR, &c::ZPX, 6}, {"RMB", &c::RMB, &c::ZPA, 5}, {"SEI", &c::SEI, &c::IMP, 2}, {"ADC", &c::ADC, &c::ABY, 4}, {"PLY", &c::PLY, &c::IMP, 4}, {"XXX", &c::XXX, &c::IMP, 2}, {"JMP", &c::JMP, &c::ABX, 6}, {"ADC", &c::ADC, &c::ABX, 4}, {"ROR", &c::ROR, &c::ABX, 7}, {"BBR", &c::BBR, &c::ZPA, 5},
        {"BRA", &c::BRA, &c::REL, 3}, {"STA", &c::STA, &c::IDX, 6}, {"XXX", &c::XXX, &c::IMP, 2}, {"XXX", &c::XXX, &c::IMP, 2}, {"STY", &c::STY, &c::ZPA, 3}, {"STA", &c::STA, &c::ZPA, 3}, {"STX", &c::STX, &c::ZPA, 3}, {"SMB", &c::SMB, &c::ZPA, 5}, {"DEY", &c::DEY, &c::IMP, 2}, {"BIT", &c::BIT, &c::IMM, 2}, {"TXA", &c::TXA, &c::IMP, 2}, {"XXX", &c::XXX, &c::IMP, 2}, {"STY", &c::STY, &c::ABS, 4}, {"STA", &c::STA, &c::ABS, 4}, {"STX", &c::STX, &c::ABS, 4}, {"BBS", &c::BBS, &c::ZPA, 5},
        {"BCC", &c::BCC, &c::REL, 2}, {"STA", &c::STA, &c::INY, 6}, {"STA", &c::STA, &c::IND, 5}, {"XXX", &c::XXX, &c::IMP, 2}, {"STY", &c::STY, &c::ZPX, 4}, {"STA", &c::STA, &c::ZPX, 4}, {"STX", &c::STX, &c::ZPY, 4}, {"SMB", &c::SMB, &c::ZPA, 5}, {"TYA", &c::TYA, &c::IMP, 2}, {"STA", &c::STA, &c::ABY, 5}, {"TXS", &c::TXS, &c::IMP, 2}, {"XXX", &c::XXX, &c::IMP, 2}, {"STZ", &c::STZ, &c::ABS, 4}, {"STA", &c::STA, &c::ABX, 5}, {"STZ", &c::STZ, &c::ABX, 5}, {"BBS", &c::BBS, &c::ZPA, 5},
        {"LDY", &c::LDY, &c::IMM, 2}, {"LDA", &c::LDA, &c::IDX, 6}, {"LDX", &c::LDX, &c::IMM, 2}, {"XXX", &c::XXX, &c::IMP, 2}, {"LDY", &c::LDY, &c::ZPA, 3}, {"LDA", &c::LDA, &c::ZPA, 3}, {"LDX", &c::LDX, &c::ZPA, 3}, {"SMB", &c::SMB, &c::ZPA, 5}, {"TAY", &c::TAY, &c::IMP, 2}, {"LDA", &c::LDA, &c::IMM, 2}, {"TAX", &c::TAX, &c::IMP, 2}, {"XXX", &c::XXX, &c::IMP, 2}, {"LDY", &c::LDY, &c::ABS, 4}, {"LDA", &c::LDA, &c::ABS, 4}, {"LDX", &c::LDX, &c::ABS, 4}, {"BBS", &c::BBS, &c::ZPA, 5},
        {"BCS", &c::BCS, &c::REL, 2}, {"LDA", &c::LDA, &c::INY, 5}, {"LDA", &c::LDA, &c::IND, 5}, {"XXX", &c::XXX, &c::IMP, 2}, {"LDY", &c::LDY, &c::ZPX, 4}, {"LDA", &c::LDA, &c::ZPX, 4}, {"LDX", &c::LDX, &c::ZPY, 4}, {"SMB", &c::SMB, &c::ZPA, 5}, {"CLV", &c::CLV, &c::IMP, 2}, {"LDA", &c::LDA, &c::ABY, 4}, {"TSX", &c::TSX, &c::IMP, 2}, {"XXX", &c::XXX, &c::IMP, 2}, {"LDY", &c::LDY, &c::ABX, 4}, {"LDA", &c::LDA, &c::ABX, 4}, {"LDX", &c::LDX, &c::ABY, 4}, {"BBS", &c::BBS, &c::ZPA, 5},
        {"CPY", &c::CPY, &c::IMM, 2}, {"CMP", &c::CMP, &c::IDX, 6}, {"XXX", &c::XXX, &c::IMP, 2}, {"XXX", &c::XXX, &c::IMP, 2}, {"CPY", &c::CPY, &c::ZPA, 3}, {"CMP", &c::CMP, &c::ZPA, 3}, {"DEC", &c::DEC, &c::ZPA, 5}, {"SMB", &c::SMB, &c::ZPA, 5}, {"INY", &c::INY, &c::IMP, 2}, {"CMP", &c::CMP, &c::IMM, 2}, {"DEX", &c::DEX, &c::IMP, 2}, {"XXX", &c::XXX, &c::IMP, 2}, {"CPY", &c::CPY, &c::ABS, 4}, {"CMP", &c::CMP, &c::ABS, 4}, {"DEC", &c::DEC, &c::ABS, 6}, {"BBS", &c::BBS, &c::ZPA, 5},
        {"BNE", &c::BNE, &c::REL, 2}, {"CMP", &c::CMP, &c::INY, 5}, {"CMP", &c::CMP, &c::IND, 5}, {"XXX", &c::XXX, &c::IMP, 2}, {"XXX", &c::XXX, &c::IMP, 2}, {"CMP", &c::CMP, &c::ZPX, 4}, {"DEC", &c::DEC, &c::ZPX, 6}, {"SMB", &c::SMB, &c::ZPA, 5}, {"CLD", &c::CLD, &c::IMP, 2}, {"CMP", &c::CMP, &c::ABY, 4}, {"PHX", &c::PHX, &c::IMP, 3}, {"XXX", &c::XXX, &c::IMP, 2}, {"XXX", &c::XXX, &c::IMP, 2}, {"CMP", &c::CMP, &c::ABX, 4}, {"DEC", &c::DEC, &c::ABX, 7}, {"BBS", &c::BBS, &c::ZPA, 5},
        {"CPX", &c::CPX, &c::IMM, 2}, {"SBC", &c::SBC, &c::IDX, 6}, {"XXX", &c::XXX, &c::IMP, 2}, {"XXX", &c::XXX, &c::IMP, 2}, {"CPX", &c::CPX, &c::ZPA, 3}, {"SBC", &c::SBC, &c::ZPA, 3}, {"INC", &c::INC, &c::ZPA, 5}, {"SMB", &c::SMB, &c::ZPA, 5}, {"INX", &c::INX, &c::IMP, 2}, {"SBC", &c::SBC, &c::IMM, 2}, {"NOP", &c::NOP, &c::IMP, 2}, {"XXX", &c::XXX, &c::IMP, 2}, {"CPX", &c::CPX, &c::ABS, 4}, {"SBC", &c::SBC, &c::ABS, 4}, {"INC", &c::INC, &c::ABS, 6}, {"BBS", &c::BBS, &c::ZPA, 5},
        {"BEQ", &c::BEQ, &c::REL, 2}, {"SBC", &c::SBC, &c::INY, 5}, {"SBC", &c::SBC, &c::IND, 5}, {"XXX", &c::XXX, &c::IMP, 2}, {"XXX", &c::XXX, &c::IMP, 2}, {"SBC", &c::SBC, &c::ZPX, 4}, {"INC", &c::INC, &c::ZPX, 6}, {"SMB", &c::SMB, &c::ZPA, 5}, {"SED", &c::SED, &c::IMP, 2}, {"SBC", &c::SBC, &c::ABY, 4}, {"PLX", &c::PLX, &c::IMP, 4}, {"XXX", &c::XXX, &c::IMP, 2}, {"XXX", &c::XXX, &c::IMP, 2}, {"SBC", &c::SBC, &c::ABX, 4}, {"INC", &c::INC, &c::ABX, 7}, {"BBS", &c::BBS, &c::ZPA, 5}   
    };
}

void Cpu6502::clock() {
    if(m_state == 0) { // Fetch
        m_ins = read(PC);
        ++PC;
        (this->*m_operations[m_ins].addressing)();
    }
    else { // Execute
        if(m_state == 1) {
            (this->*m_operations[m_ins].op)();
        }

        if(m_state + 1 == m_operations[m_ins].cycles) { // Check if done with instruction
            m_state = 0;
            return;
        }
    }
    ++m_state;
}

void Cpu6502::registerBuss(Buss* buss) {
    m_buss = buss;
}

uint8_t Cpu6502::read(const uint16_t& address) {
    return m_buss->data[address];
}

void Cpu6502::write(const uint8_t& data) {

}

uint8_t Cpu6502::IMM() {
    m_operand = read(PC++);
    return 0;
}

uint8_t Cpu6502::IMP() {
    return 0;
}

uint8_t Cpu6502::ABS() {
    uint16_t address = read(PC++); // Low byte
    address += read(PC++) << 8;
    m_operand = read(address);
    return 0; 
}

uint8_t Cpu6502::ABY() {
    return 0;
}
uint8_t Cpu6502::ABX() {
    return 0;
}

uint8_t Cpu6502::IND() {
    return 0;
}
uint8_t Cpu6502::IDX() {
    return 0;
}
uint8_t Cpu6502::ACC() {
    return 0;
}
uint8_t Cpu6502::REL() {
    return 0;
}

uint8_t Cpu6502::ZPA() {
    return 0;
}

uint8_t Cpu6502::ZPX() {
    return 0;
}

uint8_t Cpu6502::ZPY() {
    return 0;
}

uint8_t Cpu6502::ADC() {
    return 0;
}

uint8_t Cpu6502::AND() {
    return 0;
}

uint8_t Cpu6502::ASL() {
    return 0;
}

uint8_t Cpu6502::BBR() {
    return 0;
}

uint8_t Cpu6502::BBS() {
    return 0;
}

uint8_t Cpu6502::BCC() {
    return 0;
}

uint8_t Cpu6502::BCS() {
    return 0;
}

uint8_t Cpu6502::BEQ() {
    return 0;
}

uint8_t Cpu6502::BIT() {
    return 0;
}

uint8_t Cpu6502::BMI() {
    return 0;
}

uint8_t Cpu6502::BNE() {
    return 0;
}

uint8_t Cpu6502::BPL() {
    return 0;
}

uint8_t Cpu6502::BRA() {
    return 0;
}

uint8_t Cpu6502::BRK() {
    return 0;
}

uint8_t Cpu6502::BVC() {
    return 0;
}

uint8_t Cpu6502::BVS() {
    return 0;
}

uint8_t Cpu6502::CLC() {
    return 0;
}

uint8_t Cpu6502::CLD() {
    return 0;
}

uint8_t Cpu6502::CLI() {
    return 0;
}

uint8_t Cpu6502::CLV() {
    return 0;
}

uint8_t Cpu6502::CMP(){
    return 0;
}

uint8_t Cpu6502::CPX() {
    return 0;
}

uint8_t Cpu6502::CPY() {
    return 0;
}

uint8_t Cpu6502::DEC() {
    return 0;
}

uint8_t Cpu6502::DEX() {
    return 0;
}

uint8_t Cpu6502::DEY() {
    return 0;
}

uint8_t Cpu6502::EOR() {
    return 0;
}

uint8_t Cpu6502::INC() {
    return 0;
}

uint8_t Cpu6502::INX() {
    return 0;
}

uint8_t Cpu6502::INY() {
    return 0;
}

uint8_t Cpu6502::JMP() {
    return 0;
}

uint8_t Cpu6502::JSR() {
    return 0;
}

uint8_t Cpu6502::LDA() {
    A = m_operand;
    return 0;
}

uint8_t Cpu6502::LDX() {
    return 0;
}

uint8_t Cpu6502::LDY() {
    return 0;
}

uint8_t Cpu6502::LSR() {
    return 0;
}

uint8_t Cpu6502::NOP() {
    return 0;
}

uint8_t Cpu6502::ORA() {
    return 0;
}

uint8_t Cpu6502::PHA() {
    return 0;
}

uint8_t Cpu6502::PHP() {
    return 0;
}

uint8_t Cpu6502::PHX() {
    return 0;
}

uint8_t Cpu6502::PHY() {
    return 0;
}

uint8_t Cpu6502::PLA() {
    return 0;
}

uint8_t Cpu6502::PLP() {
    return 0;
}

uint8_t Cpu6502::PLX() {
    return 0;
}

uint8_t Cpu6502::PLY() {
    return 0;
}

uint8_t Cpu6502::RMB() {
    return 0;
}

uint8_t Cpu6502::ROL() {
    return 0;
}

uint8_t Cpu6502::ROR() {
    return 0;
}

uint8_t Cpu6502::RTI() {
    return 0;
}

uint8_t Cpu6502::RTS() {
    return 0;
}

uint8_t Cpu6502::SBC() {
    return 0;
}

uint8_t Cpu6502::SEC() {
    return 0;
}

uint8_t Cpu6502::SED() {
    return 0;
}

uint8_t Cpu6502::SEI() {
    return 0;
}

uint8_t Cpu6502::SMB() {
    return 0;
}

uint8_t Cpu6502::STA() {
    return 0;
}

uint8_t Cpu6502::STX() {
    return 0;
}

uint8_t Cpu6502::STY() {
    return 0;
}

uint8_t Cpu6502::STZ() {
    return 0;
}

uint8_t Cpu6502::TAX() {
    return 0;
}

uint8_t Cpu6502::TAY() {
    return 0;
}

uint8_t Cpu6502::TRB() {
    return 0;
}
uint8_t Cpu6502::TSB() {
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