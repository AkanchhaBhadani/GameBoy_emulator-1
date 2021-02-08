#include "CPU.h"
#include "InstructionSet.h"

#define BC combine(B, C)
#define DE combine(D, E)
#define HL combine(H, L)

Flag::Flag(){
    reset();
}
void Flag::reset(){
    Z = 0;
    N = 0;
    H = 0;
    C = 0;
}
Register Flag::get_register(){
   Register temp = 0;
   temp = (Z << 7) + (N << 6) + (H << 5) + (C<<4);
   return temp;  
}

CPU::CPU()
{

}

void CPU::connect(MMU* p_mmu)
{
    this->p_mmu = p_mmu;
}

void CPU::reset()
{
    A = 0;
    B = 0;
    C = 0; 
    D = 0; 
    E = 0;
    F.reset();
    H = 0;
    L = 0;
    SP = 0;
    PC = 0;
}

Pointer CPU::combine(Byte hi, Byte lo)
{
    Pointer result = (hi<<8) + lo;
    return result;
}

void CPU::fetch()
{
    opcode = p_mmu->read(PC++);
}

void CPU::decode()
{
    int args = instruction_data[opcode].length - 1;
    if( args > 0 )
        arg1 = p_mmu->read(PC++);
    if( args > 1 )
        arg2 = p_mmu->read(PC++);
}

void CPU::execute()
{
    opcode_lookup[opcode]();
}

void CPU::NOP(){}

void CPU::LD_R(Register& r, Byte value)
{
    r = value;
}

void CPU::LD_R_P(Register& r, Pointer address)
{
    r = p_mmu->read( address );
}

void CPU::LD_P(Pointer address, Byte value)
{
    p_mmu->write(address, value);
}

void CPU::LD16_R(Register& hi, Register& lo, Pointer value )
{
    lo = (Byte)(0x00ff & value);
    hi = (Byte)(value >> 8);
}

void CPU::LD16_R(Pointer& r, Pointer value)
{
    r = value;
}

void CPU::ADD( Byte value ){}
void CPU::SUB( Byte value ){}
void CPU::AND( Byte value ){}
void CPU::OR ( Byte value ){}
void CPU::ADC( Byte value ){}
void CPU::SBC( Byte value ){}
void CPU::XOR( Byte value ){}
void CPU::CP ( Byte value ){}
void CPU::INC_R(Register& r){}
void CPU::DEC_R(Register& r){}
void CPU::INC_P(Pointer p){}
void CPU::DEC_P(Pointer p){}
void CPU::INC16(Register& hi, Register& lo){}
void CPU::INC16(Pointer& r){}
void CPU::DEC16(Register& hi, Register& lo){}
void CPU::DEC16(Pointer& r){}
void CPU::POP (Register& hi, Register& lo){}
void CPU::PUSH(Register& hi, Register& lo){}

