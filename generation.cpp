#include "generation.hpp"
#include <cstring>
#include <iostream>
#include <sstream>
using namespace std;

reg_r A_R = 0b111;
reg_r B_R = 0b000;
reg_r C_R = 0b001;
reg_r D_R = 0b010;
reg_r E_R = 0b011;
reg_r H_R = 0b100;
reg_r L_R = 0b101;

reg_dd BC_DD = 0b00;
reg_dd DE_DD = 0b01;
reg_dd HL_DD = 0b10;
reg_dd SP_DD = 0b11;

reg_pp BC_PP = 0b00;
reg_pp DE_PP = 0b01;
reg_pp IX_PP = 0b10;
reg_pp SP_PP = 0b11;

reg_rr BC_RR = 0b00;
reg_rr DE_RR = 0b01;
reg_rr IY_RR = 0b10;
reg_rr SP_RR = 0b11;

reg_qq BC_QQ = 0b00;
reg_qq DE_QQ = 0b01;
reg_qq HL_QQ = 0b10;
reg_qq AF_QQ = 0b11;

status_cc NZ_CC = 0b000;
status_cc Z_CC  = 0b001;
status_cc NC_CC = 0b010;
status_cc C_CC  = 0b011;
status_cc PO_CC = 0b100;
status_cc PE_CC = 0b101;
status_cc P_CC  = 0b110;
status_cc M_CC  = 0b111;

status_c NZ_C = 0b00;
status_c Z_C  = 0b01;
status_c NC_C = 0b10;
status_c C_C  = 0b11;

reset_p R_00H = 0b000;
reset_p R_08H = 0b001;
reset_p R_10H = 0b010;
reset_p R_18H = 0b011;
reset_p R_20H = 0b100;
reset_p R_28H = 0b101;
reset_p R_30H = 0b110;
reset_p R_38H = 0b111;

Instruction newInstruction(Instruction_Tag tag, uint8_t code[INS_LEN], int len) {
    Instruction retVal;
    memset(retVal.code, 0, INS_LEN);
    memcpy(retVal.code, code, len);
    retVal.code_len = len;
    retVal.tag = tag;
    return retVal;
}

void printInstruction(Instruction n) {
    cout << n.tag << "[";
    cout << n.code_len << "] ";
    cout << "{ ";
    for (uint8_t b : n.code) {
        printf("%X ", b);
    }
    cout << "}\n";
}

bool match(Instruction n1, Instruction n2) {
    return (n1.tag == n2.tag) && (n1.code_len == n2.code_len) && (memcmp(n1.code, n2.code, INS_LEN) == 0);
}

Instruction Load_Reg_Reg(reg_r r1, reg_r r2) {
    uint8_t instruction = 0b01000000;
    instruction = instruction | (r1 << 3) | r2;

    uint8_t code[INS_LEN];
    code[0] = instruction;

    return newInstruction(LD_REG_REG, code, 1);
}

Instruction Load_Reg_Int8(reg_r r, uint8_t n) {
    uint8_t instruction = 0b00000110;
    instruction = instruction | (r << 3);

    uint8_t code[INS_LEN];
    code[0] = instruction;
    code[1] = n;

    return newInstruction(LD_REG_INT8, code, 2);
}

Instruction Load_Reg_lHL(reg_r r) {
    uint8_t instruction = 0b01000110;
    instruction = instruction | (r << 3);

    uint8_t code[INS_LEN];
    code[0] = instruction;

    return newInstruction(LD_REG_lHL, code, 1);
}

Instruction Load_Reg_lIXd(reg_r r, uint8_t d) {
    uint8_t instruction = 0b01000110;
    instruction = instruction | (r << 3);

    uint8_t code[INS_LEN];
    code[0] = 0xDD;
    code[1] = instruction;
    code[2] = d;

    return newInstruction(LD_REG_lIXd, code, 3);
}

Instruction Load_Reg_lIYd(reg_r r, uint8_t d) {
    uint8_t instruction = 0b01000110;
    instruction = instruction | (r << 3);

    uint8_t code[INS_LEN];
    code[0] = 0xFD;
    code[1] = instruction;
    code[2] = d;

    return newInstruction(LD_REG_lIYd, code, 3);
}

