#ifndef DIS_H
#define DIS_H

#include "generation.hpp"
#include <vector>
#include <functional>
#include <map>
using namespace std;

//Passed into AddInstructions() to indicate
//that the content of certain bytes does not matter
//when matching an instruction
#define NO_NEUTRALS 0b0000
#define FIRST_NEUTRAL 0b0001
#define SECOND_NEUTRAL 0b0010
#define THIRD_NEUTRAL 0b0100
#define FOURTH_NEUTRAL 0b1000

class BitTree {
private:
    struct Node {
        Node* OneNode;
        Node* ZeroNode;
        Node* NeutralNode; 
        bool hasInstruction;
        Instruction instruction;
    };
    struct Node* newNode();
    void printNode(struct Node* n);

    struct Node* tree;
    void PostOrder(struct Node* n, function<void(struct Node*)> f);
public:
    BitTree();
    ~BitTree();
    void AddInstruction(Instruction i, uint8_t neutral_bytes);
    vector<Instruction> Disassemble(vector<uint8_t> code);
    void printTree();
};

class Z80_Disassembler {
public:
    string stringifyInstruction(Instruction n);
    vector<Instruction> Disassemble(vector<uint8_t> code);
    Z80_Disassembler();

    vector<reg_r> rregs;
    map<reg_r, string> stringify_rreg;

    vector<reg_dd> ddregs;
    map<reg_dd, string> stringify_ddregs;

    vector<reg_qq> qqregs;
    map<reg_qq, string> stringify_qqregs;

    vector<reg_pp> ppregs;
    map<reg_pp, string> stringify_ppregs;

    vector<reg_rr> rrregs;
    map<reg_rr, string> stringify_rrregs;

private:
    map<Instruction_Tag, function<string(Instruction n)>> stringificationFns;
    BitTree bt;
};

#endif