/* Assembler code fragment for LC-2K */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#define MAXLINELENGTH 1000
#define BEQ 1
#define LABEL 0

char *add = "add";
char *nor = "nor";
char *lw = "lw";
char *sw = "sw";
char *beq = "beq";
char *jalr = "jalr";
char *halt = "halt";
char *noop = "noop";
char *fill = ".fill";

// label중복 확인을위한 인자
char **labelList;
int index1 = 0;
int index2 = 0;
// 새로운 label 추가할때만 labelList에 넣기위한 flag
int flag1 = 0;
int flag2 = 0;

int readAndParse(FILE *, char *, char *, char *, char *, char *);
int isNumber(char *);
int charToint(char *);
int findtext(FILE *, char *, int);
int negbinary(int, int);

char *inFileString, *outFileString, *findLable;
FILE *inFilePtr, *outFilePtr, *findLablePtr;
char label[MAXLINELENGTH], opcode[MAXLINELENGTH], arg0[MAXLINELENGTH],
	arg1[MAXLINELENGTH], arg2[MAXLINELENGTH];
int main(int argc, char *argv[])
{
	int result = 0;

	if (argc != 3)
	{
		printf("error: usage: %s <assembly-code-file> <machine-code-file>\n",
			   argv[0]);
		exit(1);
	}

	inFileString = argv[1];
	findLable = argv[1];
	outFileString = argv[2];
	labelList = malloc(sizeof(char*));

	inFilePtr = fopen(argv[1], "r");
	if (inFilePtr == NULL)
	{
		printf("error in opening %s\n", inFileString);
		exit(1);
	}
	findLablePtr = fopen(argv[1], "r");
	if (findLablePtr == NULL)
	{
		printf("error in opening %s\n", inFileString);
		exit(1);
	}
	outFilePtr = fopen(argv[2], "w");
	if (outFilePtr == NULL)
	{
		printf("error in opening %s\n", outFileString);
		exit(1);
	}

	/* here is an example for how to use readAndParse to read a line from
		 inFilePtr */
	char tmparg2[MAXLINELENGTH];
	while (readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2))
	{
		if (strcmp(opcode, add) == 0 || strcmp(opcode, nor) == 0 || strcmp(opcode, lw) == 0 || strcmp(opcode, sw) == 0 || strcmp(opcode, beq) == 0 || strcmp(opcode, jalr) == 0)
		{
			if (!isNumber(arg0) || !isNumber(arg1))
			{
				printf("Non-integer register arguments\n");
				exit(1);
			}
			int tmp1 = charToint(arg0);
			int tmp2 = charToint(arg1);

			if (tmp1 < 0 || tmp1 > 7)
			{
				printf("Registers outside the range [0, 7]\n");
				exit(1);
			}

			if (tmp2 < 0 || tmp2 > 7)
			{
				printf("Registers outside the range [0, 7]\n");
				exit(1);
			}
		}

		if (strcmp(opcode, add) == 0)
		{
			result = result + (charToint(arg0) << 19);
			result = result + (charToint(arg1) << 16);
			result = result + charToint(arg2);
			fprintf(outFilePtr, "%d\n", result);
		}
		else if (strcmp(opcode, nor) == 0)
		{
			result = result + pow(2, 22);
			result = result + (charToint(arg0) << 19);
			result = result + (charToint(arg1) << 16);
			result = result + charToint(arg2);
			fprintf(outFilePtr, "%d\n", result);
		}
		/*--------------------------------------------------------------------*/
		else if (strcmp(opcode, lw) == 0)
		{
			result = result + pow(2, 23);
			result = result + (charToint(arg0) << 19);
			result = result + (charToint(arg1) << 16);
			strcpy(tmparg2, arg2);
			if (isNumber(arg2))
			{
				int tmp = charToint(arg2);
				if (charToint(arg2) > 32767 || charToint(arg2) < -32768)
				{
					printf("offsetFields don’t fit in 16 bits\n");
					exit(1);
				}
				result = result + charToint(arg2);
			}

			else
			{
				int tmp = findtext(findLablePtr, tmparg2, LABEL);
				if (tmp == -1)
					exit(1);
				else
					result = result + tmp;
			}
			fprintf(outFilePtr, "%d\n", result);
		}
		else if (strcmp(opcode, sw) == 0)
		{
			result = result + pow(2, 23) + pow(2, 22);
			result = result + (charToint(arg0) << 19);
			result = result + (charToint(arg1) << 16);
			strcpy(tmparg2, arg2);
			if (isNumber(arg2))
			{
				int tmp = charToint(arg2);
				if (charToint(arg2) > 32767 || charToint(arg2) < -32768)
				{
					printf("offsetFields don’t fit in 16 bits\n");
					exit(1);
				}
				result = result + charToint(arg2);
			}
			else
			{
				int tmp = findtext(findLablePtr, tmparg2, LABEL);
				if (tmp == -1)
					exit(1);
				result = result + tmp;
			}
			fprintf(outFilePtr, "%d\n", result);
		}
		else if (strcmp(opcode, beq) == 0)
		{
			result = result + pow(2, 24);
			result = result + (charToint(arg0) << 19);
			result = result + (charToint(arg1) << 16);
			strcpy(tmparg2, arg2);
			if (isNumber(arg2))
			{
				int tmp = charToint(arg2);
				if (charToint(arg2) > 32767 || charToint(arg2) < -32768)
				{
					printf("offsetFields don’t fit in 16 bits\n");
					exit(1);
				}
				result = result + charToint(arg2);
			}
			else
			{
				int tmp1 = findtext(findLablePtr, tmparg2, LABEL);
				int tmp2 = findtext(findLablePtr, tmparg2, BEQ);
				if (tmp1 == -1 || tmp2 == -1)
					exit(1);
				int tmp = tmp1 - tmp2 - 1;
				if (tmp >= 0)
					result = result + tmp;
				else
				{
					tmp = negbinary(tmp, 16);
					result = result + tmp;
				}
			}
			fprintf(outFilePtr, "%d\n", result);
		}
		/*--------------------------------------------------------------------*/
		else if (strcmp(opcode, jalr) == 0)
		{
			result = result + (charToint(arg0) << 19);
			result = result + (charToint(arg1) << 16);
			result = result + pow(2, 24) + pow(2, 22);
			fprintf(outFilePtr, "%d\n", result);
		}
		/*--------------------------------------------------------------------*/
		else if (strcmp(opcode, halt) == 0)
		{
			result = result + pow(2, 24) + pow(2, 23);
			fprintf(outFilePtr, "%d\n", result);
		}
		else if (strcmp(opcode, noop) == 0)
		{
			result = result + pow(2, 24) + pow(2, 23) + pow(2, 22);
			fprintf(outFilePtr, "%d\n", result);
		}
		else if (strcmp(opcode, fill) == 0)
		{
			if (isNumber(arg0))
			{
				int tmp = charToint(arg0);
				if (tmp >= 0)
					result = result + tmp;
				else
					result = result + negbinary(tmp, 32);
			}
			else
			{
				strcpy(tmparg2, arg0);
				int tmp = findtext(findLablePtr, tmparg2, LABEL);
				if (tmp == -1)
					exit(1);
				result = result + tmp;
			}
			fprintf(outFilePtr, "%d\n", result);
		}
		else
			exit(1);
		result = 0;
	}

	/* TODO: Phase-1 label calculation */

	/* this is how to rewind the file ptr so that you start reading from the
		 beginning of the file */
	rewind(inFilePtr);

	/* TODO: Phase-2 generate machine codes to outfile */

	/* after doing a readAndParse, you may want to do the following to test the
		 opcode */

	if (inFilePtr)
	{
		fclose(inFilePtr);
	}
	if (findLablePtr)
	{
		fclose(findLablePtr);
	}
	if (outFilePtr)
	{
		fclose(outFilePtr);
	}
	exit(0);
	return (0);
}