Instruction Load_lHL_Reg(reg_r r) {
    uint8_t instruction = 0b01110000;
    instruction = instruction | r;

    uint8_t code[INS_LEN];
    code[0] = instruction;

    return newInstruction(LD_lHL_REG, code, 1);
}

Instruction Load_lIXd_Reg(uint8_t d, reg_r r) {
    uint8_t ins = 0b01110000;
    ins = ins | r;

    uint8_t code[INS_LEN];
    code[0] = 0xDD;
    code[1] = ins;
    code[2] = d;

    return newInstruction(LD_lIXd_REG, code, 3);
}

Instruction Load_lIYd_Reg(uint8_t d, reg_r r) {
    uint8_t ins = 0b01110000;
    ins = ins | r;

    uint8_t code[INS_LEN];
    code[0] = 0xFD;
    code[1] = ins;
    code[2] = d;

    return newInstruction(LD_lIYd_REG, code, 3);
}

Instruction Load_lHL_Int8(uint8_t n) {
    uint8_t code[INS_LEN];
    code[0] = 0x36;
    code[1] = n;

    return newInstruction(LD_lHL_INT8, code, 2);
}

Instruction Load_lIXd_Int8(uint8_t d, uint8_t n) {
    uint8_t code[INS_LEN];
    code[0] = 0xDD;
    code[1] = 0x36;
    code[2] = d;
    code[3] = n;

    return newInstruction(LD_lIXd_INT8, code, 4);
}

Instruction Load_lIYd_Int8(uint8_t d, uint8_t n) {
    uint8_t code[INS_LEN];
    code[0] = 0xFD;
    code[1] = 0x36;
    code[2] = d;
    code[3] = n;

    return newInstruction(LD_lIYd_INT8, code, 4);
}

Instruction Load_A_lBC() {
    uint8_t code[INS_LEN];
    code[0] = 0x0A;

    return newInstruction(LD_A_lBC, code, 1);
}

Instruction Load_A_lDE() {
    uint8_t code[INS_LEN];
    code[0] = 0x1A;

    return newInstruction(LD_A_lDE, code, 1);
}

Instruction Load_A_lInt16(uint16_t nn) {
    uint8_t code[INS_LEN];
    code[0] = 0x3A;
    code[2] = nn >> 8; //little endianess
    code[1] = 0x00FF & nn;

    return newInstruction(LD_A_lINT16, code, 3);
}

Instruction Load_lBC_A() {
    uint8_t code[INS_LEN];
    code[0] = 0x02;

    return newInstruction(LD_lBC_A, code, 1);
}

Instruction Load_lDE_A() {
    uint8_t code[INS_LEN];
    code[0] = 0x12;

    return newInstruction(LD_lDE_A, code, 1);
}

Instruction Load_lInt16_A(uint16_t nn) {
    uint8_t code[INS_LEN];
    code[0] = 0x32;
    code[2] = nn >> 8;
    code[1] = 0x00FF & nn;

    return newInstruction(LD_lINT16_A, code, 3);
}

Instruction Load_A_I() {
    uint8_t code[INS_LEN] = {0xED, 0x57, 0x00, 0x00};
    return newInstruction(LD_A_I, code, 2);
}

Instruction Load_A_R() {
    uint8_t code[INS_LEN] = {0xED, 0x5F, 0x00, 0x00};
    return newInstruction(LD_A_R, code, 2);
}

Instruction Load_I_A() {
    uint8_t code[INS_LEN] = {0xED, 0x47, 0x00, 0x00};
    return newInstruction(LD_I_A, code, 2); 
}

Instruction Load_R_A() {
    uint8_t code[INS_LEN] = {0xED, 0x4F, 0x00, 0x00};
    return newInstruction(LD_R_A, code, 2);
}

/* 16bit load group */
Instruction Load_Reg_Int16(reg_dd r, uint16_t nn) {
    uint8_t code[INS_LEN];

    uint8_t opcode = 0b00000001;
    opcode = opcode | (r << 4);
    code[0] = opcode;

    code[2] = (uint8_t) (0x00FF & nn);
    code[1] = (uint8_t) (nn >> 8);

    return newInstruction(LD_REG_INT16, code, 3);
}

