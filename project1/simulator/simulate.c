/* LC-2K Instruction-level simulator */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define NUMMEMORY 65536 /* maximum number of words in memory*/
#define NUMREGS 8   /* number of machine registers */
#define MAXLINELENGTH 1000

typedef struct stateStruct {
    int pc;
    int mem[NUMMEMORY];
    int reg[NUMREGS];
    int numMemory;
} stateType;

void printState(stateType *);
int execution(stateType *, int);
void R_format(stateType *, int);
void I_format(stateType *, int);
void J_format(stateType *, int);
int O_format(stateType *, int, int);

int main(int argc, char *argv[])
{
    char line[MAXLINELENGTH];
    stateType state;
    stateType *statePtr = &state;
    FILE *filePtr;
    int count = 0;  // instruction 수행 횟수

    if (argc != 2) {
        printf("error: usage: %s <machine-code file>\n", argv[0]);
        exit(1);
    }

    filePtr = fopen(argv[1], "r");

    if (filePtr == NULL) {
        printf("error: can't open file %s", argv[1]);
        perror("fopen");
        exit(1);
    }

    /* read in the entire machine-code file into memory */
    for (state.numMemory = 0; fgets(line, MAXLINELENGTH, filePtr) != NULL; state.numMemory++) {
        if (sscanf(line, "%d", state.mem+state.numMemory) != 1) {
            printf("error in reading address %d\n", state.numMemory);
            exit(1);
        }
        printf("memory[%d]=%d\n", state.numMemory, state.mem[state.numMemory]);
    }
    
    // reg 0으로 초기화
    for (int i=0; i<NUMREGS; i++) {
        state.reg[i] = 0;
    }

    // pc 0으로 초기화
    state.pc = 0;

    // state 확인과 instruction 수행
    printState(statePtr);
    while(!execution(statePtr, count)) {    // excution함수의 return이 1이 나오면 종료
        count++;
        printState(statePtr);
    }
    printState(statePtr);
    
    return 0;
}

void printState(stateType *statePtr)
{
    int i;
    printf("\n@@@\nstate:\n");
    printf("\tpc %d\n", statePtr->pc);
    printf("\tmemory:\n");
    for(i=0; i<statePtr->numMemory; i++) {
        printf("\t\tmem[ %d ] %d\n", i, statePtr->mem[i]);
    }
    printf("\tregisters:\n");
    for(i=0; i<NUMREGS; i++) {
        printf("\t\treg[ %d ] %d\n", i, statePtr->reg[i]);
    }
    printf("end state\n");
}

int execution(stateType *statePtr, int count)
{
    int machine_code = statePtr->mem[statePtr->pc];
    int opcode = machine_code >> 22;

    // opcode에 따라 다른 format 명령 실행
    if ((opcode == 0) || (opcode == 1))
    {
        R_format(statePtr, machine_code);
    }
    else if ((opcode == 2) || (opcode == 3) || (opcode == 4))
    {
        I_format(statePtr, machine_code);
    }
    else if (opcode == 5)
    {
        J_format(statePtr, machine_code);
    }
    else
    {
        if (O_format(statePtr, machine_code, count) == 1)
            return 1;   // 종료 명령 들어오면 return 1
    }

    return 0;
}

void R_format(stateType *statePtr, int machine_code)
{
    int opcode = machine_code >> 22;
    int regA = machine_code >> 19;
    regA &= 0b111;
    int regB = machine_code >> 16;
    regB &= 0b111;
    int destReg = (machine_code & 0b111);

    if (opcode == 0) {  // add 수행
        statePtr->reg[destReg] = statePtr->reg[regA] + statePtr->reg[regB];
    }
    else {  // nor 수행
        statePtr->reg[destReg] = ~(statePtr->reg[regA] | statePtr->reg[regB]);
    }
    statePtr->pc++;
}

void I_format(stateType *statePtr, int machine_code)
{
    int opcode = machine_code >> 22;
    int regA = machine_code >> 19;
    regA &= 0b111;
    int regB = machine_code >> 16;
    regB &= 0b111;
    int offSet = (machine_code & 0xffff);
    if (offSet > 32767) {   // 16bit offSet이 음수이면, sign bit를 32bit으로 확장
        offSet |= 0xffff0000;
    }

    if (opcode == 2) {  // lw 수행
        statePtr->reg[regB] = statePtr->mem[statePtr->reg[regA] + offSet];
    }
    else if (opcode == 3) { // sw 수행
        statePtr->mem[statePtr->reg[regA] + offSet] = statePtr->reg[regB];
    }
    else if (statePtr->reg[regA] == statePtr->reg[regB]) {  // beq 수행
        statePtr->pc += 1+offSet;
        return;
    }
    statePtr->pc++;
}

void J_format(stateType *statePtr, int machine_code)
{
    // jalr 수행
    int opcode = machine_code >> 22;
    int regA = machine_code >> 19;
    regA &= 0b111;
    int regB = machine_code >> 16;
    regB &= 0b111;
    printf("opcode: %d, regA: %d, regB: %d\n", opcode, regA, regB);
    statePtr->reg[regB] = statePtr->pc + 1;
    statePtr->pc = statePtr->reg[regA];
}

int O_format(stateType *statePtr, int machine_code, int count)
{
    int opcode = machine_code >> 22;
    if (opcode == 6) {  // halt 수행
        statePtr->pc++;
        printf("machine halted\ntotal of %d instructions executed\nfinal state of machine:\n", count+1);
        return 1;
    }
    // noop 수행
    statePtr->pc++;
    return 0;
}
