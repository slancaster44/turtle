#ifndef GEN_H
#define GEN_H
#include <cstdint>
#include <functional>
#include <string>
using namespace std;

#define INS_LEN 4

enum Instruction_Tag {
    LD_REG_REG,     // r1 <- r2
    LD_REG_INT8,     // r <- n
    LD_REG_lHL,     // r <- (HL)
    LD_REG_lIXd,     // r <- (IX + d)
    LD_REG_lIYd,    // r <- (IY + d) 
    LD_lHL_REG,     // (HL) <- r
    LD_lIXd_REG,    // (IX + d) <- r
    LD_lIYd_REG,    // (IY + d) <- r
    LD_lHL_INT8,     // (HL) <- n
    LD_lIXd_INT8,    // (IX + d) <- n
    LD_lIYd_INT8,    // (IY + d) <- n
    LD_A_lBC,       // A <- (BC)
    LD_A_lDE,       // A <- (DE)
    LD_A_lINT16,      // A <- (nn)
    LD_lBC_A,       // (BC) <- A
    LD_lDE_A,       // (DE) <- A
    LD_lINT16_A,      // (nn) <- A
    LD_A_I,         // A <- I
    LD_A_R,         // A <- R
    LD_I_A,         // I <- A
    LD_R_A,         // R <- A
    LD_REG_INT16,   // rr <- nn
    LD_IX_INT16,
    LD_IY_INT16,
    LD_HL_lINT16,
    LD_REG_lINT16,
    LD_IX_lINT16,
    LD_IY_lINT16,
    LD_lINT16_HL,
    LD_lINT16_REG,
    LD_lINT16_IX,
    LD_lINT16_IY,
    LD_SP_HL,
    LD_SP_IX,
    LD_SP_IY,
    PUSH_REG,
    PUSH_IX,
    PUSH_IY,
    POP_REG,
    POP_IX,
    POP_IY,
    EX_DE_HL,
    EX_AF_AFALT,
    EXX,
    EX_lSP_HL,
    EX_lSP_IX,
    EX_lSP_IY,
    LDI,
    LDIR,
    LDD,
    LDDR,
    CPI,
    CPIR,
    CPD,
    CPDR,
    ADD_A_REG,
    ADD_A_INT8,
    ADD_A_lHL,
    ADD_A_lIXd,
    ADD_A_lIYd,
    ADC_A_REG,
    ADC_A_INT8,
    ADC_A_lHL,
    ADC_A_lIXd,
    ADC_A_lIYd,
    SUB_A_REG,
    SUB_A_INT8,
    SUB_A_lHL,
    SUB_A_lIXd,
    SUB_A_lIYd,
    SBC_A_REG,
    SBC_A_INT8,
    SBC_A_lHL,
    SBC_A_lIXd,
    SBC_A_lIYd,
    AND_A_REG,
    AND_A_INT8,
    AND_A_lHL,
    AND_A_lIXd,
    AND_A_lIYd,
    OR_A_REG,
    OR_A_INT8,
    OR_A_lHL,
    OR_A_lIXd,
    OR_A_lIYd,
    XOR_A_REG,
    XOR_A_INT8,
    XOR_A_lHL,
    XOR_A_lIXd,
    XOR_A_lIYd,
    CP_A_REG,
    CP_A_INT8,
    CP_A_lHL,
    CP_A_lIXd,
    CP_A_lIYd,
    INC_REG,
    INC_lHL,
    INC_lIXd,
    INC_lIYd,
    DEC_REG,
    DEC_lHL,
    DEC_lIXd,
    DEC_lIYd,
    DAA,
    CPL,
    NEG,
    CCF,
    SCF,
    NOP,
    HALT,
    DI,
    EI,
    IM0, 
    IM1,
    IM2,
    ADD_HL_REG,
    ADC_HL_REG,
    SBC_HL_REG,
    ADD_IX_REG,
    ADD_IY_REG,
    INC_REG16,
    INC_IX,
    INC_IY,
    DEC_REG16,
    DEC_IX,
    DEC_IY,
    RLCA,
    RLA,
    RRCA,
    RRA,
    RLC_REG,
    RLC_lHL,
    RLC_lIXd,
    RLC_lIYd,
    RL_REG,
    RL_lHL,
    RL_lIXd,
    RL_lIYd,
    RRC_REG,
    RRC_lHL,
    RRC_lIXd,
    RRC_lIYd,
    RR_REG,
    RR_lHL,
    RR_lIXd,
    RR_lIYd,
    SLA_REG,
    SLA_lHL,
    SLA_lIXd,
    SLA_lIYd,
    SRA_REG,
    SRA_lHL,
    SRA_lIXd,
    SRA_lIYd,
    SRL_REG,
    SRL_lHL,
    SRL_lIXd,
    SRL_lIYd,
    RLD,
    RRD,
    BIT_REG,
    BIT_lHL,
    BIT_lIXd,
    BIT_lIYd,
    SET_REG,
    SET_lHL,
    SET_lIXd,
    SET_lIYd,
    RESET_REG,
    RESET_lHL,
    RESET_lIXd,
    RESET_lIYd,
    JP_lINT16,
    JP_lHL,
    JP_lIX,
    JP_lIY,
    JP_CC_lINT16,
    JR,
    JR_C_OFF8,
    DJNZ,
    CALL,
    CALL_CC,
    RET,
    RET_CC,
    RETI,
    RETN,
    RST_P,
};

