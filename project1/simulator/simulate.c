/* LC-2K Instruction-level simulator */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define NUMMEMORY 65536 /* maximum number of words in memory */
#define NUMREGS 8       /* number of machine registers */
#define MAXLINELENGTH 1000
typedef struct stateStruct
{
    int pc;
    int mem[NUMMEMORY];
    int reg[NUMREGS];
    int numMemory;
} stateType;

void printState(stateType *);
int convertNum(int num);
void parseInstFromMem(stateType*, int*,int*,int*,int*);
int negbinary(int);

int main(int argc, char *argv[])
{
    int totalInst = 0;
    char line[MAXLINELENGTH];
    stateType state;
    FILE *filePtr;

    if (argc != 2)
    {
        printf("error: usage: %s <machine-code file>\n", argv[0]);
        exit(1);
    }

    filePtr = fopen(argv[1], "r");
    if (filePtr == NULL)
    {
        printf("error: can't open file %s", argv[1]);
        perror("fopen");
        exit(1);
    }

    /* read in the entire machine-code file into memory */
    for (state.numMemory = 0; fgets(line, MAXLINELENGTH, filePtr) != NULL;
         state.numMemory++)
    {

        if (sscanf(line, "%d", state.mem + state.numMemory) != 1)
        {
            printf("error in reading address %d\n", state.numMemory);
            exit(1);
        }
        printf("memory[%d]=%d\n", state.numMemory, state.mem[state.numMemory]);
    }
    printState(&state);

    while (1)
    {
        int opcode, arg0, arg1, arg2;
        parseInstFromMem(&state, &opcode, &arg0, &arg1, &arg2);
        totalInst++;
        state.pc++;
        if(opcode == 0)//add
        {
            state.reg[arg2] = state.reg[arg0] + state.reg[arg1];
        }
        else if(opcode == 1)//nor
        {
             state.reg[arg2] = ~(state.reg[arg0] | state.reg[arg1]);
        }
        else if(opcode == 2)//lw
        {
            state.reg[arg1] = state.mem[state.reg[arg0]+arg2];
        }
        else if(opcode == 3)//sw
        {
            state.mem[arg1] = state.mem[state.reg[arg0]+arg2];
        }
        else if(opcode == 4)//beq
        {
            if(state.reg[arg0] == state.reg[arg1])
                state.pc = state.pc + arg2; 
        }
        else if(opcode == 5)//jalr
        {
            state.reg[arg1] = state.pc;
            state.pc = state.reg[arg0];
        }
        else if(opcode == 6)//halt
        {
            printf("machine halted\ntotal of %d instructions executed\n",totalInst);
            printf("final state of machine:\n");
            printState(&state);
            break;
        }
        printState(&state);
    }

    return (0);
}

void printState(stateType *statePtr)
{
    int i;
    printf("\n@@@\nstate:\n");
    printf("\tpc %d\n", statePtr->pc);
    printf("\tmemory:\n");
    for (i = 0; i < statePtr->numMemory; i++)
    {
        printf("\t\tmem[ %d ] %d\n", i, statePtr->mem[i]);
    }
    printf("\tregisters:\n");
    for (i = 0; i < NUMREGS; i++)
    {
        printf("\t\treg[ %d ] %d\n", i, statePtr->reg[i]);
    }
    printf("end state\n");
}

int convertNum(int num)
{
    /* convert a 16-bit number into a 32-bit Linux integer */
    if (num & (1 << 15))
    {
        num -= (1 << 16);
    }
    return (num);
}

int negbinary(int num)
{
	int i;
	int bin[16];
	int result = 0;

	for (i = 15; i >= 0; i--)
	{
		bin[i] = 0;
	}
	i = 0;
    num = num - 1;
	while (num > 0)
	{
		bin[i] = num % 2;
		num /= 2;
		i++;
	}
	for (i = 15; i >= 0; i--)
	{
		if (bin[i] == 0)
			bin[i] = 1;
		else
			bin[i] = 0;
	}
	i = 15;
	while (i != -1)
	{
		result = (result * 2) + bin[i];
		i--;
	}
    result = -1 * result;
	return result;
}

void parseInstFromMem(stateType *statePtr, int *opcode, int *arg0, int *arg1, int *arg2)
{
    int memValue = statePtr->mem[statePtr->pc];

    *opcode = (memValue >> 22) & 0b111;
    *arg0 = (memValue >> 19) & 0b111;
    *arg1 = (memValue >> 16) & 0b111;
    *arg2 = (memValue & 0xFFFF);
    if(*arg2>=32768)
    {
        *arg2 = negbinary(*arg2);
    }
}
