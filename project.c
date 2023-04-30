/* 
Class Project: The logical conclusion
CSCI-2500 Spring 2023
Prof. Slota 
*/

/******************************************************************************/
/* Usual suspects to include  */
/******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "circuits.h"

// define BIT type as a char (i.e., one byte)
typedef char BIT;
#define TRUE 1
#define FALSE 0
#define UNDEF -1
// useful constants
BIT ONE[32] = {TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 
  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
  FALSE};
BIT ZERO[32] = {FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
  FALSE}; BIT REG_THIRTY_ONE[5] = {TRUE, TRUE, TRUE, TRUE, TRUE};
BIT THIRTY_TWO[32] = {FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, FALSE, FALSE, FALSE, 
  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
  FALSE};


/******************************************************************************/
/* Function prototypes */
/******************************************************************************/
BIT not_gate(BIT A);
BIT or_gate(BIT A, BIT B);
BIT or_gate3(BIT A, BIT B, BIT C);
BIT and_gate(BIT A, BIT B);
BIT and_gate3(BIT A, BIT B, BIT C);
BIT xor_gate(BIT A, BIT B);
BIT nor_gate(BIT A, BIT B);
BIT nand_gate(BIT A, BIT B);

void decoder2(BIT* I, BIT EN, BIT* O);
BIT multiplexor2(BIT S, BIT I0, BIT I1);
void multiplexor2_32(BIT S, BIT* I0, BIT* I1, BIT* Output);
BIT multiplexor4(BIT S0, BIT S1, BIT I0, BIT I1, BIT I2, BIT I3);

void copy_bits(BIT* A, BIT* B);
void print_binary(BIT* A);
void convert_to_binary(int a, BIT* A);
void convert_to_binary_char(int a, char* A, int length);
int binary_to_integer(BIT* A);

int get_instructions(BIT Instructions[][32]);

void Instruction_Memory(BIT* ReadAddress, BIT* Instruction);
void Control(BIT* OpCode);
void Read_Register(BIT* ReadRegister1, BIT* ReadRegister2,
  BIT* ReadData1, BIT* ReadData2);
void Write_Register(BIT RegWrite, BIT* WriteRegister, BIT* WriteData);
void ALU_Control(BIT* funct);
void ALU(BIT* ALUControl, BIT* Input1, BIT* Input2, BIT* Zero, BIT* Result);
void Data_Memory(BIT MemWrite, BIT MemRead, 
  BIT* Address, BIT* WriteData, BIT* ReadData);
void Extend_Sign16(BIT* Input, BIT* Output);
void updateState();
  
/******************************************************************************/
/* Functions provided for your convenience */
/******************************************************************************/
BIT not_gate(BIT A)
{
  return (!A);
}

BIT or_gate(BIT A, BIT B)
{
  return (A || B);
}

BIT or_gate3(BIT A, BIT B, BIT C)
{
  return or_gate(A, or_gate(B, C));
}

BIT and_gate(BIT A, BIT B)
{
  return (A && B);
}

BIT and_gate3(BIT A, BIT B, BIT C)
{
  return and_gate(A, and_gate(B, C));
}

BIT xor_gate(BIT A, BIT B)
{
  BIT nA = not_gate(A);
  BIT nB = not_gate(B);
  BIT x0 = and_gate(A, nB);
  BIT x1 = and_gate(nA, B);
  return or_gate(x0, x1);
}

BIT nor_gate(BIT A, BIT B)
{
  return not_gate(or_gate(A, B));
}

BIT nand_gate(BIT A, BIT B)
{
  return not_gate(and_gate(A, B));
}

void decoder2(BIT* I, BIT EN, BIT* O)
{
  BIT nI1 = not_gate(I[1]);
  BIT nI0 = not_gate(I[0]);
  O[0] = and_gate(nI1, nI0);
  O[1] = and_gate(nI1, I[0]);
  O[2] = and_gate(I[1], nI0);
  O[3] = and_gate(I[1],  I[0]);
  
  // Note use of EN (enable) line below
  for (int i = 0; i < 4; ++i)
    O[i] = and_gate(EN, O[i]);
  
  return;
}