typedef struct {
    uint8_t code[INS_LEN];        //Space enough for largest instruction encoding
    int code_len;           //Length of instruction in bytes
    Instruction_Tag tag;    //Indicator as to which instruction this is
    function<string()> stringify;
} Instruction;

Instruction newInstruction(Instruction_Tag tag, uint8_t code[INS_LEN], int len);
void printInstruction(Instruction n);
bool match(Instruction n1, Instruction n2);

/* A bunch of values to pass into code generation
 * functions to indicate arguments such as registers
 * status bits and reset vectors
 */
typedef uint8_t reg_dd;
typedef uint8_t reg_qq;
typedef uint8_t reg_pp;
typedef uint8_t reg_rr;
typedef uint8_t status_cc;
typedef uint8_t reg_r;
typedef uint8_t status_c;
typedef uint8_t reset_p;

extern reg_r A_R;
extern reg_r B_R;
extern reg_r C_R;
extern reg_r D_R;
extern reg_r E_R;
extern reg_r H_R;
extern reg_r L_R;

extern reg_dd BC_DD;
extern reg_dd DE_DD;
extern reg_dd HL_DD;
extern reg_dd SP_DD;

extern reg_pp BC_PP;
extern reg_pp DE_PP;
extern reg_pp IX_PP;
extern reg_pp SP_PP;

extern reg_rr BC_RR;
extern reg_rr DE_RR;
extern reg_rr IY_RR;
extern reg_rr SP_RR;

extern reg_qq BC_QQ;
extern reg_qq DE_QQ;
extern reg_qq HL_QQ;
extern reg_qq AF_QQ;

extern status_cc NZ_CC;
extern status_cc Z_CC ;
extern status_cc NC_CC;
extern status_cc C_CC ;
extern status_cc PO_CC;
extern status_cc PE_CC;
extern status_cc P_CC;
extern status_cc M_CC ;

extern status_c NZ_C;
extern status_c Z_C ;
extern status_c NC_C;
extern status_c C_C ;

extern reset_p R_00H;
extern reset_p R_08H;
extern reset_p R_10H;
extern reset_p R_18H;
extern reset_p R_20H;
extern reset_p R_28H;
extern reset_p R_30H0;
extern reset_p R_38H;

/* 8bit Load Group */

/* These functions will generate instructions in the 8bit
    * load group that are described starting on pg70 of the
    * z80 CPU user manual
    *
    * Note on naming:
    * Lowercase 'l' will indicate that the following item
    * will contain a location that will be used in the operation
    * Example: Load_lBC_A loads the value in 'A' into the location
    * stored in BC. Thus it can be read as: Load into the location in BC
    * the value stored in 'A.'
    *
    * Lowercase 'd' will be sued on index registers to indicate that the
    * index is applied to the regester during the operation. For example
    * Load_lIXd_Int can be read as load into the location 'IX + the offset d'
    * the integer.
    */

