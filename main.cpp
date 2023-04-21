#include "generation.hpp"
#include "disassemble.hpp"
#include <iostream>
#include <vector>
#include <fstream>
using namespace std;

void assert(Instruction result, Instruction expected, string test_name) {
    if (!match(expected, result)) {
        cout << "Test failure '" << test_name << "' ";
        cout << "outputs don't match:\n";
        cout << "Expected: ";
        printInstruction(expected);
        cout << "Result: ";
        printInstruction(result);
        exit(1);
    } else {
        cout << test_name << " passed\n";
    }
}



void test_generation() {
    uint8_t code[INS_LEN];

    code[0] = 0b01111000;
    assert(Load_Reg_Reg(A_R, B_R), newInstruction(LD_REG_REG, code, 1), "ld r, r'");

    code[0] = 0b00111110;
    code[1] = 0xFF;
    assert(Load_Reg_Int8(A_R, 0xFF), newInstruction(LD_REG_INT8, code, 2), "ld r, imm"); 

    code[0] = 0x3A;
    code[1] = 0xFF;
    code[2] = 0xEE;
    assert(Load_A_lInt16(0xFFEE), newInstruction(LD_A_lINT16, code, 3), "ld a, (nn)");
}

void test_diss() {

    vector<uint8_t> someCode = {
        0x3A, 0xBB, 0xFF,       //ld a, (0xFFBB)
        0b01111101,             //ld a, l
        0b00111110, 0xFF,       //ld a, 0xFF
        0b01101110,             //ld l, (hl)
        0xDD, 0b01111110, 0xFF, //ld a, (IX - 0x7F)
        0xFD, 0b01111110, 0xFE, //ld a, (IY - 0x7E)
        0xDD, 0b01110111, 0x1A, //ld (IX + 0x1A), a
        0xFD, 0b01110111, 0x1E, //ld (IY + 0x1E), a
        0x36, 0xFE,             //ld (hl), 0xFE
        0xDD, 0x36, 0x1A, 0xFE, //ld, (IX + 0x1A), 0xFE
        0xFD, 0x36, 0x1A, 0xFE, //ld, (IY + 0x1A), 0xFE
        0x1A, 0x0A, 0x02, 0x12, //ld a, (de); ld a, (bc); ld (de), a; ld (bc), a
        0x3A, 0xAE, 0x1F,       //ld a, (0x1FAE)
        0x32, 0xAE, 0xF1,       //ld (0xF1EA), a
        0xED, 0x57,             //ld a, i
        0xED, 0x5F,             //ld a, r
        0xED, 0x47,             //ld i, a
        0xED, 0x4F,             //ld r, a
        0b00110001, 0xFA, 0xB1,   //ld sp, 0xB1FA
        0xDD, 0x21, 0xAD, 0xDE, //ld ix, 0xDEAD
        0xFD, 0x21, 0xEF, 0xBE, //ld iy, 0xBEEF
        0x2A, 0xAF, 0xFA,       //ld hl, (0xFAAF)
        0xED, 0b01111011, 0xAF, 0xFA, //ld sp, (0xFAAF)
        0xDD, 0x2A, 0xAF, 0xFA, //ld ix, (0xFAAF)
        0xFD, 0x2A, 0xAF, 0xFA, //ld iy, (0xFAAF)
        0x22, 0xED, 0xDE, //ld (0xdeed), hl
        0xED, 0b01110011, 0xED, 0xDE, //ld (0xdeed), sp
        0xF9, 0xDD, 0xF9, 0xFD, 0xF9, //ld sp, hl; ld sp, ix; ld sp, iy
        0b11110101, //push af
        0xDD, 0xE5, 0xFD, 0xE5, 0xDD, 0xE1, 0xFD, 0xE1,
        0b11110001 // pop af
    };

    Z80_Disassembler dis;
    vector<Instruction> ins_vec = dis.Disassemble(someCode);
    for (Instruction i : ins_vec) {
        cout << dis.stringifyInstruction(i) << endl;
    }
}

void fromfile_test() {
    ifstream infile("./test_asm/ld8.rom");
    vector<uint8_t> rom;

    do {
        char n;
        infile.get(n);
        rom.push_back((uint8_t) n);
    } while (!infile.eof());
    rom.pop_back();

    Z80_Disassembler dis;
    vector<Instruction> ins_vec = dis.Disassemble(rom);
    for (Instruction i : ins_vec) {
        cout << dis.stringifyInstruction(i) << endl;
    }
}

void maxTime_Test() {
    vector<uint8_t> someCode;
    for (int i = 0; i < (64 * 1024); i++) {
        someCode.push_back(0b01101110);
    }

    cout << someCode.size() << endl;

    Z80_Disassembler dis;
    vector<Instruction> ins_vec = dis.Disassemble(someCode);
}

int main() {
    test_diss();
    return 0;
}