void decoder3(BIT* I, BIT EN, BIT* O)
{
  O[0] = and_gate3(not_gate(I[2]), not_gate(I[1]), not_gate(I[0]));
  O[1] = and_gate3(not_gate(I[2]), not_gate(I[1]), I[0]);
  O[2] = and_gate3(not_gate(I[2]), I[1], not_gate(I[0]));
  O[3] = and_gate3(not_gate(I[2]), I[1], I[0]);
  O[4] = and_gate3(I[2], not_gate(I[1]), not_gate(I[0]));
  O[5] = and_gate3(I[2], not_gate(I[1]), I[0]);
  O[6] = and_gate3(I[2], I[1], not_gate(I[0]));
  O[7] = and_gate3(I[2], I[1], I[0]);
  
  for (int i = 0; i < 8; ++i)
    O[i] = and_gate(EN, O[i]);
  
  return;
}

void decoder5(BIT* I, BIT EN, BIT* O)
{
   BIT EN_O[4] = {FALSE};
   decoder2(&I[3], EN, EN_O);
   decoder3(I, EN_O[3], &O[24]);
   decoder3(I, EN_O[2], &O[16]);
   decoder3(I, EN_O[1], &O[8]);
   decoder3(I, EN_O[0], &O[0]);
   
  for (int i = 0; i < 32; ++i)
    O[i] = and_gate(EN, O[i]);
  
}
void adder1(BIT A, BIT B, BIT CarryIn, BIT* CarryOut, BIT* Sum)
{
  // TODO: implement a 1-bit adder
  // Note: you can copy+paste this from your or my Lab 5
  
  BIT x0 = xor_gate(A, B);
  *Sum = xor_gate(CarryIn, x0);
  
  BIT y0 = and_gate(x0, CarryIn);
  BIT y1 = and_gate(A, B);
  *CarryOut = or_gate(y0, y1);
}

void adder32(BIT* A, BIT* B, BIT CarryIn, BIT* Result){
  BIT CarryOut = FALSE;
  adder1(A[0], B[0], CarryIn, &CarryOut, &Result[0]);
  for (int i = 1; i < 32; ++i) {
    adder1(A[i], B[i], CarryOut, &CarryOut, &Result[i]);
  }
}

BIT multiplexor2(BIT S, BIT I0, BIT I1)
{
  BIT nS = not_gate(S);
  BIT x0 = and_gate(nS, I0);
  BIT x1 = and_gate(S,  I1);
  return or_gate(x0, x1);
}

void multiplexor2_32(BIT S, BIT* I0, BIT* I1, BIT* Output)
{
  for (int i = 0; i < 32; ++i) {
    BIT nS = not_gate(S);
    BIT x0 = and_gate(nS, I0[i]);
    BIT x1 = and_gate(S, I1[i]);
    Output[i] = or_gate(x0, x1);
  }
}

BIT multiplexor4(BIT S0, BIT S1, BIT I0, BIT I1, BIT I2, BIT I3)
{
  BIT S[] = {S0, S1};
  BIT O[] = {I0,I1,I2,I3};
  decoder2(S,TRUE, O);
  
  BIT y0 = and_gate(O[0], I0);
  BIT y1 = and_gate(O[1], I1);
  BIT y2 = and_gate(O[2], I2);
  BIT y3 = and_gate(O[3], I3);
  
  BIT z0 = or_gate(y0, y1);
  BIT z1 = or_gate(y2, y3);
  
  return or_gate(z0, z1);
}


/******************************************************************************/
/* Helper functions */
/******************************************************************************/
void copy_bits(BIT* A, BIT* B)
{
  for (int i = 0; i < 32; ++i)
    B[i] = A[i]; 
}

void print_binary(BIT* A)
{
  for (int i = 31; i >= 0; --i)
    printf("%d", A[i]); 
}

void convert_to_binary_char(int a, char* A, int length)
{
  if (a >= 0) {
    for (int i = 0; i < length; ++i) {
      A[i] = (a % 2 == 1 ? '1' : '0');
      a /= 2;
    }
  } else {
    a += 1;
    for (int i = 0; i < length; ++i) {
      A[i] = (a % 2 == 0 ? '1' : '0');
      a /= 2;
    }
  }
}

void convert_to_binary(int a, BIT* A)
{
  // TODO: convert integer to 2's complement BIT representation
  // Note: A[0] is least significant bit and A[31] is most significant bit
  // For this function ONLY: you're allowed to use logical if-else statements
  
  if (a >= 0) {
    for (int i = 0; i < 32; ++i) {
      A[i] = (a % 2 == 1);
      a /= 2;
    }
  } else {
    a += 1;
    for (int i = 0; i < 32; ++i) {
      A[i] = (a % 2 == 0);
      a /= 2;
    }
  }
}
  