void CPU::init(){
    opcode_lookup[0x00] = [this](){ NOP(); };
    opcode_lookup[0x10] = [this](){};
    opcode_lookup[0x20] = [this](){};
    opcode_lookup[0x30] = [this](){};
    opcode_lookup[0x40] = [this](){ LD_R(B, B); };
    opcode_lookup[0x50] = [this](){ LD_R(D, B); };
    opcode_lookup[0x60] = [this](){ LD_R(H, B); };
    opcode_lookup[0x70] = [this](){ LD_P(HL,B); };
    opcode_lookup[0x80] = [this](){ ADD(B); };
    opcode_lookup[0x90] = [this](){ SUB(B); };
    opcode_lookup[0xa0] = [this](){ AND(B); };
    opcode_lookup[0xb0] = [this](){ OR (B); };
    opcode_lookup[0xc0] = [this](){};
    opcode_lookup[0xd0] = [this](){};
    opcode_lookup[0xe0] = [this](){ LD_P(0xff00 + arg1, A); };
    opcode_lookup[0xf0] = [this](){ LD_R(A, 0xff00 + arg1); };

    opcode_lookup[0x01] = [this](){ LD_R(B, arg2); LD_R(C, arg1); };
    opcode_lookup[0x11] = [this](){ LD_R(D, arg2); LD_R(E, arg1); };
    opcode_lookup[0x21] = [this](){ LD_R(H, arg2); LD_R(L, arg1); };
    opcode_lookup[0x31] = [this](){ LD16_R(SP, combine(arg2,arg1)); };
    opcode_lookup[0x41] = [this](){ LD_R(B, C); };
    opcode_lookup[0x51] = [this](){ LD_R(D, C); };
    opcode_lookup[0x61] = [this](){ LD_R(H, C); };
    opcode_lookup[0x71] = [this](){ LD_P(HL,C); };
    opcode_lookup[0x81] = [this](){ ADD(C); };
    opcode_lookup[0x91] = [this](){ SUB(C); };
    opcode_lookup[0xa1] = [this](){ AND(C); };
    opcode_lookup[0xb1] = [this](){ OR (C); };
    opcode_lookup[0xc1] = [this](){ POP(B, C); };
    opcode_lookup[0xd1] = [this](){ POP(D, E); };
    opcode_lookup[0xe1] = [this](){ POP(H, L); };
    opcode_lookup[0xf1] = [this](){ /* Change flag */ };

    opcode_lookup[0x02] = [this](){ LD_P(BC, A); };
    opcode_lookup[0x12] = [this](){ LD_P(DE, A); };
    opcode_lookup[0x22] = [this](){ LD_P(HL, A); LD16_R(H, L, HL + 1); };
    opcode_lookup[0x32] = [this](){ LD_P(HL, A); LD16_R(H, L, HL - 1); };
    opcode_lookup[0x42] = [this](){ LD_R(B, D); };
    opcode_lookup[0x52] = [this](){ LD_R(D, D); };
    opcode_lookup[0x62] = [this](){ LD_R(H, D); };
    opcode_lookup[0x72] = [this](){ LD_P(HL,D); };
    opcode_lookup[0x82] = [this](){ ADD(D); };
    opcode_lookup[0x92] = [this](){ SUB(D); };
    opcode_lookup[0xa2] = [this](){ AND(D); };
    opcode_lookup[0xb2] = [this](){ OR (D); };
    opcode_lookup[0xc2] = [this](){};
    opcode_lookup[0xd2] = [this](){};
    opcode_lookup[0xe2] = [this](){ LD_P(0xff00 + C, A); };
    opcode_lookup[0xf2] = [this](){ LD_R(A, 0xff00 + C); };

    opcode_lookup[0x03] = [this](){ INC16(B,C); };
    opcode_lookup[0x13] = [this](){ INC16(D,E); };
    opcode_lookup[0x23] = [this](){ INC16(H,L); };
    opcode_lookup[0x33] = [this](){ INC16(SP);  };
    opcode_lookup[0x43] = [this](){ LD_R(B, E); };
    opcode_lookup[0x53] = [this](){ LD_R(D, E); };
    opcode_lookup[0x63] = [this](){ LD_R(H, E); };
    opcode_lookup[0x73] = [this](){ LD_P(HL,E); };
    opcode_lookup[0x83] = [this](){ ADD(E); };
    opcode_lookup[0x93] = [this](){ SUB(E); };
    opcode_lookup[0xa3] = [this](){ AND(E); };
    opcode_lookup[0xb3] = [this](){ OR (E); };
    opcode_lookup[0xc3] = [this](){};
    opcode_lookup[0xd3] = [this](){};
    opcode_lookup[0xe3] = [this](){};
    opcode_lookup[0xf3] = [this](){};

    opcode_lookup[0x04] = [this](){ INC_R(B); };
    opcode_lookup[0x14] = [this](){ INC_R(D); };
    opcode_lookup[0x24] = [this](){ INC_R(H); };
    opcode_lookup[0x34] = [this](){ INC_P(HL); };
    opcode_lookup[0x44] = [this](){ LD_R(B, H); };
    opcode_lookup[0x54] = [this](){ LD_R(D, H); };
    opcode_lookup[0x64] = [this](){ LD_R(H, H); };
    opcode_lookup[0x74] = [this](){ LD_P(HL,H); };
    opcode_lookup[0x84] = [this](){ ADD(H); };
    opcode_lookup[0x94] = [this](){ SUB(H); };
    opcode_lookup[0xa4] = [this](){ AND(H); };
    opcode_lookup[0xb4] = [this](){ OR (H); };
    opcode_lookup[0xc4] = [this](){};
    opcode_lookup[0xd4] = [this](){};
    opcode_lookup[0xe4] = [this](){};
    opcode_lookup[0xf4] = [this](){};

    opcode_lookup[0x05] = [this](){ DEC_R(B); };
    opcode_lookup[0x15] = [this](){ DEC_R(D); };
    opcode_lookup[0x25] = [this](){ DEC_R(H); };
    opcode_lookup[0x35] = [this](){ DEC_P(HL); };
    opcode_lookup[0x45] = [this](){ LD_R(B, L); };
    opcode_lookup[0x55] = [this](){ LD_R(D, L); };
    opcode_lookup[0x65] = [this](){ LD_R(H, L); };
    opcode_lookup[0x75] = [this](){ LD_P(HL,L); };
    opcode_lookup[0x85] = [this](){ ADD(L); };
    opcode_lookup[0x95] = [this](){ SUB(L); };
    opcode_lookup[0xa5] = [this](){ AND(L); };
    opcode_lookup[0xb5] = [this](){ OR (L); };
    opcode_lookup[0xc5] = [this](){ PUSH(B, C); };
    opcode_lookup[0xd5] = [this](){ PUSH(D, E); };
    opcode_lookup[0xe5] = [this](){ PUSH(H, L); };
    opcode_lookup[0xf5] = [this](){};

}