Instruction Load_IX_Int16(uint16_t nn) {
    uint8_t code[INS_LEN];
    code[0] = 0xDD;
    code[1] = 0x21;

    code[3] = (uint8_t) (0x00FF & nn);
    code[2] = (uint8_t) (nn >> 8);

    return newInstruction(LD_IX_INT16, code, 4);
}

Instruction Load_IY_Int16(uint16_t nn) {
    uint8_t code[INS_LEN];
    code[0] = 0xFD;
    code[1] = 0x21;

    code[3] = (uint8_t) (0x00FF & nn);
    code[2] = (uint8_t) (nn >> 8);

    return newInstruction(LD_IY_INT16, code, 4);
}   

Instruction Load_HL_lInt16(uint16_t nn) {
    uint8_t code[INS_LEN];

    code[0] = 0x2A;
    code[2] = (uint8_t) (0x00FF & nn);
    code[1] = (uint8_t) (nn >> 8);

    return newInstruction(LD_HL_lINT16, code, 3);
}

Instruction Load_Reg_lInt16(reg_dd r, uint16_t nn) {
    uint8_t code[INS_LEN];
    code[0] = 0xED;

    code[1] = 0b01001011 | (r << 4);

    code[4] = (uint8_t) (0x00FF & nn);
    code[3] = (uint8_t) (0x00FF & (nn >> 8));

    return newInstruction(LD_REG_lINT16, code, 4);
}

Instruction Load_IX_lInt16(uint16_t nn) {
    uint8_t code[INS_LEN];
    code[0] = 0xDD;
    code[1] = 0x2A;

    code[3] = (uint8_t) (0x00FF & nn);
    code[2] = (uint8_t) (nn >> 8);

    return newInstruction(LD_IX_lINT16, code, 4);
}

Instruction Load_IY_lInt16(uint16_t nn) {
    uint8_t code[INS_LEN];
    code[0] = 0xFD;
    code[1] = 0x2A;

    code[3] = (uint8_t) (0x00FF & nn);
    code[2] = (uint8_t) (nn >> 8);

    return newInstruction(LD_IY_lINT16, code, 4);
}

Instruction Load_lInt16_HL(uint16_t nn) {
    uint8_t code[INS_LEN];
    code[0] = 0x22;
    code[2] = (uint8_t) (0x00FF & nn);
    code[1] = (uint8_t) (nn >> 8);

    return newInstruction(LD_lINT16_HL, code, 3);
}

Instruction Load_lInt16_Reg(uint16_t nn, reg_dd r) {
    uint8_t code[INS_LEN];
    code[0] = 0xED;
    code[1] = 0b01000011 | (r << 4);
    code[3] = (uint8_t) (0x00FF & nn);
    code[2] = (uint8_t) (nn >> 8);

    return newInstruction(LD_lINT16_REG, code, 4);
}

Instruction Load_lInt16_IX(uint16_t nn) {
    uint8_t code[INS_LEN];
    code[0] = 0xDD;
    code[1] = 0x22;

    code[3] = (uint8_t) (0x00FF & nn);
    code[2] = (uint8_t) (nn >> 8);

    return newInstruction(LD_lINT16_IX, code, 4);
}

Instruction Load_lInt16_IY(uint16_t nn) {
    uint8_t code[INS_LEN];
    code[0] = 0xFD;
    code[1] = 0x22;

    code[3] = (uint8_t) (0x00FF & nn);
    code[2] = (uint8_t) (nn >> 8);

    return newInstruction(LD_lINT16_IY, code, 4);
}

Instruction Load_SP_HL() {
    uint8_t code[INS_LEN];
    code[0] = 0xF9;

    return newInstruction(LD_SP_HL, code, 1);
}

Instruction Load_SP_IX() {
    uint8_t code[INS_LEN];
    code[0] = 0xDD;
    code[1] = 0xF9;

    return newInstruction(LD_SP_IX, code, 2);
}

Instruction Load_SP_IY() {
    uint8_t code[INS_LEN];
    code[0] = 0xFD;
    code[1] = 0xF9;

    return newInstruction(LD_SP_IY, code, 2);
}