int binary_to_integer(BIT* A)
{
  unsigned a = 0;
  unsigned mult = 1;
  
  for (int i = 0; i < 32; ++i) {
    a += A[i]*mult;
    mult *= 2;
  }
  
  return (int)a;
}

void set_register(char* input, char* output) 
{
  if(strcmp(input, "zero") == 0) {
    strncpy(output, "00000", 5);
  }
  else if(strcmp(input, "v0") == 0) {
    strncpy(output, "01000", 5);
  }
  else if(strcmp(input, "a0") == 0) {
    strncpy(output, "00100", 5);
  }
  if (strcmp(input, "t0") == 0) {
    strncpy(output, "00010", 5);//00010
  }
  else if(strcmp(input, "t1") == 0) {
    strncpy(output, "10010", 5);
  }
  else if (strcmp(input, "s0") == 0) {
    strncpy(output, "00001", 5);
  }
  else if(strcmp(input, "s1") == 0) {
    strncpy(output, "10001", 5);
  }
  else if(strcmp(input, "sp") == 0) {
    strncpy(output, "10111", 5);
  }
  else if(strcmp(input, "ra") == 0) {
    strncpy(output, "11111", 5);
  }
}

/******************************************************************************/
/* Parsing functions */
/******************************************************************************/


int get_instructions(BIT Instructions[][32])
{
  char line[256] = {0};
  int instruction_count = 0;
  while (fgets(line, 256, stdin) != NULL) {        
    BIT t_output[32] = {FALSE};
    char inst[256] = {0};
    char op1[256] = {0};
    char op2[256] = {0};
    char op3[256] = {0};

    sscanf(line, "%s %s %s %s", inst, op1, op2, op3); //instructions of length 4
    char output[33] = {0}; 
    char rs[6] = {0}; //1
    char rt[6] = {0}; //2 
    char rd[6] = {0}; //3
    char imm[17] = {0};
    char address[27] = {0};

    if(strcmp(inst, "lw") == 0) { //I-type
      convert_to_binary_char(atoi(op3), imm, 16);
      set_register(op1, rt);
      set_register(op2, rs);
      strncpy(&output[0], imm, 16);
      strncpy(&output[16], rt, 5);
      strncpy(&output[21], rs, 5);
      strncpy(&output[26], "110001", 6);      
    } else if(strcmp(inst, "sw") == 0) { //I-type
      set_register(op1, rt);
      set_register(op2, rs);    
      convert_to_binary_char(atoi(op3), imm, 16);
      strncpy(&output[0], imm, 16); 
      strncpy(&output[16], rt, 5);
      strncpy(&output[21], rs, 5);
      strncpy(&output[26], "110101", 6);
    } else if(strcmp(inst, "beq") == 0) { //I-type
      convert_to_binary_char(atoi(op3), imm, 16);
      set_register(op1, rt);
      set_register(op2, rs);
      strncpy(&output[0], imm, 16);
      strncpy(&output[16], rs, 5);
      strncpy(&output[21], rt, 5); 
      strncpy(&output[26], "001000", 6);
    } else if(strcmp(inst, "addi") == 0) { //I-type
      convert_to_binary_char(atoi(op3), imm, 16);
      set_register(op1, rt);
      set_register(op2, rs);
      strncpy(&output[0], imm, 16);
      strncpy(&output[16], rt, 5);
      strncpy(&output[21], rs, 5); 
      strncpy(&output[26], "000100", 6);
    } else if(strcmp(inst, "and") == 0) { //R-type
      set_register(op1, rd);
      set_register(op2, rs);
      set_register(op3, rt);
      strncpy(&output[0], "001001", 6);
      strncpy(&output[6], "00000", 5);
      strncpy(&output[11], rd, 5);
      strncpy(&output[16], rt, 5);
      strncpy(&output[21], rs, 5);
      strncpy(&output[26], "000000", 6);  
    } else if(strcmp(inst, "or") == 0) { //R-type 
      set_register(op1, rd);
      set_register(op2, rs);
      set_register(op3, rt);
      strncpy(&output[0], "101001", 6);
      strncpy(&output[6], "00000", 5);
      strncpy(&output[11], rd, 5);
      strncpy(&output[16], rt, 5);
      strncpy(&output[21], rs, 5);
      strncpy(&output[26], "000000", 6); 
    } else if(strcmp(inst, "add") == 0) { // R-type
      set_register(op1, rd);
      set_register(op2, rs);
      set_register(op3, rt);
      strncpy(&output[0], "000001", 6);
      strncpy(&output[6], "00000", 5);
      strncpy(&output[11], rd, 5);
      strncpy(&output[16], rt, 5);
      strncpy(&output[21], rs, 5);
      strncpy(&output[26], "000000", 6);     
    } else if(strcmp(inst, "sub") == 0) { // R-type
      set_register(op1, rd);
      set_register(op2, rs);
      set_register(op3, rt);
      strncpy(&output[0], "010001", 6);
      strncpy(&output[6], "00000", 5);
      strncpy(&output[11], rd, 5);
      strncpy(&output[16], rt, 5);
      strncpy(&output[21], rs, 5);
      strncpy(&output[26], "000000", 6); 
    } else if(strcmp(inst, "slt") == 0) { //R-type 
      set_register(op1, rd);
      set_register(op2, rs);
      set_register(op3, rt);
      strncpy(&output[0], "010101", 6);
      strncpy(&output[6], "00000", 5);
      strncpy(&output[11], rd, 5);
      strncpy(&output[16], rt, 5);
      strncpy(&output[21], rs, 5);
      strncpy(&output[26], "000000", 6); 
    } else if (strcmp(inst, "j") == 0) { //J-type
      convert_to_binary_char(atoi(op1), address, 26);
      strncpy(&output[0], address, 26);
      strncpy(&output[26], "010000", 6);      
    } else if(strcmp(inst, "jal") == 0) {  //J-type
      convert_to_binary_char(atoi(op1), address, 26);
      strncpy(&output[0], address, 26);
      strncpy(&output[26], "110000", 6);  
    } else if(strcmp(inst, "jr") == 0) { //R-type
      set_register(op1, rs); 
      strncpy(&output[0], "000100", 6);
      strncpy(&output[6], "00000", 5);
      strncpy(&output[11], "000000000000000", 15);
      strncpy(&output[21], rs, 5);
      strncpy(&output[26], "000000", 6);
    } 
    // Convert 'output' char array to 't_output' BIT array
    for (int i = 0; i < 32; i++) {
      t_output[i] = (output[i] == '1') ? TRUE : FALSE;
    }
    // Copy t_output to Instructions at the current instruction_count index
    memcpy(Instructions[instruction_count], t_output, sizeof(BIT) * 32);

    instruction_count++;
  }
  return instruction_count;
}


