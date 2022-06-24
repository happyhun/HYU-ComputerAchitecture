#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define NUMMEMORY 65536 /* maximum number of data words in memory */
#define NUMREGS 8 /* number of machine registers */
#define MAXLINELENGTH 1000

#define ADD 0
#define NOR 1
#define LW 2
#define SW 3
#define BEQ 4
#define JALR 5 /* JALR will not implemented for this project */
#define HALT 6
#define NOOP 7

#define NOOPINSTRUCTION 0x1c00000

typedef struct IFIDStruct {
	int instr;
	int pcPlus1;
} IFIDType;

typedef struct IDEXStruct {
	int instr;
	int pcPlus1;
	int readRegA;
	int readRegB;
	int offset;
} IDEXType;

typedef struct EXMEMStruct {
	int instr;
	int branchTarget;
	int aluResult;
	int readRegB;
} EXMEMType;

typedef struct MEMWBStruct {
	int instr;
	int writeData;
} MEMWBType;

typedef struct WBENDStruct {
	int instr;
	int writeData;
} WBENDType;

typedef struct stateStruct {
	int pc;
	int instrMem[NUMMEMORY];
	int dataMem[NUMMEMORY];
	int reg[NUMREGS];
	int numMemory;
	IFIDType IFID;
	IDEXType IDEX;
	EXMEMType EXMEM;
	MEMWBType MEMWB;
	WBENDType WBEND;
	int cycles; /* number of cycles run so far */
} stateType;

int main(int argc, char *argv[])
{
    char line[MAXLINELENGTH];
    stateType state;
    stateType *statePtr = &state;
    stateType newState;
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

    
    /* read in the entire machine-code file into BOTH instrMem and dataMem */
    for (state.numMemory = 0; fgets(line, MAXLINELENGTH, filePtr) != NULL; state.numMemory++) {
        if (sscanf(line, "%d", state.instrMem+state.numMemory) != 1) {
            printf("error in reading address %d\n", state.numMemory);
            exit(1);
        }
        state.dataMem[state.numMemory] = state.instrMem[state.numMemory];
        printf("memory[%d]=%d\n", state.numMemory, state.instrMem[state.numMemory]);
    }
    printf("%d memory words\n", state.numMemory);
    printf("\tinstruction memory:\n");

    for(int i=0; i<state.numMemory; i++) {
        printf("\t\tinstrMem[ %d ] ", i);
        printInstruction(state.instrMem[i]);
    }

    // reg 0으로 초기화
    for (int i=0; i<NUMREGS; i++) {
        state.reg[i] = 0;
    }

    // pc 와 cycle 0으로 초기화
    state.pc = 0;
    state.cycles = 0;

    // pipeline register 의 instruction 전부 NOOP으로 초기화
    state.IFID.instr = NOOPINSTRUCTION;
    state.IDEX.instr = NOOPINSTRUCTION;
    state.EXMEM.instr = NOOPINSTRUCTION;
    state.MEMWB.instr = NOOPINSTRUCTION;
    state.WBEND.instr = NOOPINSTRUCTION;




	
    while (1) {

	printState(&state);

	/* check for halt */
	if (opcode(state.MEMWB.instr) == HALT) {
		printf("machine halted\n");
		printf("total of %d cycles executed\n", state.cycles);
		exit(0);
	}

	newState = state;
	newState.cycles++;

	/* --------------------- IF stage --------------------- */
	newState.IFID.instr = state.instrMem[state.pc];
	newState.IFID.pcPlus1 = state.pc + 1;
	newState.pc ++;

	/* --------------------- ID stage --------------------- */
	newState.IDEX.instr = state.IFID.instr;
	newState.IDEX.pcPlus1 = state.IFID.pcPlus1;
	newState.IDEX.readRegA = state.reg[field0(newState.IDEX.instr)];
	newState.IDEX.readRegB = state.reg[field1(newState.IDEX.instr)];
	newState.IDEX.offset = field2(newState.IDEX.instr);
	if(newState.IDEX.offset > 32767) {
		newState.IDEX.offset |= 0xffff0000;
	}

	/* --------------------- EX stage --------------------- */
	newState.EXMEM.instr = state.IDEX.instr;
	newState.EXMEM.branchTarget = state.IDEX.pcPlus1 + state.IDEX.offset;
	if ((opcode(state.IDEX.instr) != HALT) && (opcode(state.IDEX.instr) != NOOP))
		newState.EXMEM.aluResult = execution(newState.EXMEM.instr, state.IDEX.readRegA, state.IDEX.readRegB, state.IDEX.offset);
	if ((opcode(state.IDEX.instr) == BEQ) && (newState.EXMEM.aluResult) == 0)
		newState.pc = newState.EXMEM.branchTarget;
	newState.EXMEM.readRegB = state.IDEX.readRegB;

	/* --------------------- MEM stage --------------------- */
	newState.MEMWB.instr = state.EXMEM.instr;
	if((opcode(newState.MEMWB.instr) == ADD) || (opcode(newState.MEMWB.instr) == NOR)) {
		newState.MEMWB.writeData = state.EXMEM.aluResult;
	}
	else if(opcode(newState.MEMWB.instr) == SW) {
		newState.dataMem[state.EXMEM.aluResult] = state.EXMEM.readRegB;
	}
	else if(opcode(newState.MEMWB.instr) == LW) {
		newState.MEMWB.writeData = state.dataMem[state.EXMEM.aluResult];
	}
	
	/* --------------------- WB stage --------------------- */
	newState.WBEND.instr = state.MEMWB.instr;
	if((opcode(newState.WBEND.instr) == ADD) || (opcode(newState.WBEND.instr) == NOR)) {
		newState.WBEND.writeData = state.MEMWB.writeData;
		newState.reg[field2(newState.WBEND.instr)] = newState.WBEND.writeData;
	}
	else if(opcode(newState.WBEND.instr) == LW) {
		newState.WBEND.writeData = state.MEMWB.writeData;
		newState.reg[field1(newState.WBEND.instr)] = newState.WBEND.writeData;
	}
	state = newState; /* this is the last statement before end of the loop.
			It marks the end of the cycle and updates the
			current state with the values calculated in this
			cycle */
	
	}

	return 0;
}