Instruction Push_Reg(reg_qq r) {
    uint8_t code[INS_LEN];
    code[0] = 0b11000101 | (r << 4);

    return newInstruction(PUSH_REG, code, 1);
}

Instruction Push_IX() {
    uint8_t code[INS_LEN];
    code[0] = 0xDD;
    code[1] = 0xE5;

    return newInstruction(PUSH_IX, code, 2);
}

Instruction Push_IY() {
    uint8_t code[INS_LEN];
    code[0] = 0xFD;
    code[1] = 0xE5;

    return newInstruction(PUSH_IY, code, 2);
}

Instruction Pop_Reg(reg_qq r) {
    uint8_t code[INS_LEN];
    code[0] = 0b11000001 | (r << 4);

    return newInstruction(POP_REG, code, 1);
}

Instruction Pop_IX() {
    uint8_t code[INS_LEN];
    code[0] = 0xDD;
    code[1] = 0xE1;

    return newInstruction(POP_IX, code, 2);
}

Instruction Pop_IY() {
    uint8_t code[INS_LEN];
    code[0] = 0xFD;
    code[1] = 0xE1;

    return newInstruction(POP_IX, code, 2);
}

Instruction Exchange_DE_HL() {
    uint8_t code[INS_LEN];
    code[0] = 0xEB;
    return newInstruction(EX_DE_HL, code, 1);
}

Instruction Exchange_AF_AFALT() {
    uint8_t code[INS_LEN];
    code[0] = 0x08;
    return newInstruction(EX_AF_AFALT, code, 1);
} 

Instruction Exchange_Alternates() {
    uint8_t code[INS_LEN];
    code[0] = 0xD9;
    return newInstruction(EXX, code, 1);
}

Instruction Exchange_lSP_HL() {
    uint8_t code[INS_LEN];
    code[0] = 0xE3;
    return newInstruction(EX_lSP_HL, code, 1);
}

Instruction Exchange_lSP_IX() {
    uint8_t code[INS_LEN];
    code[0] = 0xDD;
    code[1] = 0xE3;
    return newInstruction(EX_lSP_IX, code, 2);
}

Instruction Exchange_lSP_IY() {
    uint8_t code[INS_LEN];
    code[0] = 0xFD;
    code[1] = 0xE3;
    return newInstruction(EX_lSP_IY, code, 2);
}

Instruction Load_Decrement_Increase() {
    uint8_t code[INS_LEN];
    code[0] = 0xED;
    code[1] = 0xA0;
    return newInstruction(LDI, code, 2);
}

Instruction Load_Decrement_Increase_Repeat() {
    uint8_t code[INS_LEN];
    code[0] = 0xED;
    code[1] = 0xB0;
    return newInstruction(LDIR, code, 2);
}

Instruction Load_Decrement_Decrement() {
    uint8_t code[INS_LEN];
    code[0] = 0xED;
    code[1] = 0xA8;
    return newInstruction(LDD, code, 2);
}         
Instruction Load_Decrement_Decrement_Repeat() {
    uint8_t code[INS_LEN];
    code[0] = 0xED;
    code[1] = 0xB8;
    return newInstruction(LDDR, code, 2);
}

Instruction Compare_Increase() {
    uint8_t code[INS_LEN];
    code[0] = 0xED;
    code[1] = 0xA1;
    return newInstruction(CPI, code, 2);
}

Instruction Compare_Increase_Repeat() {
    uint8_t code[INS_LEN];
    code[0] = 0xED;
    code[1] = 0xB1;
    return newInstruction(CPIR, code, 2);
}

Instruction Compare_Decrease() {
    uint8_t code[INS_LEN];
    code[0] = 0xED;
    code[1] = 0xA9;
    return newInstruction(CPD, code, 2);
}

Instruction Compare_Decrease_Repeat() {
    uint8_t code[INS_LEN];
    code[0] = 0xED;
    code[1] = 0xB9;
    return newInstruction(CPDR, code, 2);
}

Instruction Add_A_Reg(reg_r r) {
    uint8_t code[INS_LEN];
    code[0] = 0b10000000 | r;
    return newInstruction(ADD_A_REG, code, 1);
}