/******************************************************************************/
/* Program state - memory spaces, PC, and control */
/******************************************************************************/
BIT PC[32]                  = {FALSE};
BIT MEM_Instruction[32][32] = {FALSE};
BIT MEM_Data[32][32]        = {FALSE};
BIT MEM_Register[32][32]    = {FALSE};


BIT RegDst    = FALSE;
BIT Jump      = FALSE;
BIT Branch    = FALSE;
BIT MemToReg  = FALSE;
BIT ALUOp[2]  = {FALSE};
BIT MemWrite  = FALSE;
BIT ALUImm    = FALSE;
BIT RegWrite  = FALSE;
BIT Zero      = FALSE;
BIT Link      = FALSE;
BIT JumpReg   = FALSE;
BIT ALUControl[4] = {FALSE};

void print_instruction()
{
  unsigned pc = binary_to_integer(PC);
  printf("PC: %d\n", pc);
  printf("Instruction: ");
  print_binary(MEM_Instruction[pc]);
  printf("\n");
}

void print_state()
{
  printf("Data: ");
  for (int i = 0; i < 32; ++i) {
    printf("%d ", binary_to_integer(MEM_Data[i]));
  } 
  printf("\n");  
  
  printf("Register: ");
  for (int i = 0; i < 32; ++i) {
    printf("%d ", binary_to_integer(MEM_Register[i]));
  } 
  printf("\n");
}


/******************************************************************************/
/* Functions that you will implement */
/******************************************************************************/
void Instruction_Memory(BIT* ReadAddress, BIT* Instruction)
{
  
  BIT Result1[32] = {FALSE};
  decoder5(ReadAddress, TRUE, Result1);
  for (int i = 0; i < 32; i++){
      multiplexor2_32(Result1[i], Instruction, MEM_Instruction[i], Instruction);

  }
  
}


void Control(BIT* OpCode) {

  MemToReg = MemToRegCircuit(OpCode);
  MemWrite = MemWriteCircuit(OpCode);
  Branch   = BranchCircuit(OpCode);
  RegDst   = RegDstCircuit(OpCode);
  Link     = LinkCircuit(OpCode);
  RegWrite = or_gate3(RegDst, Link, or_gate(MemToReg, AddiCircuit(OpCode)));
  Jump     = or_gate(JumpCircuit(OpCode), Link);
  ALUOp[0] = or_gate3(AddiCircuit(OpCode), MemToReg, MemWrite);
  ALUOp[1] = Branch;
  ALUImm   = ALUOp[0];
}