void
printState(stateType *statePtr)
{
    int i;
    printf("\n@@@\nstate before cycle %d starts\n", statePtr->cycles);
    printf("\tpc %d\n", statePtr->pc);

    printf("\tdata memory:\n");
	for (i=0; i<statePtr->numMemory; i++) {
	    printf("\t\tdataMem[ %d ] %d\n", i, statePtr->dataMem[i]);
	}
    printf("\tregisters:\n");
	for (i=0; i<NUMREGS; i++) {
	    printf("\t\treg[ %d ] %d\n", i, statePtr->reg[i]);
	}
    printf("\tIFID:\n");
	printf("\t\tinstruction ");
	printInstruction(statePtr->IFID.instr);
	printf("\t\tpcPlus1 %d\n", statePtr->IFID.pcPlus1);
    printf("\tIDEX:\n");
	printf("\t\tinstruction ");
	printInstruction(statePtr->IDEX.instr);
	printf("\t\tpcPlus1 %d\n", statePtr->IDEX.pcPlus1);
	printf("\t\treadRegA %d\n", statePtr->IDEX.readRegA);
	printf("\t\treadRegB %d\n", statePtr->IDEX.readRegB);
	printf("\t\toffset %d\n", statePtr->IDEX.offset);
    printf("\tEXMEM:\n");
	printf("\t\tinstruction ");
	printInstruction(statePtr->EXMEM.instr);
	printf("\t\tbranchTarget %d\n", statePtr->EXMEM.branchTarget);
	printf("\t\taluResult %d\n", statePtr->EXMEM.aluResult);
	printf("\t\treadRegB %d\n", statePtr->EXMEM.readRegB);
    printf("\tMEMWB:\n");
	printf("\t\tinstruction ");
	printInstruction(statePtr->MEMWB.instr);
	printf("\t\twriteData %d\n", statePtr->MEMWB.writeData);
    printf("\tWBEND:\n");
	printf("\t\tinstruction ");
	printInstruction(statePtr->WBEND.instr);
	printf("\t\twriteData %d\n", statePtr->WBEND.writeData);
}

int
field0(int instruction)
{
	return( (instruction>>19) & 0x7);
}

int
field1(int instruction)
{
	return( (instruction>>16) & 0x7);
}

int
field2(int instruction)
{
	return(instruction & 0xFFFF);
}

int
opcode(int instruction)
{
	return(instruction>>22);
}

void
printInstruction(int instr)
{
	
	char opcodeString[10];
	
	if (opcode(instr) == ADD) {
		strcpy(opcodeString, "add");
	} else if (opcode(instr) == NOR) {
		strcpy(opcodeString, "nor");
	} else if (opcode(instr) == LW) {
		strcpy(opcodeString, "lw");
	} else if (opcode(instr) == SW) {
		strcpy(opcodeString, "sw");
	} else if (opcode(instr) == BEQ) {
		strcpy(opcodeString, "beq");
	} else if (opcode(instr) == JALR) {
		strcpy(opcodeString, "jalr");
	} else if (opcode(instr) == HALT) {
		strcpy(opcodeString, "halt");
	} else if (opcode(instr) == NOOP) {
		strcpy(opcodeString, "noop");
	} else {
		strcpy(opcodeString, "data");
    }
    printf("%s %d %d %d\n", opcodeString, field0(instr), field1(instr),
		field2(instr));
}

int execution(int instr, int regA, int regB, int offset)
{
    // opcode에 따라 다른 format 명령 실행
    if ((opcode(instr) == ADD) || (opcode(instr) == NOR))
    {
        return R_format(instr, regA, regB, offset);
    }
    else
    {
        return I_format(instr, regA, regB, offset);
    }
}

int R_format(int instr, int regA, int regB, int offset)
{
	int result;
    if (opcode(instr) == ADD) {
		result = regA + regB;
	}
    else {  // nor 수행
        result = ~(regA | regB);
    }

	return result;
}

int I_format(int instr, int regA, int regB, int offset)
{
	int result;
    if ((opcode(instr) == LW) | (opcode(instr) == SW)) {  
        result = regA + offset;
	}
    else {	// beq
		result = regA - regB;
	}

	return result;

}