Instruction Add_A_Int8(uint8_t n) {
    uint8_t code[INS_LEN];
    code[0] = 0xC6;
    code[1] = n;
    return newInstruction(ADD_A_INT8, code, 2);
}

Instruction Add_A_lHL() {
    uint8_t code[INS_LEN];
    code[0] = 0x86;
    return newInstruction(ADD_A_lHL, code, 1);
}

Instruction Add_A_lIXd(uint8_t d) {
    uint8_t code[INS_LEN];
    code[0] = 0xDD;
    code[1] = 0x86;
    code[2] = d;
    return newInstruction(ADD_A_lIXd, code, 3);
}

Instruction Add_A_lIYd(uint8_t d) {
    uint8_t code[INS_LEN];
    code[0] = 0xFD;
    code[1] = 0x86;
    code[2] = d;
    return newInstruction(ADD_A_lIYd, code, 3);
}

Instruction Add_CY_A_Reg(reg_r r) {
    uint8_t code[INS_LEN];
    code[0] = 0b10001000 | r;
    return newInstruction(ADDC_A_REG, code, 1);
}

Instruction Add_CY_A_Int8(uint8_t n) {
    uint8_t code[INS_LEN];
    code[0] = 0xCE;
    code[1] = n;
    return newInstruction(ADDC_A_INT8, code, 2);
}

Instruction Add_CY_A_lHL() {
    uint8_t code[INS_LEN];
    code[0] = 0x8E;
    return newInstruction(ADDC_A_lHL, code, 1);
}

Instruction Add_CY_A_lIXd(uint8_t d) {
    uint8_t code[INS_LEN];
    code[0] = 0xDD;
    code[1] = 0x8E;
    code[2] = d;
    return newInstruction(ADDC_A_lIXd, code, 3);
}

Instruction Add_CY_A_lIYd(uint8_t d) {
    uint8_t code[INS_LEN];
    code[0] = 0xFD;
    code[1] = 0x8E;
    code[2] = d;
    return newInstruction(ADDC_A_lIYd, code, 3);
}

Instruction Sub_A_Reg(reg_r r) {
    uint8_t code[INS_LEN];
    code[0] = 0b10010000 | r;
    return newInstruction(SUB_A_REG, code, 1);
}

Instruction Sub_A_Int8(uint8_t n) {
    uint8_t code[INS_LEN];
    code[0] = 0xD6;
    code[1] = n;
    return newInstruction(SUB_A_INT8, code, 2);
}

Instruction Sub_A_lHL() {
    uint8_t code[INS_LEN];
    code[0] = 0x96;
    return newInstruction(SUB_A_lHL, code, 1);
}

Instruction Sub_A_lIXd(uint8_t d) {
    uint8_t code[INS_LEN];
    code[0] = 0xDD;
    code[1] = 0x96;
    code[2] = d;
    return newInstruction(SUB_A_lIXd, code, 3);
}

Instruction Sub_A_lIYd(uint8_t d) {
    uint8_t code[INS_LEN];
    code[0] = 0xFD;
    code[1] = 0x96;
    code[2] = d;
    return newInstruction(SUB_A_lIYd, code, 3);
}

Instruction Sub_CY_A_Reg(reg_r r) {
    uint8_t code[INS_LEN];
    code[0] = 0b10011000 | r;
    return newInstruction(SUBC_A_REG, code, 1);
}

Instruction Sub_CY_A_Int8(uint8_t n) {
    uint8_t code[INS_LEN];
    code[0] = 0xDE;
    code[1] = n;
    return newInstruction(SUBC_A_INT8, code, 2);
}

Instruction Sub_CY_A_lHL() {
    uint8_t code[INS_LEN];
    code[0] = 0x9E;
    return newInstruction(SUBC_A_lHL, code, 1);
}

Instruction Sub_CY_A_lIXd(uint8_t d) {
    uint8_t code[INS_LEN];
    code[0] = 0xDD;
    code[1] = 0x9E;
    code[2] = d;
    return newInstruction(SUBC_A_lIXd, code, 3);
}