/*
 * Read and parse a line of the assembly-language file.  Fields are returned
 * in label, opcode, arg0, arg1, arg2 (these strings must have memory already
 * allocated to them).
 *
 * Return values:
 *     0 if reached end of file
 *     1 if all went well
 *
 * exit(1) if line is too long.
 */
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
		return (0);
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
		int i = 0;
		if (strlen(label) > 6)
		{
			printf("label more then 6 char\n");
			exit(1);
		}
		while (label[i])
		{
			if ((label[i] >= 'A' && label[i] <= 'Z') || (label[i] >= 'a' && label[i] <= 'z') || (label[i] >= '0' && label[i] <= '9'))
			{
				if (label[0] >= '0' && label[0] <= '9')
				{
					printf("label starts with number\n");
					exit(1);
				}
				i++;
			}
			else
			{
				printf("invalid lable\n");
				exit(1);
			}
		}
		/* successfully read label; advance pointer over the label */
		if (flag1 == 0)
		{
			for(int i = 0;i<index2;i++)
			{
				if(!strcmp(labelList[i],label))
				{
					printf("duplicate label\n");
					exit(1);
				}	
			}
				int tmp = sizeof(**labelList);
				labelList = realloc(labelList, tmp + sizeof(char*));
				labelList[index2] = malloc(sizeof(char)*strlen(label));
				for (int i = 0; i < strlen(label); i++)
				{
					labelList[index2][i] = label[i];
				}
				index2++;
		}

		ptr += strlen(label);
	}

	/*
	 * Parse the rest of the line.  Would be nice to have real regular
	 * expressions, but scanf will suffice.
	 */
	sscanf(ptr, "%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%"
				"[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]",
		   opcode, arg0, arg1, arg2);
	return (1);
}

