#include "disassemble.hpp"
#include <functional>
#include <iostream>
#include <cstring>
#include <sstream>
using namespace std;

#define INS_SIZE 4

#define UINT8_TO_SINT(VAL) \
    ((VAL > 0b10000000) ? (0x80 - (VAL & 0b01111111)) : (int) VAL)

BitTree::BitTree() {
    BitTree::tree = newNode();
}

BitTree::~BitTree() {
    PostOrder(BitTree::tree, free);
}

struct BitTree::Node* BitTree::newNode() {
    struct Node* retVal = new struct Node;
    retVal->OneNode = nullptr;
    retVal->ZeroNode = nullptr;
    retVal->NeutralNode = nullptr;
    retVal->hasInstruction = false;

    return retVal;
}

void BitTree::PostOrder(struct Node* n, function<void(struct Node*)> f) {
    if (n != nullptr) {
        PostOrder(n->OneNode, f);
        PostOrder(n->NeutralNode, f);
        PostOrder(n->ZeroNode, f);
        f(n);
    }
}

void BitTree::AddInstruction(Instruction ins, uint8_t neutral_bytes) {
    uint32_t val = 0;
    
    for (int i = 0; i < ins.code_len; i++) {
        val = (val << 8) | ins.code[i];
    }

    struct Node* curNode = BitTree::tree;
    for (int bit = (ins.code_len * 8)-1; bit >= 0; bit--) {

        int curByte = (ins.code_len - (bit / 8))-1;
        bool isNeutralByte = neutral_bytes & (1 << curByte);
        int bit_value = ((1 << bit) & val) > 0 ;

        if (isNeutralByte && (curNode->NeutralNode == nullptr)) {
            curNode->NeutralNode = newNode();
            curNode = curNode->NeutralNode;
        
        } else if (isNeutralByte) {
            curNode = curNode->NeutralNode;

        } else if (bit_value && (curNode->OneNode == nullptr)) {
            curNode->OneNode = newNode();
            curNode = curNode->OneNode;
        
        } else if (bit_value && (curNode->OneNode != nullptr)) {
            curNode = curNode->OneNode;

        } else if (!bit_value && (curNode->ZeroNode == nullptr)) {
            curNode->ZeroNode = newNode();
            curNode = curNode->ZeroNode;

        } else if (!bit_value && (curNode->ZeroNode != nullptr)) {
            curNode = curNode->ZeroNode;
        }
    }

    curNode->hasInstruction = true;
    curNode->instruction = ins;
}

vector<Instruction> BitTree::Disassemble(vector<uint8_t> code) {
    vector<Instruction> retVal;
    struct Node* curNode = BitTree::tree;
    uint8_t curInsValue[INS_SIZE];
    int curInsIndex = 0;


    for (int i = 0; i < code.size(); i++) {
        uint8_t curByte = code.at(i);
        curInsValue[curInsIndex] = curByte;

        for (int bit = 7; bit >= 0; bit--) {
            int bitValue = (curByte & (1 << bit)) > 0;

            if (curNode->NeutralNode != nullptr) {
                curNode = curNode->NeutralNode;
            } else if (bitValue && (curNode->OneNode != nullptr)) {
                curNode = curNode->OneNode;
            } else if ((!bitValue) && curNode->ZeroNode != nullptr) {
                curNode = curNode->ZeroNode;
            } else {
                cerr << "Invalid instruction" << endl;
                cerr << bit << ", " << bitValue << endl;
                cerr << hex << (int) curByte << endl;
                printNode(curNode);
                exit(1);
            }
        }
        curInsIndex++;

        if (curNode->hasInstruction) {
            memcpy(curNode->instruction.code, curInsValue, INS_SIZE);
            retVal.push_back(curNode->instruction);
            curNode = BitTree::tree;

            curInsIndex = 0;
            memset(curInsValue, 0, INS_SIZE);
        }
    }

    return retVal;
}

void BitTree::printNode(struct Node* n) {
        cout << "Node: " << n << endl;
        cout << "Has Instruction: " << n->hasInstruction << endl;
        cout << "Has One Node: " << n->OneNode<< endl;
        cout << "Has Zero Node: " << n->ZeroNode << endl;
        cout << "Has Neutral Node: " << n->NeutralNode  << endl;
}

void BitTree::printTree() {
    PostOrder(tree, [&](struct Node* n) {
        printNode(n);
    });
}

