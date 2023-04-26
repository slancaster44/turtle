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
        0b11110001, // pop af
        0xEB, 0x08, 0xD9, 0xE3, 0xDD, 0xE3, 0xFD, 0xE3, //exchange instructions
        0xED, 0xA0, 0xED, 0xB0, 0xED, 0xA8, 0xED, 0xB8, //ldi(r) & ldd(r)
        0xED, 0xA1, 0xED, 0xB1, 0xED, 0xA9, 0xED, 0xB9, //cpi(r) & cpd(r)
        0b10000111, // add a, a
        0xC6, 0xFF, //add a, 0xFF
        0x86, //add a, (hl)
        0xDD, 0x86, 0xC1, //add a, (IX - 0x41)
        0xFD, 0x86, 0xC1, //add a, (IY - 0x41)
        0b10001111, //adc a, a
        0xCE, 0xFE, //adc a, 0xFE
        0x8E, //adc a, (hl)
        0xDD, 0x8E, 0xC1,
        0xFD, 0x8E, 0xC1,
        0b10100111, // and a
        0xE6, 0xFF, // and 0xFF
        0xA6, //and a, (hl)
        0xDD, 0xA6, 0x04,
        0xFD, 0xA6, 0x04,
        0b00111101, //dec a
        0x35, //dec (hl)
        0xDD, 0x35, 0x04, //dec (ix + 4)
        0xFD, 0x35, 0x05, //dec (ix + 5)
        0b00111100, //inc a
        0x34, //inc (hl)
        0xDD, 0x34, 0x04, //inc (ix + 4)
        0xFD, 0x34, 0x05, //inc (ix + 5)
        0b00011001,
        0xED, 0b01011010,
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