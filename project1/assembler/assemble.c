#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define MAXLINELENGTH 1000

typedef struct _SymTab {
    char symbol[100][7];
    int address[100];
    int idx;
} SymTab;   // symbol address table

SymTab symtab;

int readAndParse(FILE *, char *, char *, char *, char* ,char *);
int isNumber(char *);
int convToNum(char *);
int R_format(char *, char *, char *, char *);
int I_format(char *, char *, char *, char *, int);
int J_format(char *, char *);
int O_format(char *);

int main(int argc, char *argv[])
{
    char *inFileString, *outFileString;
    FILE *inFilePtr, *outFilePtr;
    char label[MAXLINELENGTH], opcode[MAXLINELENGTH], arg0[MAXLINELENGTH],
            arg1[MAXLINELENGTH], arg2[MAXLINELENGTH];
    
    if (argc != 3) {
        printf("error: usage: %s <assembly-code-file> <machine-code-file>\n",
            argv[0]);
        exit(1);
    }

    inFileString = argv[1];
    outFileString = argv[2];

    inFilePtr = fopen(inFileString, "r");
    if (inFilePtr == NULL) {
        printf("error in opening %s\n", inFileString);
        exit(1);
    }

    outFilePtr = fopen(outFileString, "w");
    if (outFilePtr == NULL) {
        printf("error in opening %s\n", outFileString);
        exit(1);
    }

    // pass 1

    symtab.idx = 0;  // symbol table의 index 초기화
    int adr1 = 0; // instruction의 주소 초기화

    while(readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2)) {
        if(label[0] != '\0') {  // label이 존재하면 중복검사, symbtab에 저장
            if(symtab.idx > 0) {
                for(int i = 0; i < symtab.idx; i ++) {
                    if(!strcmp(label, symtab.symbol[i])) {  // 중복이면 error
                        printf("error: duplicate label\n");
                        exit(1);
                    }     
                }
            }
            strcpy(symtab.symbol[symtab.idx], label);
            symtab.address[symtab.idx] = adr1;
            symtab.idx++;   // 최종 idx는 실제 인덱스 + 1
        }
        adr1++;
    }

    rewind(inFilePtr);
    
    // pass2

    int adr2 = 0; // instruction의 주소 초기화
    
    for(int i=0; i < adr1; i++) {   // 한 줄 씩 읽고 machine code로 변환
        readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2);

        int machine_code;

        if(!strcmp(opcode, "add") || !strcmp(opcode, "nor"))
            machine_code = R_format(opcode, arg0, arg1, arg2);
        else if(!strcmp(opcode, "lw") || !strcmp(opcode, "sw") || !strcmp(opcode, "beq"))
            machine_code = I_format(opcode, arg0, arg1, arg2, adr2);
        else if(!strcmp(opcode, "jalr"))
            machine_code = J_format(arg0, arg1);
        else if(!strcmp(opcode, "halt") || !strcmp(opcode, "noop"))
            machine_code = O_format(opcode);
        else if(!strcmp(opcode, ".fill"))
            machine_code = convToNum(arg0);
        else {  // 알 수 없는 opcode error
            printf("error: unrecognized opcode\n");
            exit(1);
        }
        fprintf(outFilePtr, "%d", machine_code);    // mc파일 쓰기
        if(i != adr1 -1)
            fprintf(outFilePtr, "\n");
        adr2++;
    }
    exit(0);
}

int readAndParse(FILE *inFilePtr, char *label, char *opcode, char *arg0,
        char *arg1, char *arg2)
{
    char line[MAXLINELENGTH];
    char *ptr = line;
    /* delete prior values */
    label[0] = opcode[0] = arg0[0] = arg1[0] = arg2[0] = '\0';
    /* read the line from the assembly-language file */
    if (fgets(line, MAXLINELENGTH, inFilePtr) == NULL)
    {
        /* reached end of file */
        return(0);
    }
    /* check for line too long (by looking for a \n) */
    if (strchr(line, '\n') == NULL)
    {
        /* line too long */
        printf("error: line too long\n");
        exit(1);
    }
    /* is there a label? */
    ptr = line;
    if (sscanf(ptr, "%[^\t\n\r ]", label))
    {
        /* successfully read label; advance pointer over the label */
        ptr += strlen(label);
    }
    /*
     * Parse the rest of the line. Would be nice to have real regular
     * expressions, but scanf will suffice.
     */
    sscanf(ptr, "%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]",
    opcode, arg0, arg1, arg2);
    return(1);
}

int isNumber(char *string)
{
    /* return 1 if string is a number */
    int i;
    return( (sscanf(string, "%d", &i)) == 1);
}

int convToNum(char *arg)
{
    // 숫자면 atoi, 문자면 symbol address로 변환
    if(isNumber(arg))   
        return atoi(arg);
    else {
        for(int i = 0; i < symtab.idx; i++) {
            if(!strcmp(arg, symtab.symbol[i]))
                return symtab.address[i]; 
        }
        printf("error: use of undefined label\n");  // symbol address가 없는 symbol이면 error
        exit(1);
    }
}

int R_format(char *opcode, char *arg0, char *arg1, char *arg2)
{
    int opcode_b;

    if(!strcmp(opcode, "add"))
        opcode_b = 0;   // add: 000
    else
        opcode_b = 1;   // nor: 001
    
    int ans = 0;
    int regA = convToNum(arg0);
    int regB = convToNum(arg1);
    int destReg = convToNum(arg2);
    
    ans |= destReg;
    ans |= regB << 16;
    ans |= regA << 19;
    ans |= opcode_b << 22;

    return ans;
}

int I_format(char *opcode, char *arg0, char *arg1, char *arg2, int adr)
{
    int opcode_b;
    int offSet = convToNum(arg2);
    if(offSet > 32767 || offSet < -32768) {
        printf("error: offsetFields that don't fit in 16 bits\n");
        exit(1);
    }
    if(!strcmp(opcode, "lw"))
        opcode_b = 2;   // lw: 010  
    else if(!strcmp(opcode, "sw"))
        opcode_b = 3;   // sw: 011
    else {
        opcode_b = 4;   // beq: 100
        if(!isNumber(arg2))
            offSet -= (adr + 1);
    }
    int ans = 0;
    int regA = convToNum(arg0);
    int regB = convToNum(arg1);
    
    if(offSet < 0)
        offSet ^= 0xffff0000;   // 32bit offSet을 16bit offSet으로 변경
    ans |= offSet;
    ans |= regB << 16;
    ans |= regA << 19;
    ans |= opcode_b << 22;

    return ans;
}

int J_format(char *arg0, char *arg1)
{
    int opcode_b = 5;   // jalr: 101
    int ans = 0;
    int regA = convToNum(arg0);
    int regB = convToNum(arg1);

    ans |= regB << 16;
    ans |= regA << 19;
    ans |= opcode_b << 22;

    return ans;
}

int O_format(char *opcode)
{
    int opcode_b;

    if(!strcmp(opcode, "halt"))
        opcode_b = 6;   // halt: 110
    else
        opcode_b = 7;   // noop: 111
    
    int ans = 0;
    ans |= opcode_b << 22;

    return ans;
}