string Z80_Disassembler::stringifyInstruction(Instruction n) {
    auto fn = stringificationFns[n.tag];
    return fn(n);
}

vector<Instruction> Z80_Disassembler::Disassemble(vector<uint8_t> code) {
    return bt.Disassemble(code);
}

Z80_Disassembler::Z80_Disassembler() {
    rregs = {A_R, B_R, C_R, D_R, E_R, H_R, L_R};
    stringify_rreg[A_R] = "a";
    stringify_rreg[B_R] = "b";
    stringify_rreg[C_R] = "c";
    stringify_rreg[D_R] = "d";
    stringify_rreg[E_R] = "e";
    stringify_rreg[H_R] = "h";
    stringify_rreg[L_R] = "l";


    bt.AddInstruction(Load_A_lInt16(0), SECOND_NEUTRAL | THIRD_NEUTRAL);
    bt.AddInstruction(Load_lHL_Int8(0), SECOND_NEUTRAL);
    bt.AddInstruction(Load_lIXd_Int8(0, 0), THIRD_NEUTRAL | FOURTH_NEUTRAL);
    bt.AddInstruction(Load_lIYd_Int8(0, 0), THIRD_NEUTRAL | FOURTH_NEUTRAL);
    bt.AddInstruction(Load_A_lBC(), NO_NEUTRALS);
    bt.AddInstruction(Load_A_lDE(), NO_NEUTRALS);
    bt.AddInstruction(Load_lBC_A(), NO_NEUTRALS);
    bt.AddInstruction(Load_lDE_A(), NO_NEUTRALS);
    bt.AddInstruction(Load_lInt16_A(0), SECOND_NEUTRAL | THIRD_NEUTRAL);
    bt.AddInstruction(Load_A_I(), NO_NEUTRALS);
    bt.AddInstruction(Load_A_R(), NO_NEUTRALS);
    bt.AddInstruction(Load_R_A(), NO_NEUTRALS);
    bt.AddInstruction(Load_I_A(), NO_NEUTRALS);

    
    for (reg_r r1 : rregs) {
        bt.AddInstruction(Load_Reg_Int8(r1, 0), SECOND_NEUTRAL);
        bt.AddInstruction(Load_Reg_lHL(r1), NO_NEUTRALS);
        bt.AddInstruction(Load_Reg_lIXd(r1, 0), THIRD_NEUTRAL);
        bt.AddInstruction(Load_Reg_lIYd(r1, 0), THIRD_NEUTRAL);
        bt.AddInstruction(Load_lHL_Reg(r1), NO_NEUTRALS);
        bt.AddInstruction(Load_lIXd_Reg(0, r1), THIRD_NEUTRAL);
        bt.AddInstruction(Load_lIYd_Reg(0, r1), THIRD_NEUTRAL);

        for (reg_r r2 : rregs) {
            bt.AddInstruction(Load_Reg_Reg(r1, r2), NO_NEUTRALS);
        }
    }


    stringificationFns[LD_A_lINT16] = [](Instruction n) -> string {
        stringstream ss;
        ss << "ld a, (0x";
        ss << hex << ((int) n.code[2]) << ((int) n.code[1]);
        ss << ")";
        return ss.str();
    };

    stringificationFns[LD_REG_REG] = [&](Instruction n) -> string {
        stringstream ss;
        ss << "ld ";
        ss << stringify_rreg[((0b00111000 & n.code[0]) >> 3)];
        ss << ", ";
        ss << stringify_rreg[(0b00000111 & n.code[0])];
        return ss.str();
    };

    stringificationFns[LD_REG_INT8] = [&](Instruction n) -> string {
        stringstream ss;
        ss << "ld ";
        ss << stringify_rreg[((0b00111000 & n.code[0]) >> 3)];
        ss << ", 0x";
        ss << hex << (int) n.code[1];
        return ss.str();
    };

    stringificationFns[LD_REG_lHL] = [&](Instruction n) -> string {
        stringstream ss;
        ss << "ld ";
        ss << stringify_rreg[((0b00111000 & n.code[0]) >> 3)];
        ss << ", (hl)";
        return ss.str();
    };

    stringificationFns[LD_REG_lIXd] = [&](Instruction n) -> string {
        stringstream ss;
        ss << "ld ";
        ss << stringify_rreg[((0b00111000 & n.code[1]) >> 3)];
        ss << ", (IX ";
        int val = UINT8_TO_SINT(n.code[2]);
        ss << ((n.code[2] & 0b10000000) ? "-" : "+");
        ss << " 0x";
        ss << hex << val;
        ss << ")";
        return ss.str();
    };

    stringificationFns[LD_REG_lIYd] = [&](Instruction n) -> string {
        stringstream ss;
        ss << "ld ";
        ss << stringify_rreg[((0b00111000 & n.code[1]) >> 3)];
        ss << ", (IY ";
        int val = UINT8_TO_SINT(n.code[2]);
        ss << ((n.code[2] & 0b10000000) ? "-" : "+");
        ss << " 0x";
        ss << hex << val;
        ss << ")";
        return ss.str();
    };

    stringificationFns[LD_lHL_REG] = [&](Instruction n) -> string {
        stringstream ss;
        ss << "ld (hl), ";
        ss << stringify_rreg[(0b00000111 & n.code[0])];
        return ss.str();
    };

    stringificationFns[LD_lIXd_REG] = [&](Instruction n) -> string {
        stringstream ss;
        ss << "ld (IX ";
        int val = UINT8_TO_SINT(n.code[2]);
        ss << ((n.code[2] & 0b10000000) ? "-" : "+");
        ss << " 0x";
        ss << hex << val;
        ss << "), ";
        ss << stringify_rreg[(0b00000111 & n.code[1])];
        return ss.str();
    };

    stringificationFns[LD_lIYd_REG] = [&](Instruction n) -> string {
        stringstream ss;
        ss << "ld (IY ";
        int val = UINT8_TO_SINT(n.code[2]);
        ss << ((n.code[2] & 0b10000000) ? "-" : "+");
        ss << " 0x";
        ss << hex << val;
        ss << "), ";
        ss << stringify_rreg[(0b00000111 & n.code[1])];
        return ss.str();
    };

    stringificationFns[LD_lHL_INT8] = [](Instruction n) -> string {
        stringstream ss;
        ss << "ld (hl), 0x";
        ss << hex << (int) n.code[1];
        return ss.str();
    };

    stringificationFns[LD_lIXd_INT8] = [](Instruction n) -> string {
        stringstream ss;
        ss << "ld (IX ";
        int val = UINT8_TO_SINT(n.code[2]);
        ss << ((n.code[2] & 0b10000000) ? "-" : "+");
        ss << " 0x";
        ss << hex << val;
        ss << "), 0x";
        ss << hex << (int) n.code[3];
        return ss.str();
    };

    stringificationFns[LD_lIYd_INT8] = [](Instruction n) -> string {
        stringstream ss;
        ss << "ld (IY ";
        int val = UINT8_TO_SINT(n.code[2]);
        ss << ((n.code[2] & 0b10000000) ? "-" : "+");
        ss << " 0x";
        ss << hex << val;
        ss << "), 0x";
        ss << hex << (int) n.code[3];
        return ss.str();
    };

    stringificationFns[LD_A_lBC] = [](Instruction n) -> string {
        return "ld a, (bc)";
    };

    stringificationFns[LD_A_lDE] = [](Instruction n) -> string {
        return "ld a, (de)";
    };

    stringificationFns[LD_lBC_A] = [](Instruction n) -> string {
        return "ld (bc), a";
    };

    stringificationFns[LD_lDE_A] = [](Instruction n) -> string {
        return "ld (de), a";
    };

    stringificationFns[LD_A_lINT16] = [](Instruction n) -> string {
        stringstream ss;
        ss << "ld a, (0x";
        ss << hex << (int) n.code[2];
        ss << hex << ((int) n.code[1]);
        ss << ")";
        return ss.str();
    };

    stringificationFns[LD_lINT16_A] = [](Instruction n) -> string {
        stringstream ss;
        ss << "ld (0x";
        ss << hex << (int) n.code[2];
        ss << hex << ((int) n.code[1]);
        ss << "), a";
        return ss.str();
    };

    stringificationFns[LD_A_R] = [](Instruction n) -> string {
        return "ld a, r";
    };

    stringificationFns[LD_A_I] = [](Instruction n) -> string {
        return "ld a, i";
    };

    stringificationFns[LD_R_A] = [](Instruction n) -> string {
        return "ld i, a";
    };

    stringificationFns[LD_I_A] = [](Instruction n) -> string {
        return "ld r, a";
    };
}