void Read_Register(BIT* ReadRegister1, BIT* ReadRegister2,
  BIT* ReadData1, BIT* ReadData2)
{
  BIT Result1[32] = {FALSE};
  decoder5(ReadRegister1, TRUE, Result1);
  for (int i = 0; i < 32; i++){
      multiplexor2_32(Result1[i], ReadData1,MEM_Register[i],ReadData1);

  }

  BIT Result2[32] = {FALSE};
  decoder5(ReadRegister2, TRUE, Result2);
  for (int i = 0; i < 32; i++){
    multiplexor2_32(Result2[i], ReadData2,MEM_Register[i],ReadData2);
  }
  
}

void Write_Register(BIT RegWrite, BIT* WriteRegister, BIT* WriteData)
{
  BIT Result[32] = {FALSE};
  decoder5(WriteRegister, RegWrite, Result);
  for (int i = 0; i < 32; i++){
    multiplexor2_32(Result[i], MEM_Register[i],WriteData,MEM_Register[i]);
  }
}

void updateAluControl(){
    //ALUOp != {0,0} we have to update the alu contorl bits
    BIT ADD[] = {0, 0, 1, 0};//FLIPPED
    BIT SUB[] = {0, 1, 1, 0};
    for(int i=0; i<4; i++)
        ALUControl[i] = multiplexor2(ALUOp[0], ALUControl[i], ADD[i]);
    for(int i=0; i<4; i++)
        ALUControl[i] = multiplexor2(ALUOp[1], ALUControl[i], SUB[i]);
}

void ALU_Control(BIT* funct)
{

  JumpReg  = and_gate3(JumpRegCircuit(funct),   \
          not_gate(or_gate(ALUOp[0], ALUOp[1])), RegDst);
  //Set RegWrite to 0 if we are preforming a jr instruction
  RegWrite = multiplexor2(JumpReg, RegWrite, 0);
  ALUControl[3] = AluControl_Circuit0(funct);
  ALUControl[2] = not_gate(or_gate(AluControl_Circuit1(funct), ALUControl[3]));
  ALUControl[3] = or_gate(AluControl_Circuit0(funct), LessCircuit(funct));
  ALUControl[1] = not_gate(or_gate(not_gate(ALUControl[2]), BinvertCircuit(funct)));
  ALUControl[0] = 0;
  updateAluControl();
}

void ALU1(BIT A, BIT B, BIT Ainvert, BIT Binvert, BIT CarryIn, BIT Less, 
BIT Op0, BIT Op1, BIT* Result, BIT* CarryOut, BIT* Set, BIT* Zero)
{
  
  BIT x0 = multiplexor2(Binvert, B, not_gate(B));
  BIT x1 = multiplexor2(Ainvert, A, not_gate(A));
  BIT y0 = and_gate(x1, x0);
  BIT y1 = or_gate(x1, x0);
  
  BIT y2 = FALSE;
  adder1(x1, x0, CarryIn, CarryOut, &y2); 
  *Set = y2;
  
  BIT y3 = Less; 
  
  *Result = multiplexor4(Op0, Op1, y0, y1, y2, y3);
  
  *Zero = or_gate(*Result, *Zero);
}

void ALU32(BIT* A, BIT* B, BIT Ainvert, BIT Binvert, BIT CarryIn, 
  BIT Op0, BIT Op1, BIT* Result, BIT* CarryOut, BIT* Zero)
{
  BIT Less = FALSE;
  BIT Set = FALSE;
  ALU1(A[0], B[0], Ainvert, Binvert, CarryIn, Less, 
    Op0, Op1, &Result[0], CarryOut, &Set, Zero);
  for (int i = 1; i < 32; ++i) {
    ALU1(A[i], B[i], Ainvert, Binvert, *CarryOut, Less, 
      Op0, Op1, &Result[i], CarryOut, &Set, Zero);
  }
  
  Less = Set;
  ALU1(A[0], B[0], Ainvert, Binvert, CarryIn, Less, 
    Op0, Op1, &Result[0], CarryOut, &Set, Zero);
  *Zero = not_gate(*Zero);
}

