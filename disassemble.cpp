#include "disassemble.hpp"
#include <functional>
#include <iostream>
#include <cstring>
#include <sstream>
using namespace std;


#define UINT8_TO_SINT(VAL) \
    ((VAL > 0b10000000) ? (VAL - 0x80) : (int) VAL)

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
    uint8_t curInsValue[INS_LEN];
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
            memcpy(curNode->instruction.code, curInsValue, INS_LEN);
            retVal.push_back(curNode->instruction);
            curNode = BitTree::tree;

            curInsIndex = 0;
            memset(curInsValue, 0, INS_LEN);
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
    qqregs = {BC_QQ, DE_QQ, HL_QQ, AF_QQ};
    ddregs = {BC_DD, HL_DD, DE_DD, SP_DD};

    stringify_rreg[A_R] = "a";
    stringify_rreg[B_R] = "b";
    stringify_rreg[C_R] = "c";
    stringify_rreg[D_R] = "d";
    stringify_rreg[E_R] = "e";
    stringify_rreg[H_R] = "h";
    stringify_rreg[L_R] = "l";

    stringify_ddregs[BC_DD] = "bc";
    stringify_ddregs[DE_DD] = "de";
    stringify_ddregs[HL_DD] = "hl";
    stringify_ddregs[SP_DD] = "sp";

    stringify_qqregs[BC_QQ] = "bc";
    stringify_qqregs[DE_QQ] = "de";
    stringify_qqregs[HL_QQ] = "hl";
    stringify_qqregs[AF_QQ] = "af";

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

    bt.AddInstruction(Add_A_Int8(0), SECOND_NEUTRAL);
    bt.AddInstruction(Add_A_lHL(), NO_NEUTRALS);
    bt.AddInstruction(Add_A_lIXd(0), THIRD_NEUTRAL);
    bt.AddInstruction(Add_A_lIYd(0), THIRD_NEUTRAL);
    bt.AddInstruction(Add_CY_A_Int8(0), SECOND_NEUTRAL);
    bt.AddInstruction(Add_CY_A_lHL(), NO_NEUTRALS);
    bt.AddInstruction(Add_CY_A_lIXd(0), THIRD_NEUTRAL);
    bt.AddInstruction(Add_CY_A_lIYd(0), THIRD_NEUTRAL);

    bt.AddInstruction(Sub_A_Int8(0), SECOND_NEUTRAL);
    bt.AddInstruction(Sub_A_lHL(), NO_NEUTRALS);
    bt.AddInstruction(Sub_A_lIXd(0), THIRD_NEUTRAL);
    bt.AddInstruction(Sub_A_lIYd(0), THIRD_NEUTRAL);
    bt.AddInstruction(Sub_CY_A_Int8(0), SECOND_NEUTRAL);
    bt.AddInstruction(Sub_CY_A_lHL(), NO_NEUTRALS);
    bt.AddInstruction(Sub_CY_A_lIXd(0), THIRD_NEUTRAL);
    bt.AddInstruction(Sub_CY_A_lIYd(0), THIRD_NEUTRAL);

    bt.AddInstruction(And_A_Int8(0), SECOND_NEUTRAL);
    bt.AddInstruction(And_A_lHL(), NO_NEUTRALS);
    bt.AddInstruction(And_A_lIXd(0), THIRD_NEUTRAL);
    bt.AddInstruction(And_A_lIYd(0), THIRD_NEUTRAL);

    bt.AddInstruction(Or_A_Int8(0), SECOND_NEUTRAL);
    bt.AddInstruction(Or_A_lHL(), NO_NEUTRALS);
    bt.AddInstruction(Or_A_lIXd(0), THIRD_NEUTRAL);
    bt.AddInstruction(Or_A_lIYd(0), THIRD_NEUTRAL);

    bt.AddInstruction(Xor_A_Int8(0), SECOND_NEUTRAL);
    bt.AddInstruction(Xor_A_lHL(), NO_NEUTRALS);
    bt.AddInstruction(Xor_A_lIXd(0), THIRD_NEUTRAL);
    bt.AddInstruction(Xor_A_lIYd(0), THIRD_NEUTRAL);

    bt.AddInstruction(Cp_A_Int8(0), SECOND_NEUTRAL);
    bt.AddInstruction(Cp_A_lHL(), NO_NEUTRALS);
    bt.AddInstruction(Cp_A_lIXd(0), THIRD_NEUTRAL);
    bt.AddInstruction(Cp_A_lIYd(0), THIRD_NEUTRAL);

    bt.AddInstruction(Dec_lHL(), NO_NEUTRALS);
    bt.AddInstruction(Dec_lIXd(0), THIRD_NEUTRAL);
    bt.AddInstruction(Dec_lIYd(0), THIRD_NEUTRAL);
    bt.AddInstruction(Inc_lHL(), NO_NEUTRALS);
    bt.AddInstruction(Inc_lIXd(0), THIRD_NEUTRAL);
    bt.AddInstruction(Inc_lIYd(0), THIRD_NEUTRAL);

    for (reg_r r1 : rregs) {
        bt.AddInstruction(Load_Reg_Int8(r1, 0), SECOND_NEUTRAL);
        bt.AddInstruction(Load_Reg_lHL(r1), NO_NEUTRALS);
        bt.AddInstruction(Load_Reg_lIXd(r1, 0), THIRD_NEUTRAL);
        bt.AddInstruction(Load_Reg_lIYd(r1, 0), THIRD_NEUTRAL);
        bt.AddInstruction(Load_lHL_Reg(r1), NO_NEUTRALS);
        bt.AddInstruction(Load_lIXd_Reg(0, r1), THIRD_NEUTRAL);
        bt.AddInstruction(Load_lIYd_Reg(0, r1), THIRD_NEUTRAL);
        bt.AddInstruction(Add_A_Reg(r1), NO_NEUTRALS);
        bt.AddInstruction(Add_CY_A_Reg(r1), NO_NEUTRALS);
        bt.AddInstruction(Sub_A_Reg(r1), NO_NEUTRALS);
        bt.AddInstruction(Sub_CY_A_Reg(r1), NO_NEUTRALS);
        bt.AddInstruction(And_A_Reg(r1), NO_NEUTRALS);
        bt.AddInstruction(Or_A_Reg(r1), NO_NEUTRALS);
        bt.AddInstruction(Xor_A_Reg(r1), NO_NEUTRALS);
        bt.AddInstruction(Cp_A_Reg(r1), NO_NEUTRALS);
        bt.AddInstruction(Dec_Reg(r1), NO_NEUTRALS);
        bt.AddInstruction(Inc_Reg(r1), NO_NEUTRALS);


        for (reg_r r2 : rregs) {
            bt.AddInstruction(Load_Reg_Reg(r1, r2), NO_NEUTRALS);
        }
    }

    bt.AddInstruction(Load_SP_HL(), NO_NEUTRALS);
    bt.AddInstruction(Load_SP_IX(), NO_NEUTRALS);
    bt.AddInstruction(Load_SP_IY(), NO_NEUTRALS);
    bt.AddInstruction(Push_IX(), NO_NEUTRALS);
    bt.AddInstruction(Push_IY(), NO_NEUTRALS);
    bt.AddInstruction(Pop_IX(), NO_NEUTRALS);
    bt.AddInstruction(Pop_IY(), NO_NEUTRALS);
    bt.AddInstruction(Load_IX_Int16(0), THIRD_NEUTRAL | FOURTH_NEUTRAL);
    bt.AddInstruction(Load_IY_Int16(0), THIRD_NEUTRAL | FOURTH_NEUTRAL);
    bt.AddInstruction(Load_HL_lInt16(0), SECOND_NEUTRAL | THIRD_NEUTRAL);
    bt.AddInstruction(Load_IX_lInt16(0), THIRD_NEUTRAL | FOURTH_NEUTRAL);
    bt.AddInstruction(Load_IY_lInt16(0), THIRD_NEUTRAL | FOURTH_NEUTRAL);
    bt.AddInstruction(Load_lInt16_HL(0), SECOND_NEUTRAL | THIRD_NEUTRAL);
    bt.AddInstruction(Load_IX_lInt16(0), THIRD_NEUTRAL | FOURTH_NEUTRAL);
    bt.AddInstruction(Load_IY_lInt16(0), THIRD_NEUTRAL | FOURTH_NEUTRAL);
    for (reg_dd r : ddregs) {
        bt.AddInstruction(Load_Reg_Int16(r, 0), SECOND_NEUTRAL | THIRD_NEUTRAL);
        bt.AddInstruction(Load_Reg_lInt16(r, 0), FOURTH_NEUTRAL | THIRD_NEUTRAL);
        bt.AddInstruction(Load_lInt16_Reg(0, r), FOURTH_NEUTRAL | THIRD_NEUTRAL);
    }
    for (reg_qq r : qqregs) {
        bt.AddInstruction(Push_Reg(r), NO_NEUTRALS);
        bt.AddInstruction(Pop_Reg(r), NO_NEUTRALS);
    }

    bt.AddInstruction(Exchange_DE_HL(), NO_NEUTRALS);
    bt.AddInstruction(Exchange_AF_AFALT(), NO_NEUTRALS);
    bt.AddInstruction(Exchange_Alternates(), NO_NEUTRALS);
    bt.AddInstruction(Exchange_lSP_HL(), NO_NEUTRALS);
    bt.AddInstruction(Exchange_lSP_IX(), NO_NEUTRALS);
    bt.AddInstruction(Exchange_lSP_IY(), NO_NEUTRALS);
    bt.AddInstruction(Load_Decrement_Increase(), NO_NEUTRALS);
    bt.AddInstruction(Load_Decrement_Increase_Repeat(), NO_NEUTRALS);
    bt.AddInstruction(Load_Decrement_Decrement(), NO_NEUTRALS);
    bt.AddInstruction(Load_Decrement_Decrement_Repeat(), NO_NEUTRALS);
    bt.AddInstruction(Compare_Increase(), NO_NEUTRALS);
    bt.AddInstruction(Compare_Increase_Repeat(), NO_NEUTRALS);
    bt.AddInstruction(Compare_Decrease(), NO_NEUTRALS);
    bt.AddInstruction(Compare_Decrease_Repeat(), NO_NEUTRALS);

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
        ss << ", (ix ";
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
        ss << ", (iy ";
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
        ss << "ld (ix ";
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
        ss << "ld (iy ";
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
        ss << "ld (ix ";
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
        ss << "ld (iy ";
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

    stringificationFns[LD_REG_INT16] = [&](Instruction n) -> string {
        stringstream ss;
        ss << "ld ";
        ss << stringify_ddregs[(n.code[0] >> 4)];
        ss << ", 0x";
        ss << hex << (int) n.code[2] << (int) n.code[1];
        return ss.str();
    };

    stringificationFns[LD_IX_INT16] = [](Instruction n) -> string {
        stringstream ss;
        ss << "ld ix, 0x";
        ss << hex << (int) n.code[3] << (int) n.code[2];
        return ss.str();
    };

    stringificationFns[LD_IY_INT16] = [](Instruction n) -> string {
        stringstream ss;
        ss << "ld iy, 0x";
        ss << hex << (int) n.code[3] << (int) n.code[2];
        return ss.str();
    };

    stringificationFns[LD_HL_lINT16] = [](Instruction n) -> string {
        stringstream ss;
        ss << "ld hl, (0x";
        ss << hex << (int) n.code[2] << (int) n.code[1];
        ss << ")";
        return ss.str();
    };

    stringificationFns[LD_REG_lINT16] = [&](Instruction n) -> string {
        stringstream ss;
        ss << "ld ";
        ss << stringify_ddregs[(0b11 & (n.code[1] >> 4))];
        ss << ", (0x";
        ss << hex << (int) n.code[3] << (int) n.code[2];
        ss << ")";
        return ss.str();
    };

    stringificationFns[LD_IX_lINT16] = [](Instruction n) -> string {
        stringstream ss;
        ss << "ld ix, (0x";
        ss << hex << (int) n.code[3] << (int) n.code[2];
        ss << ")";
        return ss.str();
    };

    stringificationFns[LD_IY_lINT16] = [](Instruction n) -> string {
        stringstream ss;
        ss << "ld iy, (0x";
        ss << hex << (int) n.code[3] << (int) n.code[2];
        ss << ")";
        return ss.str();
    };

    stringificationFns[LD_lINT16_HL] = [](Instruction n) -> string {
        stringstream ss;
        ss << "ld (0x";
        ss << hex << (int) n.code[2] << (int) n.code[1];
        ss << "), hl";
        return ss.str();
    };

    stringificationFns[LD_lINT16_REG] = [&](Instruction n) -> string {
        stringstream ss;
        ss << "ld ";
        ss << "(0x";
        ss << hex << (int) n.code[3] << (int) n.code[2];
        ss << "), ";
        ss << stringify_ddregs[(0b11 & (n.code[1] >> 4))];
        return ss.str();
    };

    stringificationFns[LD_lINT16_IX] = [](Instruction n) -> string {
        stringstream ss;
        ss << "ld (0x";
        ss << hex << (int) n.code[3] << (int) n.code[2];
        ss << "), ix";
        return ss.str();
    };

    stringificationFns[LD_lINT16_IY] = [](Instruction n) -> string {
        stringstream ss;
        ss << "ld iy, (0x";
        ss << hex << (int) n.code[3] << (int) n.code[2];
        ss << "), iy";
        return ss.str();
    };

    stringificationFns[LD_SP_HL] = [](Instruction n) -> string {
        return "ld sp, hl";
    };

    stringificationFns[LD_SP_IX] = [](Instruction n) -> string {
        return "ld sp, ix";
    };   
    
    stringificationFns[LD_SP_IY] = [](Instruction n) -> string {
        return "ld sp, iy";
    };

    stringificationFns[PUSH_REG] = [&](Instruction n) -> string {
        stringstream ss;
        ss << "push ";
        ss << stringify_qqregs[(3 & (n.code[0] >> 4))];
        return ss.str();
    };

    stringificationFns[POP_REG] = [&](Instruction n) -> string {
        stringstream ss;
        ss << "pop ";
        ss << stringify_qqregs[(3 & (n.code[0] >> 4))];
        return ss.str();
    };

    stringificationFns[PUSH_IX] = [](Instruction n) -> string {
        return "push ix";
    };

    stringificationFns[PUSH_IY] = [](Instruction n) -> string {
        return "push iy";
    };

    stringificationFns[POP_IX] = [](Instruction n) -> string {
        return "pop ix";
    };

    stringificationFns[POP_IY] = [](Instruction n) -> string {
        return "pop iy";
    };

    stringificationFns[EX_DE_HL] = [](Instruction n) -> string {
        return "ex de, hl";
    };

    stringificationFns[EX_AF_AFALT] = [](Instruction n) -> string {
        return "ex af, af'";
    };

    stringificationFns[EXX] = [](Instruction n) -> string {
        return "exx";
    };

    stringificationFns[EX_lSP_HL] = [](Instruction n) -> string {
        return "ex (sp), hl";
    };

    stringificationFns[EX_lSP_IX] = [](Instruction n) -> string {
        return "ex (sp), ix";
    };

    stringificationFns[EX_lSP_IY] = [](Instruction n) -> string {
        return "ex (sp), iy";
    };

    stringificationFns[LDI] = [](Instruction n) -> string {
        return "ldi";
    };

    stringificationFns[LDIR] = [](Instruction n) -> string {
        return "ldir";
    };

    stringificationFns[LDD] = [](Instruction n) -> string {
        return "ldd";
    };

    stringificationFns[LDDR] = [](Instruction n) -> string {
        return "lddr";
    };

    stringificationFns[CPI] = [](Instruction n) -> string {
        return "cpi";
    };

    stringificationFns[CPIR] = [](Instruction n) -> string {
        return "cpir";
    };

    stringificationFns[CPD] = [](Instruction n) -> string {
        return "cpd";
    };

    stringificationFns[CPDR] = [](Instruction n) -> string {
        return "cpdr";
    };

    stringificationFns[ADD_A_REG] = [&](Instruction n) -> string {
        stringstream ss;
        ss << "add a, ";
        ss << stringify_rreg[(n.code[0] & 0b00000111)];
        return ss.str();
    };

    stringificationFns[ADD_A_INT8] = [](Instruction n) -> string {
        stringstream ss;
        ss << "add a, 0x";
        ss << hex << (int) n.code[1];
        return ss.str();
    };

    stringificationFns[ADD_A_lHL] = [](Instruction n) -> string {
        return "add a, (hl)";
    };

    stringificationFns[ADD_A_lIXd] = [](Instruction n) -> string {
        stringstream ss;
        ss << "add a, (ix ";
        ss << ((n.code[2] & 0b10000000) ? "-" : "+");
        
        int val = UINT8_TO_SINT(n.code[2]);
        ss << " 0x";
        ss << hex << val;
        ss << ")";
        
        return ss.str();
    };

    stringificationFns[ADD_A_lIYd] = [](Instruction n) -> string {
        stringstream ss;
        ss << "add a, (iy ";
        ss << ((n.code[2] & 0b10000000) ? "-" : "+");
        
        int val = UINT8_TO_SINT(n.code[2]);
        ss << " 0x";
        ss << hex << val;
        ss << ")";
        
        return ss.str();
    };

    stringificationFns[ADDC_A_REG] = [&](Instruction n) -> string {
        stringstream ss;
        ss << "addc a, ";
        ss << stringify_rreg[(n.code[0] & 0b00000111)];
        return ss.str();
    };

    stringificationFns[ADDC_A_INT8] = [](Instruction n) -> string {
        stringstream ss;
        ss << "addc a, 0x";
        ss << hex << (int) n.code[1];
        return ss.str();
    };

    stringificationFns[ADDC_A_lHL] = [](Instruction n) -> string {
        return "addc a, (hl)";
    };

    stringificationFns[ADDC_A_lIXd] = [](Instruction n) -> string {
        stringstream ss;
        ss << "addc a, (ix ";
        ss << ((n.code[2] & 0b10000000) ? "-" : "+");
        
        int val = UINT8_TO_SINT(n.code[2]);
        ss << " 0x";
        ss << hex << val;
        ss << ")";
        
        return ss.str();
    };

    stringificationFns[ADDC_A_lIYd] = [](Instruction n) -> string {
        stringstream ss;
        ss << "addc a, (iy ";
        ss << ((n.code[2] & 0b10000000) ? "-" : "+");
        
        int val = UINT8_TO_SINT(n.code[2]);
        ss << " 0x";
        ss << hex << val;
        ss << ")";
        
        return ss.str();
    };

    stringificationFns[SUB_A_REG] = [&](Instruction n) -> string {
        stringstream ss;
        ss << "sub a, ";
        ss << stringify_rreg[(n.code[0] & 0b00000111)];
        return ss.str();
    };

    stringificationFns[SUB_A_INT8] = [](Instruction n) -> string {
        stringstream ss;
        ss << "sub a, 0x";
        ss << hex << (int) n.code[1];
        return ss.str();
    };

    stringificationFns[SUB_A_lHL] = [](Instruction n) -> string {
        return "sub a, (hl)";
    };

    stringificationFns[SUB_A_lIXd] = [](Instruction n) -> string {
        stringstream ss;
        ss << "sub a, (ix ";
        ss << ((n.code[2] & 0b10000000) ? "-" : "+");
        
        int val = UINT8_TO_SINT(n.code[2]);
        ss << " 0x";
        ss << hex << val;
        ss << ")";
        
        return ss.str();
    };

    stringificationFns[SUB_A_lIYd] = [](Instruction n) -> string {
        stringstream ss;
        ss << "sub a, (iy ";
        ss << ((n.code[2] & 0b10000000) ? "-" : "+");
        
        int val = UINT8_TO_SINT(n.code[2]);
        ss << " 0x";
        ss << hex << val;
        ss << ")";
        
        return ss.str();
    };

    stringificationFns[SUBC_A_REG] = [&](Instruction n) -> string {
        stringstream ss;
        ss << "subc a, ";
        ss << stringify_rreg[(n.code[0] & 0b00000111)];
        return ss.str();
    };

    stringificationFns[SUBC_A_INT8] = [](Instruction n) -> string {
        stringstream ss;
        ss << "subc a, 0x";
        ss << hex << (int) n.code[1];
        return ss.str();
    };

    stringificationFns[SUBC_A_lHL] = [](Instruction n) -> string {
        return "subc a, (hl)";
    };

    stringificationFns[SUBC_A_lIXd] = [](Instruction n) -> string {
        stringstream ss;
        ss << "subc a, (ix ";
        ss << ((n.code[2] & 0b10000000) ? "-" : "+");
        
        int val = UINT8_TO_SINT(n.code[2]);
        ss << " 0x";
        ss << hex << val;
        ss << ")";
        
        return ss.str();
    };

    stringificationFns[SUBC_A_lIYd] = [](Instruction n) -> string {
        stringstream ss;
        ss << "subc a, (iy ";
        ss << ((n.code[2] & 0b10000000) ? "-" : "+");
        
        int val = UINT8_TO_SINT(n.code[2]);
        ss << " 0x";
        ss << hex << val;
        ss << ")";
        
        return ss.str();
    };

    stringificationFns[AND_A_REG] = [&](Instruction n) -> string {
        stringstream ss;
        ss << "and ";
        ss << stringify_rreg[(n.code[0] & 0b00000111)];
        return ss.str();
    };

    stringificationFns[AND_A_INT8] = [](Instruction n) -> string {
        stringstream ss;
        ss << "and 0x";
        ss << hex << (int) n.code[1];
        return ss.str();
    };

    stringificationFns[AND_A_lHL] = [](Instruction n) -> string {
        return "and (hl)";
    };

    stringificationFns[AND_A_lIXd] = [](Instruction n) -> string {
        stringstream ss;
        ss << "and (ix ";
        ss << ((n.code[2] & 0b10000000) ? "-" : "+");
        
        int val = UINT8_TO_SINT(n.code[2]);
        ss << " 0x";
        ss << hex << val;
        ss << ")";
        
        return ss.str();
    };

    stringificationFns[AND_A_lIYd] = [](Instruction n) -> string {
        stringstream ss;
        ss << "and (iy ";
        ss << ((n.code[2] & 0b10000000) ? "-" : "+");
        
        int val = UINT8_TO_SINT(n.code[2]);
        ss << " 0x";
        ss << hex << val;
        ss << ")";
        
        return ss.str();
    };

    stringificationFns[OR_A_REG] = [&](Instruction n) -> string {
        stringstream ss;
        ss << "or ";
        ss << stringify_rreg[(n.code[0] & 0b00000111)];
        return ss.str();
    };

    stringificationFns[OR_A_INT8] = [](Instruction n) -> string {
        stringstream ss;
        ss << "or 0x";
        ss << hex << (int) n.code[1];
        return ss.str();
    };

    stringificationFns[OR_A_lHL] = [](Instruction n) -> string {
        return "or (hl)";
    };

    stringificationFns[OR_A_lIXd] = [](Instruction n) -> string {
        stringstream ss;
        ss << "or (ix ";
        ss << ((n.code[2] & 0b10000000) ? "-" : "+");
        
        int val = UINT8_TO_SINT(n.code[2]);
        ss << " 0x";
        ss << hex << val;
        ss << ")";
        
        return ss.str();
    };

    stringificationFns[OR_A_lIYd] = [](Instruction n) -> string {
        stringstream ss;
        ss << "or (iy ";
        ss << ((n.code[2] & 0b10000000) ? "-" : "+");
        
        int val = UINT8_TO_SINT(n.code[2]);
        ss << " 0x";
        ss << hex << val;
        ss << ")";
        
        return ss.str();
    };

    stringificationFns[XOR_A_REG] = [&](Instruction n) -> string {
        stringstream ss;
        ss << "xor ";
        ss << stringify_rreg[(n.code[0] & 0b00000111)];
        return ss.str();
    };

    stringificationFns[XOR_A_INT8] = [](Instruction n) -> string {
        stringstream ss;
        ss << "xor 0x";
        ss << hex << (int) n.code[1];
        return ss.str();
    };

    stringificationFns[XOR_A_lHL] = [](Instruction n) -> string {
        return "xor (hl)";
    };

    stringificationFns[XOR_A_lIXd] = [](Instruction n) -> string {
        stringstream ss;
        ss << "xor (ix ";
        ss << ((n.code[2] & 0b10000000) ? "-" : "+");
        
        int val = UINT8_TO_SINT(n.code[2]);
        ss << " 0x";
        ss << hex << val;
        ss << ")";
        
        return ss.str();
    };

    stringificationFns[XOR_A_lIYd] = [](Instruction n) -> string {
        stringstream ss;
        ss << "xor (iy ";
        ss << ((n.code[2] & 0b10000000) ? "-" : "+");
        
        int val = UINT8_TO_SINT(n.code[2]);
        ss << " 0x";
        ss << hex << val;
        ss << ")";
        
        return ss.str();
    };

    stringificationFns[CP_A_REG] = [&](Instruction n) -> string {
        stringstream ss;
        ss << "cp ";
        ss << stringify_rreg[(n.code[0] & 0b00000111)];
        return ss.str();
    };

    stringificationFns[CP_A_INT8] = [](Instruction n) -> string {
        stringstream ss;
        ss << "cp 0x";
        ss << hex << (int) n.code[1];
        return ss.str();
    };

    stringificationFns[CP_A_lHL] = [](Instruction n) -> string {
        return "cp (hl)";
    };

    stringificationFns[CP_A_lIXd] = [](Instruction n) -> string {
        stringstream ss;
        ss << "cp (ix ";
        ss << ((n.code[2] & 0b10000000) ? "-" : "+");
        
        int val = UINT8_TO_SINT(n.code[2]);
        ss << " 0x";
        ss << hex << val;
        ss << ")";
        
        return ss.str();
    };

    stringificationFns[CP_A_lIYd] = [](Instruction n) -> string {
        stringstream ss;
        ss << "cp (iy ";
        ss << ((n.code[2] & 0b10000000) ? "-" : "+");
        
        int val = UINT8_TO_SINT(n.code[2]);
        ss << " 0x";
        ss << hex << val;
        ss << ")";
        
        return ss.str();
    };

    stringificationFns[INC_REG] = [&](Instruction n) -> string {
        stringstream ss;
        ss << "inc ";
        ss << stringify_rreg[((n.code[0] & 0b00111000) >> 3)];
        return ss.str();
    };

    stringificationFns[INC_lHL] = [](Instruction n) -> string {
        return "inc (hl)";
    };

    stringificationFns[INC_lIXd] = [](Instruction n) -> string {
        stringstream ss;
        ss << "inc (ix ";
        ss << ((n.code[2] & 0b10000000) ? "-" : "+");
        
        int val = UINT8_TO_SINT(n.code[2]);
        ss << " 0x";
        ss << hex << val;
        ss << ")";
        
        return ss.str();
    };

    stringificationFns[INC_lIYd] = [](Instruction n) -> string {
        stringstream ss;
        ss << "inc (iy ";
        ss << ((n.code[2] & 0b10000000) ? "-" : "+");
        
        int val = UINT8_TO_SINT(n.code[2]);
        ss << " 0x";
        ss << hex << val;
        ss << ")";
        
        return ss.str();
    };

        stringificationFns[DEC_REG] = [&](Instruction n) -> string {
        stringstream ss;
        ss << "dec ";
        ss << stringify_rreg[((n.code[0] & 0b00111000) >> 3)];
        return ss.str();
    };

    stringificationFns[DEC_lHL] = [](Instruction n) -> string {
        return "dec (hl)";
    };

    stringificationFns[DEC_lIXd] = [](Instruction n) -> string {
        stringstream ss;
        ss << "dec (ix ";
        ss << ((n.code[2] & 0b10000000) ? "-" : "+");
        
        int val = UINT8_TO_SINT(n.code[2]);
        ss << " 0x";
        ss << hex << val;
        ss << ")";
        
        return ss.str();
    };

    stringificationFns[DEC_lIYd] = [](Instruction n) -> string {
        stringstream ss;
        ss << "dec (iy ";
        ss << ((n.code[2] & 0b10000000) ? "-" : "+");
        
        int val = UINT8_TO_SINT(n.code[2]);
        ss << " 0x";
        ss << hex << val;
        ss << ")";
        
        return ss.str();
    };
}