Instruction Load_Reg_Reg(reg_r r1, reg_r r2);    // r1 <- r2
Instruction Load_Reg_Int8(reg_r r, uint8_t n);   // r <- n
Instruction Load_Reg_lHL(reg_r r);               // r <- (HL)
Instruction Load_Reg_lIXd(reg_r r, uint8_t d);   // r <- (IX + d)
Instruction Load_Reg_lIYd(reg_r r, uint8_t d);   // r <- (IY + d) 
Instruction Load_lHL_Reg(reg_r r);               // (HL) <- r
Instruction Load_lIXd_Reg(uint8_t d, reg_r r);   // (IX + d) <- r
Instruction Load_lIYd_Reg(uint8_t d, reg_r r);   // (IY + d) <- r
Instruction Load_lHL_Int8(uint8_t n);            // (HL) <- n
Instruction Load_lIXd_Int8(uint8_t d, uint8_t n);// (IX + d) <- n
Instruction Load_lIYd_Int8(uint8_t d, uint8_t n);// (IY + d) <- n
Instruction Load_A_lBC();                        // A <- (BC)
Instruction Load_A_lDE();                        // A <- (DE)
Instruction Load_A_lInt16(uint16_t nn);          // A <- (nn)
Instruction Load_lBC_A();                        // (BC) <- A
Instruction Load_lDE_A();                        // (DE) <- A
Instruction Load_lInt16_A(uint16_t nn);          // (nn) <- A
Instruction Load_A_I();                          // A <- I
Instruction Load_A_R();                          // A <- R
Instruction Load_I_A();                          // I <- A
Instruction Load_R_A();                          // R <- A


/* 16bit load group */

Instruction Load_Reg_Int16(reg_dd r, uint16_t nn);  // r <- nn 
Instruction Load_IX_Int16(uint16_t nn);             // IX <- nn
Instruction Load_IY_Int16(uint16_t nn);             // IY <- nn
Instruction Load_HL_lInt16(uint16_t nn);            // hl <- (nn)
Instruction Load_Reg_lInt16(reg_dd r, uint16_t nn); // r <- (nn)
Instruction Load_IX_lInt16(uint16_t nn);            // IX <- (nn)
Instruction Load_IY_lInt16(uint16_t nn);            // IY <- (nn)
Instruction Load_lInt16_HL(uint16_t nn);            // (nn) <- HL
Instruction Load_lInt16_Reg(uint16_t nn, reg_dd r); // (nn) <- r
Instruction Load_lInt16_IX(uint16_t nn);            // (nn) <- IX
Instruction Load_lInt16_IY(uint16_t nn);            // (nn) <- IY
Instruction Load_SP_HL();                           // SP <- HL
Instruction Load_SP_IX();                           // SP <- IX
Instruction Load_SP_IY();                           // SP <- IY
Instruction Push_Reg(reg_qq r);                     // (SP) <- r; SP--
Instruction Push_IX();                              // (SP) <- IX; SP--
Instruction Push_IY();                              // (SP) <- IY; SP--
Instruction Pop_Reg(reg_qq r);                      // r <- (SP); SP++
Instruction Pop_IX();                               // IX <- (SP); S++
Instruction Pop_IY();                               // IY <- (SP); SP++

/* Exchange, block transfer and search group */
Instruction Exchange_DE_HL();                   // DE <-> HL
Instruction Exchange_AF_AFALT();                // AF <-> AF'
Instruction Exchange_Alternates();              // BC <-> BC'; DE <-> DE'; HL <-> HL'
Instruction Exchange_lSP_HL();                  // (SP) <-> HL
Instruction Exchange_lSP_IX();                  // (SP) <-> IX
Instruction Exchange_lSP_IY();                  // (SP) <-> IY
Instruction Load_Decrement_Increase();          // (DE) <- (HL); BC-- DE++; HL++; 
Instruction Load_Decrement_Increase_Repeat();   // while (BC != 0) { (DE) <- (HL); BC-- DE++; HL++; }
Instruction Load_Decrement_Decrement();         // (DE) <- (HL); BC-- DE--; HL--; 
Instruction Load_Decrement_Decrement_Repeat();  // while (BC != 0) { (DE) <- (HL); BC-- DE--; HL--; }
Instruction Compare_Increase();                 // CP(A, (HL)); HL++; BC--
Instruction Compare_Increase_Repeat();          // while(A != (HL) && BC != 0) { HL++; BC--; }
Instruction Compare_Decrease();                 // CP(A, (HL)); HL--; BC--
Instruction Compare_Decrease_Repeat();          // while(A != (HL) && BC != 0) { HL--; BC--; }

/* 8-bit Arithmetic Group */
Instruction Add_A_Reg(reg_r r);         // A <- A + r
Instruction Add_A_Int8(uint8_t n);      // A <- A + n
Instruction Add_A_lHL();                // A <- A + (HL)
Instruction Add_A_lIXd(uint8_t d);      // A <- A + (IX + d)
Instruction Add_A_lIYd(uint8_t d);      // A <- A + (IY + d)
Instruction Add_CY_A_Reg(reg_r r);      // A <- A + CY + r
Instruction Add_CY_A_Int8(uint8_t n);   // A <- A + CY + n
Instruction Add_CY_A_lHL();             // A <- A + CY + (HL)
Instruction Add_CY_A_lIXd(uint8_t d);   //A <- A + CY + (IX + d)
Instruction Add_CY_A_lIYd(uint8_t d);   //A <- A + CY + (IX + d)