void ALU(BIT* ALUControl, BIT* Input1, BIT* Input2, BIT* Zero, BIT* Result)
{   
  BIT Carryout;

  ALU32(Input1,Input2,ALUControl[0], ALUControl[1], ALUControl[1], ALUControl[3],
    ALUControl[2], Result, &Carryout, Zero);
}

void Data_Memory(BIT MemWrite, BIT MemRead, 
  BIT* Address, BIT* WriteData, BIT* ReadData)
{
  BIT Result_read[32] = {FALSE};
  decoder5(Address, MemRead, Result_read);
  for (int i = 0; i < 32; i++){
    multiplexor2_32(Result_read[i], ReadData,MEM_Data[i],ReadData);
  }

  BIT Result[32] = {FALSE};
  decoder5(Address, MemWrite, Result);
  for (int i = 0; i < 32; i++){
    multiplexor2_32(Result[i], MEM_Data[i],WriteData,MEM_Data[i]);
  }

}

void Extend_Sign16(BIT* Input, BIT* Output)
{
  for(int i = 0; i < 16; i++){
    Output[i] = Input[i];
  }
  for(int i = 16; i < 32; i++){
    Output[i] = Input[15];
  }
  
}

void updateState()
{

  //fetching from instruction
  BIT instruction[32] = {FALSE};
  Instruction_Memory(PC, instruction);


  BIT control_instruction[6];
  int c = 0;
  for (int i = 26; i < 32; i++){
    control_instruction[c] = instruction[i];
    c++;
  }

  //set control bits
  Control(control_instruction);

  //read from register file
  c = 0;
  BIT reg_bits1[5];
  for (int i = 21; i < 26; i++){
    reg_bits1[c] = instruction[i];
    c++;
  }
  c = 0;
  BIT reg_bits2[5];
  for (int i = 16; i < 21; i++){
    reg_bits2[c] = instruction[i];
    c++;
  }
  BIT data1[32] = {FALSE};
  BIT data2[32] = {FALSE};

  Read_Register(reg_bits1, reg_bits2, data1, data2);


  //ALU process
  ALU_Control(instruction);
  BIT extend[32] = {FALSE};
  Extend_Sign16(instruction, extend);
  BIT option[32] = {FALSE};
  multiplexor2_32(ALUImm, data2, extend, option);
  BIT zero = FALSE;
  BIT res[32] = {FALSE};

  ALU(ALUControl, data1, option, &zero, res);
  BIT res2[32] = {FALSE};
  Data_Memory(MemWrite, TRUE , res, data2, res2);
  BIT write_reg[32] = {FALSE};
  BIT write_dat[32] = {FALSE};


  
  BIT sum[32] = {FALSE};
  adder32(PC, ONE, FALSE, sum);
  BIT sum2[32] = {FALSE};
  adder32(sum, extend, FALSE, sum2);

  BIT zero_branch = and_gate(Branch, zero);
  BIT sum3[32];
  multiplexor2_32(zero_branch, sum, sum2, sum3);

  BIT extend_and_reg1[32];
  multiplexor2_32(JumpReg, extend, data1, extend_and_reg1);
  BIT or_sel = or_gate(Jump, JumpReg);

  BIT final_pc[32];


  multiplexor2_32(or_sel, sum3, extend_and_reg1, final_pc);

  c = 0;
  BIT reg_bits3[5];
  for (int i = 11; i < 16; i++){
    reg_bits3[c] = instruction[i];
    c++;
  }

  //multiplexor2_32(RegDst, reg_bits2, reg_bits3, write_reg);
  for(int i=0; i<6; i++){
      write_reg[i] = multiplexor2(RegDst, reg_bits2[i], reg_bits3[i]);
  }
  BIT write_reg_final[32];
  multiplexor2_32(Link, write_reg, REG_THIRTY_ONE, write_reg_final);
  multiplexor2_32(MemToReg, res, res2,write_dat);
  BIT write_dat_final[32];
  multiplexor2_32(Link, write_dat, sum, write_dat_final);
  Write_Register(RegWrite, write_reg_final, write_dat_final);
  
  copy_bits(final_pc, PC);

}


/******************************************************************************/
/* Main */
/******************************************************************************/

int main()
{
  setbuf(stdout, NULL);
    
  // parse instructions into binary format
  int counter = get_instructions(MEM_Instruction);
  
  // load program and run
  copy_bits(ZERO, PC);
  copy_bits(THIRTY_TWO, MEM_Register[29]);
  
  while (binary_to_integer(PC) < counter) {
    print_instruction();
    updateState();
    print_state();
  }

  return 0;
}