Instruction Sub_CY_A_lIYd(uint8_t d) {
    uint8_t code[INS_LEN];
    code[0] = 0xFD;
    code[1] = 0x9E;
    code[2] = d;
    return newInstruction(SUBC_A_lIYd, code, 3);
}

Instruction And_A_Reg(reg_r r) {
    uint8_t code[INS_LEN];
    code[0] = 0b10100000 | r;
    return newInstruction(AND_A_REG, code, 1);
}

Instruction And_A_Int8(uint8_t n) {
    uint8_t code[INS_LEN];
    code[0] = 0xE6;
    code[1] = n;
    return newInstruction(AND_A_INT8, code, 2);
}

Instruction And_A_lHL() {
    uint8_t code[INS_LEN];
    code[0] = 0xA6;
    return newInstruction(AND_A_lHL, code, 1);
}

Instruction And_A_lIXd(uint8_t d) {
    uint8_t code[INS_LEN];
    code[0] = 0xDD;
    code[1] = 0xA6;
    code[2] = d;
    return newInstruction(AND_A_lIXd, code, 3);
}

Instruction And_A_lIYd(uint8_t d) {
    uint8_t code[INS_LEN];
    code[0] = 0xFD;
    code[1] = 0xA6;
    code[2] = d;
    return newInstruction(AND_A_lIYd, code, 3);
}

Instruction Or_A_Reg(reg_r r) {
    uint8_t code[INS_LEN];
    code[0] = 0b10110000 | r;
    return newInstruction(OR_A_REG, code, 1);
}

Instruction Or_A_Int8(uint8_t n) {
    uint8_t code[INS_LEN];
    code[0] = 0xF6;
    code[1] = n;
    return newInstruction(OR_A_INT8, code, 2);
}

Instruction Or_A_lHL() {
    uint8_t code[INS_LEN];
    code[0] = 0xB6;
    return newInstruction(OR_A_lHL, code, 1);
}

Instruction Or_A_lIXd(uint8_t d) {
    uint8_t code[INS_LEN];
    code[0] = 0xDD;
    code[1] = 0xB6;
    code[2] = d;
    return newInstruction(OR_A_lIXd, code, 3);
}

Instruction Or_A_lIYd(uint8_t d) {
    uint8_t code[INS_LEN];
    code[0] = 0xFD;
    code[1] = 0xB6;
    code[2] = d;
    return newInstruction(OR_A_lIYd, code, 3);
}

Instruction Xor_A_Reg(reg_r r) {
    uint8_t code[INS_LEN];
    code[0] = 0b10100000 | r;
    return newInstruction(XOR_A_REG, code, 1);
}

Instruction Xor_A_Int8(uint8_t n) {
    uint8_t code[INS_LEN];
    code[0] = 0xEE;
    code[1] = n;
    return newInstruction(XOR_A_INT8, code, 2);
}

Instruction Xor_A_lHL() {
    uint8_t code[INS_LEN];
    code[0] = 0xAE;
    return newInstruction(XOR_A_lHL, code, 1);
}

Instruction Xor_A_lIXd(uint8_t d) {
    uint8_t code[INS_LEN];
    code[0] = 0xDD;
    code[1] = 0xAE;
    code[2] = d;
    return newInstruction(XOR_A_lIXd, code, 3);
}

Instruction Xor_A_lIYd(uint8_t d) {
    uint8_t code[INS_LEN];
    code[0] = 0xFD;
    code[1] = 0xAE;
    code[2] = d;
    return newInstruction(XOR_A_lIYd, code, 3);
}

Instruction Cp_A_Reg(reg_r r) {
    uint8_t code[INS_LEN];
    code[0] = 0b10111000 | r;
    return newInstruction(CP_A_REG, code, 1);
}

Instruction Cp_A_Int8(uint8_t n) {
    uint8_t code[INS_LEN];
    code[0] = 0xFE;
    code[1] = n;
    return newInstruction(CP_A_INT8, code, 2);
}

Instruction Cp_A_lHL() {
    uint8_t code[INS_LEN];
    code[0] = 0xBE;
    return newInstruction(CP_A_lHL, code, 1);
}