Instruction Sub_A_Reg(reg_r r);         //A <- A - r          
Instruction Sub_A_Int8(uint8_t n);      //A <- A - n          
Instruction Sub_A_lHL();                //A <- A - (HL)        
Instruction Sub_A_lIXd(uint8_t d);      //A <- A - (IX + d)   
Instruction Sub_A_lIYd(uint8_t d);      //A <- A - (IY + d)     
Instruction Sub_CY_A_Reg(reg_r r);      
Instruction Sub_CY_A_Int8(uint8_t n);    
Instruction Sub_CY_A_lHL();              
Instruction Sub_CY_A_lIXd(uint8_t d);   
Instruction Sub_CY_A_lIYd(uint8_t d);   

Instruction And_A_Reg(reg_r r);     //A <- A & r         
Instruction And_A_Int8(uint8_t n);  //A <- A & n    
Instruction And_A_lHL();            //A <- A & (HL)   
Instruction And_A_lIXd(uint8_t d);  //A <- A & (IX + d) 
Instruction And_A_lIYd(uint8_t d);  //A <- A & (IY + d) 

Instruction Or_A_Reg(reg_r r);     //A <- A | r    
Instruction Or_A_Int8(uint8_t n);  //A <- A | n       
Instruction Or_A_lHL();            //A <- A | (HL)   
Instruction Or_A_lIXd(uint8_t d);  //A <- A | (IX + d) 
Instruction Or_A_lIYd(uint8_t d);  //A <- A | (IY + d) 

Instruction Xor_A_Reg(reg_r r);     //A <- A ^| r    
Instruction Xor_A_Int8(uint8_t n);  //A <- A ^| n        
Instruction Xor_A_lHL();            //A <- A ^| (HL)   
Instruction Xor_A_lIXd(uint8_t d);  //A <- A ^| (IX + d) 
Instruction Xor_A_lIYd(uint8_t d);  //A <- A ^| (IX + d) 

Instruction Cp_A_Reg(reg_r r);      // TMP <- A; A <- A - r; A <- TMP      
Instruction Cp_A_Int8(uint8_t n);   // TMP <- A; A <- A - n; A <- TMP    
Instruction Cp_A_lHL();             // TMP <- A; A <- A - (HL); A <- TMP    
Instruction Cp_A_lIXd(uint8_t d);   // TMP <- A; A <- A - (IX + d); A <- TMP    
Instruction Cp_A_lIYd(uint8_t d);   // TMP <- A; A <- A - (IY + d); A <- TMP     

Instruction Inc_Reg(reg_r r);   // r++
Instruction Inc_lHL();          // (HL)++
Instruction Inc_lIXd(uint8_t d);          // (IX)++
Instruction Inc_lIYd(uint8_t d);          // (IY)++

Instruction Dec_Reg(reg_r r);   // r--
Instruction Dec_lHL();          // (HL)--
Instruction Dec_lIXd(uint8_t d);          // (IX)--
Instruction Dec_lIYd(uint8_t d);          // (IY)--

/* General Purpose Arithmatic and CPU Control Groups */
Instruction Decimal_Adjust();       //Its complicated see pg 173
Instruction Complement_A();         // A <- A ^| 0b11111111
Instruction Negate();               // A <- 0 - A
Instruction Complement_CF();        // CY <- CY ^| 1
Instruction Set_CF();       // CY <- 1
Instruction Nop();                  //Nothing Intresting
Instruction Halt();                 //No catch fire
Instruction Disable_Interrupts();   //Does what it says
Instruction Enable_Interrupts();    //Undoes the other one
Instruction Interrupt_Mode0();
Instruction Interrupt_Mode1();
Instruction Interrupt_Mode2();

/* 16bit Arithmetic Group */
Instruction Add_HL_Reg(reg_dd r);       //HL <- HL + r
Instruction Add_CY_HL_Reg(reg_dd r);    //HL <- HL + CY + r
Instruction Sub_CY_HL_Reg(reg_dd r);    //HL <- HL - CY - r
Instruction Add_IX_Reg(reg_pp r);       //IX <- IX + r
Instruction Add_IY_Reg(reg_rr r);       //IX <- IX + r
Instruction Inc_Reg16(reg_dd r);          //r++
Instruction Inc_IX();                   //IX++
Instruction Inc_IY();                   //IY++
Instruction Dec_Reg16(reg_dd r);          //r--
Instruction Dec_IX();                   //IX--
Instruction Dec_IY();                   //IY--

