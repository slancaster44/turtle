#include "generation.hpp"
#include "disassemble.hpp"
#include "lexer.hpp"
#include <iostream>
#include <vector>
#include <fstream>
#include <map>
#include <string.h>
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

void testGen() {
    map<Instruction_Tag, Instruction> testMap1;
    map<Instruction_Tag, uint8_t[INS_LEN]> testMap2;

    testMap1[LD_REG_REG] = Load_Reg_Reg(A_R, B_R);
    testMap2[LD_REG_REG][0] = 0b01111000;

   /* testMap1[LD_REG_INT8] = Load_Reg_Int8(C_R, 0);
    testMap1[LD_REG_lHL] = Load_Reg_lHL(D_R);
    testMap1[LD_REG_lIXd] = Load_Reg_lIXd(E_R, 0);
    testMap1[LD_REG_lIYd] = Load_Reg_lIYd(H_R, 0);

    testMap1[LD_lHL_REG] = Load_lHL_Reg(L_R);
    testMap1[LD_lIXd_REG] = Load_lIXd_Reg(0, A_R);
    testMap1[LD_lIYd_REG] = Load_lIYd_Reg(0, B_R);*/

    for (auto const& [key, value] : testMap1) {
        if (value.tag != key) {
            cout << "Tag did not match key: " << key << " ";
            printInstruction(value);
        }

        uint8_t* expectedCode = testMap2[key];
        if (memcmp(expectedCode, value.code, value.code_len) != 0) {
            cout << "Code did not match expected output ";
            printInstruction(value);
        }
    }

    cout << "Code generation tests pass" << endl;
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
    Z80ASM_Lexer l("file", "ld a, 'h'");

    while (!l.isDone()) {
        Token t = l.genToken();
        cout << t.text << " " << t.tag << endl;
    }

    return 0;
}