int isNumber(char *string)
{
	/* return 1 if string is a number */
	int i;
	return ((sscanf(string, "%d", &i)) == 1);
}

int findtext(FILE *findLablePtr, char *text, int whatTofind)
{
	int num = 0;
	if (whatTofind == 0)
	{
		flag1 = 1;
		while (readAndParse(findLablePtr, label, opcode, arg0, arg1, arg2))
		{
			if (strcmp(label, text) == 0)
			{
				fseek(findLablePtr, 0, SEEK_SET);
				flag1 = 0;
				return num;
			}
			num++;
		}
		flag1 = 0;
		return -1;
	}
	else if (whatTofind == 1)
	{
		flag1 = 1;
		while (readAndParse(findLablePtr, label, opcode, arg0, arg1, arg2))
		{
			if (strcmp(arg2, text) == 0)
			{
				flag1 = 0;
				fseek(findLablePtr, 0, SEEK_SET);
				return num;
			}
			num++;
		}
		flag1 = 0;
		return -1;
	}
	return 0;
}

int charToint(char *binary)
{
	int result = 0;
	int flag = 0;
	while (1)
	{
		if (*binary == '-')
		{
			flag = 1;
			binary++;
			continue;
		}
		result = result + (*binary - '0');
		binary++;
		if (*binary == '\0')
			break;
		else
			result = result * 10;
	}
	if (flag == 1)
		result = result * (-1);
	return result;
}

int negbinary(int num, int index)
{
	int i;
	int bin[index];
	int result = 0;

	for (i = index - 1; i >= 0; i--)
	{
		bin[i] = 0;
	}
	i = 0;
	num = num * (-1);
	while (num > 0)
	{
		bin[i] = num % 2;
		num /= 2;
		i++;
	}
	for (i = index - 1; i >= 0; i--)
	{
		if (bin[i] == 0)
			bin[i] = 1;
		else
			bin[i] = 0;
	}
	i = index - 1;
	while (i != -1)
	{
		result = (result * 2) + bin[i];
		i--;
	}
	result++;
	return result;
}