/* Rotate and Shift Group */    
Instruction Rotate_Left_Carry_A();          //CY = A[0]; A << 1; 
Instruction Rotate_Left_A();                //(CY A) << 1
Instruction Rotate_Right_Carry_A();         //CY = A[0]; A >> 1; 
Instruction Rotate_Right_A();               //(CY A) >> 1
Instruction Rotate_Left_Carry_Reg(reg_r r);
Instruction Rotate_Left_Carry_lHL();
Instruction Rotate_Left_Carry_lIXd(uint8_t d);
Instruction Rotate_Left_Carry_lIYd(uint8_t d);
Instruction Rotate_Left_Reg(reg_r r);
Instruction Rotate_Left_lHL();
Instruction Rotate_Left_lIXd(uint8_t d);
Instruction Rotate_Left_lIYd(uint8_t d);
Instruction Rotate_Right_Carry_Reg(reg_r r);
Instruction Rotate_Right_Carry_lHL();
Instruction Rotate_Right_Carry_lIXd(uint8_t d);
Instruction Rotate_Right_Carry_lIYd(uint8_t d);
Instruction Rotate_Right_Reg(reg_r r);
Instruction Rotate_Right_lHL();
Instruction Rotate_Right_lIXd(uint8_t d);
Instruction Rotate_Right_lIYd(uint8_t d);
Instruction Shift_Left_Arithmetic_Reg(reg_r r);
Instruction Shift_Left_Arithmetic_lHL();
Instruction Shift_Left_Arithmetic_lIXd(uint8_t d);
Instruction Shift_Left_Arithmetic_lIYd(uint8_t d);
Instruction Shift_Right_Arithmetic_Reg(reg_r r);
Instruction Shift_Right_Arithmetic_lHL();
Instruction Shift_Right_Arithmetic_lIXd(uint8_t d);
Instruction Shift_Right_Arithmetic_lIYd(uint8_t d);
Instruction Shift_Right_Logical_Reg(reg_r r);
Instruction Shift_Right_Logical_lHL();
Instruction Shift_Right_Logical_lIXd(uint8_t d);
Instruction Shift_Right_Logical_lIYd(uint8_t d);
Instruction Rotate_Left_Digit_A();
Instruction Rotate_Right_Digit_A();

/*Bit Set & Reset Group */
Instruction Bit_Reg(uint8_t n, reg_r r);
Instruction Bit_lHL(uint8_t n);
Instruction Bit_lIXd(uint8_t n, uint8_t d);
Instruction Bit_lIYd(uint8_t n, uint8_t d);
Instruction Set_Reg(uint8_t n, reg_r r);
Instruction Set_lHL(uint8_t n);
Instruction Set_lIXd(uint8_t n, uint8_t d);
Instruction Set_lIYd(uint8_t n, uint8_t d);
Instruction Reset_Reg(uint8_t n, reg_r r);
Instruction Reset_lHL(uint8_t n);
Instruction Reset_lIXd(uint8_t n, uint8_t d);
Instruction Reset_lIYd(uint8_t n, uint8_t d);

/* Jump Group */
Instruction Jump_lInt16(uint16_t addr);
Instruction Jump_lHL();
Instruction Jump_lIX();
Instruction Jump_lIY();
Instruction Jump_CC_lInt16(status_cc cc, uint16_t addr);
Instruction Jump_Relative(uint8_t offset);
Instruction Jump_Realtive_C(status_c c, uint8_t offset);
Instruction Decrement_Jump_NZ(uint8_t offset); // while (B != 0) { B--; }

/* Call & Return Group */
Instruction Call(uint16_t addr);
Instruction Call_CC(status_cc cc, uint16_t addr);
Instruction Return();
Instruction Return_CC(status_cc cc);
Instruction Return_From_Interrupt();
Instruction Return_From_Nonmaskable();
Instruction Reset(reset_p p);

/* Input Output Group */
Instruction Input_A_lInt8(uint8_t addr);
Instruction Input_Reg_lC(reg_r r);
Instruction Input_Increment();
Instruction Input_Increment_Repeat();
Instruction Input_Decrement();
Instruction Input_Decrement_Repeat();
Instruction Output_A_lInt8(uint8_t addr);
Instruction Output_Reg_lC(reg_r r);
Instruction Output_Increment();
Instruction Output_Increment_Repeat();
Instruction Output_Decrement();
Instruction Output_Decrement_Repeat();

#endif