Instruction Cp_A_lIXd(uint8_t d) {
    uint8_t code[INS_LEN];
    code[0] = 0xDD;
    code[1] = 0xBE;
    code[2] = d;
    return newInstruction(CP_A_lIXd, code, 3);
}

Instruction Cp_A_lIYd(uint8_t d) {
    uint8_t code[INS_LEN];
    code[0] = 0xFD;
    code[1] = 0xBE;
    code[2] = d;
    return newInstruction(CP_A_lIYd, code, 3);
}

Instruction Inc_Reg(reg_r r) {
    uint8_t code[INS_LEN];
    code[0] = 0b00000100 | (r << 3);
    return newInstruction(INC_REG, code, 1);
}

Instruction Inc_lHL() {
    uint8_t code[INS_LEN];
    code[0] = 0x34;
    return newInstruction(INC_lHL, code, 1);
}

Instruction Inc_lIXd(uint8_t d) {
    uint8_t code[INS_LEN];
    code[0] = 0xDD;
    code[1] = 0x34;
    code[2] = d;
    return newInstruction(INC_lIXd, code, 3);
}

Instruction Inc_lIYd(uint8_t d) {
    uint8_t code[INS_LEN];
    code[0] = 0xFD;
    code[1] = 0x34;
    code[2] = d;
    return newInstruction(INC_lIYd, code, 3);
}

Instruction Dec_Reg(reg_r r) {
    uint8_t code[INS_LEN];
    code[0] = 0b00000101 | (r << 3);
    return newInstruction(DEC_REG, code, 1);
}

Instruction Dec_lHL() {
    uint8_t code[INS_LEN];
    code[0] = 0x35;
    return newInstruction(DEC_lHL, code, 1);
}

Instruction Dec_lIXd(uint8_t d) {
    uint8_t code[INS_LEN];
    code[0] = 0xDD;
    code[1] = 0x35;
    code[2] = d;
    return newInstruction(DEC_lIXd, code, 3);
}

Instruction Dec_lIYd(uint8_t d) {
    uint8_t code[INS_LEN];
    code[0] = 0xFD;
    code[1] = 0x35;
    code[2] = d;
    return newInstruction(DEC_lIYd, code, 3);
}

Instruction Decimal_Adjust() {
    uint8_t code[INS_LEN];
    code[0] = 0x27;
    return newInstruction(DAA, code, 1);
}

Instruction Complement_A() {
    uint8_t code[INS_LEN];
    code[0] = 0x2F;
    return newInstruction(CPL, code, 1);
}

Instruction Negate() {
    uint8_t code[INS_LEN];
    code[0] = 0xED;
    code[1] = 0x44;
    return newInstruction(NEG, code, 2);
}

Instruction Complement_CF() {
    uint8_t code[INS_LEN];
    code[0] = 0x3F;
    return newInstruction(CCF, code, 1);
}

Instruction Set_Carry_Flag() {
    uint8_t code[INS_LEN];
    code[0] = 0x37;
    return newInstruction(SCF, code, 1);
}

Instruction Nop() {
    uint8_t code[INS_LEN];
    code[0] = 0x00;
    return newInstruction(NOP, code, 1);
}

Instruction Halt() {
    uint8_t code[INS_LEN];
    code[0] = 0x76;
    return newInstruction(HALT, code, 1);
}

Instruction Disable_Interrupts() {
    uint8_t code[INS_LEN];
    code[0] = 0xF3;
    return newInstruction(DI, code, 1);
}

Instruction Enable_Interrupts() {
    uint8_t code[INS_LEN];
    code[0] = 0xFB;
    return newInstruction(EI, code, 1);
}

Instruction Interrupt_Mode0() {
    uint8_t code[INS_LEN];
    code[0] = 0xED;
    code[1] = 0x46;
    return newInstruction(IM0, code, 2);
}

Instruction Interrupt_Mode1() {
    uint8_t code[INS_LEN];
    code[0] = 0xED;
    code[1] = 0x56;
    return newInstruction(IM1, code, 2);
}

Instruction Interrupt_Mode2() {
    uint8_t code[INS_LEN];
    code[0] = 0xED;
    code[1] = 0x5E;
    return newInstruction(IM2, code, 2);
}
