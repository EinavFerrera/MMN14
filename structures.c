#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "structures.h"

/*struct of command table*/
static const struct opcodeTable
{
	char *name;		/*command name - mov/lea/jmp etc.*/
	int operandNum; /*operands supose to be*/
	int src;		/*source operands type supose to be*/
	int dest;		/*destination operands type supose to be*/
} opTable[] = {{"mov", 2, (IMMEDIATE + DIRECT + DIRECT_REG), (DIRECT + DIRECT_REG)},
			   {"cmp", 2, (IMMEDIATE + DIRECT + DIRECT_REG), (IMMEDIATE + DIRECT + DIRECT_REG)},
			   {"add", 2, (IMMEDIATE + DIRECT + DIRECT_REG), (DIRECT + DIRECT_REG)},
			   {"sub", 2, (IMMEDIATE + DIRECT + DIRECT_REG), (DIRECT + DIRECT_REG)},
			   {"not", 1, (NO_ADDRESS), (DIRECT + DIRECT_REG)},
			   {"clr", 1, (NO_ADDRESS), (DIRECT + DIRECT_REG)},
			   {"lea", 2, (DIRECT), (DIRECT + DIRECT_REG)},
			   {"inc", 1, (NO_ADDRESS), (DIRECT + DIRECT_REG)},
			   {"dec", 1, (NO_ADDRESS), (DIRECT + DIRECT_REG)},
			   {"jmp", 1, (NO_ADDRESS), (DIRECT + DIRECT_REG)},
			   {"bne", 1, (NO_ADDRESS), (DIRECT + DIRECT_REG)},
			   {"red", 1, (NO_ADDRESS), (DIRECT + DIRECT_REG)},
			   {"prn", 1, (NO_ADDRESS), (IMMEDIATE + DIRECT + DIRECT_REG)},
			   {"jsr", 1, (NO_ADDRESS), (DIRECT + DIRECT_REG)},
			   {"rts", 0, (NO_ADDRESS), (NO_ADDRESS)},
			   {"stop", 0, (NO_ADDRESS), (NO_ADDRESS)}};

typedef struct genericNode
{
	char *name;			/*the name of the node - Macro name/label/row operands etc.*/
	int lineNum;		/*line number*/
	int startMacro;		/*macro - line of macro begin*/
	int endMacro;		/*macro - line of macro end*/
	int address;		/*label - address of label in ob*/
	int numOfOps;		/*row (command) - number of operand*/
	int commandIndex;	/*row (command) - between 0-15 of commands*/
	int operandType[3]; /*row (command) - maximum 3 operands (without the command itself)*/
	int op[3];			/*row (command) - if the operand is number or register*/
	int ARE;			/*label - A/R/E address*/
	char *label1;		/*row (command) - if the first operand is label*/
	char *label2;		/*row (command) - if the second operand is label*/
	char *label3;		/*row (command) - if the third operand is label*/
	types type;			/*label - type of the instruction - DATA/CODE/JUMP/EXT/ENT*/
	gNode next;			/*pointer to the next struct in the linked list*/
} Item;

gNode createNode(char *name, int address)
{
	gNode newNode = (gNode)calloc(1, sizeof(Item));
	newNode->name = (char *)calloc(1, (strlen(name) + 1) * sizeof(char));
	strcpy(newNode->name, name);
	newNode->startMacro = 0;
	newNode->endMacro = 0;
	newNode->address = address;
	newNode->commandIndex = -1;
	newNode->operandType[0] = NO_ADDRESS;
	newNode->operandType[1] = NO_ADDRESS;
	newNode->operandType[2] = NO_ADDRESS;
	newNode->ARE = 0;
	newNode->type = CODE;
	newNode->next = NULL;
	return newNode;
}

gNode search(gNode *list, char *name)
{
	if (*list != NULL)
	{
		if (strcmp(getName(*list), name) == 0)
			return *list;
		else
		{
			list = getNext(*list);
			return search(&list, name);
		}
	}
	else
		return NULL;
}

void insert(gNode *HEAD, gNode newNode)
{
	if (*HEAD != NULL)
	{
		newNode->next = *HEAD;
	}
	*HEAD = newNode;
}

gNode cmpListsName(gNode *src, gNode toCmp)
{
	gNode temp = NULL;
	while (toCmp != NULL)
	{
		temp = search(src, getName(toCmp));
		if (temp != NULL)
			return toCmp;
		toCmp = getNext(toCmp);
	}
	return NULL;
}

void cmpListDeleteSameName(gNode *src, gNode *willBeDeleted)
{
	gNode temp = cmpListsName(src, *willBeDeleted);
	if (temp != NULL)
	{
		deleteNode(willBeDeleted, temp);
		cmpListDeleteSameName(src, willBeDeleted);
	}
}
void cmpListCpyDeleteSameName(gNode *src, gNode *willBeDeleted, gNode *willBeExtended)
{
	gNode temp = cmpListsName(src, *willBeDeleted);
	if (temp != NULL)
	{
		deleteNode(willBeDeleted, temp);
		insert(willBeExtended, temp);
		cmpListCpyDeleteSameName(src, willBeDeleted, willBeExtended);
	}
}

void deleteHead(gNode *HEAD)
{
	gNode temp = *HEAD;
	*HEAD = (*HEAD)->next;
	freeList(temp);
}

void deleteNode(gNode *HEAD, gNode deleteNode)
{
	gNode cur = *HEAD;
	gNode prev = cur;
	while (cur != NULL)
	{
		if (getName(cur) == getName(deleteNode))
		{
			if (prev == cur)
			{
				*HEAD = cur->next;
				return;
			}
			else
			{
				setNext(prev, cur);
				return;
			}
		}
		prev = cur;
		cur = getNext(cur);
	}
}

void setName(gNode list, char name[]) { list->name = name; }
void setLineNum(gNode list, int lineNum) { list->lineNum = lineNum; }
void setStart(gNode list, int start) { list->startMacro = start; }
void setEnd(gNode list, int end) { list->endMacro = end; }
void setAddress(gNode list, int address) { list->address = address; }
void setNumOfOps(gNode list, int NumOps) { list->numOfOps = NumOps; }
void setCommand(gNode list, int commandIndex) { list->commandIndex = commandIndex; }
void setOpType(gNode list, int index, int type) { list->operandType[index - 1] = type; }
void setOp(gNode list, int index, int content) { list->op[index - 1] = content; }
void setARE(gNode list, int ARE) { list->ARE = ARE; }
void setLabel1(gNode list, char label1[])
{
	char *word;
	word = (char *)calloc(strlen(label1), sizeof(char));
	strncpy(word, label1, strlen(label1));
	list->label1 = word;
}
void setLabel2(gNode list, char label2[])
{
	char *word;
	word = (char *)calloc(strlen(label2), sizeof(char));
	strncpy(word, label2, strlen(label2));
	list->label2 = word;
}
void setLabel3(gNode list, char label3[])
{
	char *word;
	word = (char *)calloc(strlen(label3), sizeof(char));
	strncpy(word, label3, strlen(label3));
	list->label3 = word;
}
void setType(gNode list, types type) { list->type = type; }
void setNext(gNode list, gNode toSet) { list->next = getNext(toSet); }

char *getName(gNode list) { return list->name; }
int getLineNum(gNode list) { return list->lineNum; }
int *getStartMacro(gNode list) { return list->startMacro; }
int *getEndMacro(gNode list) { return list->endMacro; }
int getAddress(gNode list) { return list->address; }
int getNumOfOps(gNode list) { return list->numOfOps; }
int getCommand(gNode list) { return list->commandIndex; }
int getOpType(gNode list, int index) { return list->operandType[index - 1]; }
int getOp(gNode list, int index) { return list->op[index - 1]; }
int getARE(gNode list) { return list->ARE; }
char *getLabel1(gNode list) { return list->label1; }
char *getLabel2(gNode list) { return list->label2; }
char *getLabel3(gNode list) { return list->label3; }
types getType(gNode list) { return list->type; }
gNode *getNext(gNode list) { return list->next; }

char *getOpName(int index)
{
	if (index >= 0 && index < NUM_OF_OPCODE)
		return opTable[index].name;
	return "FAIL";
}
int getOpNum(int index)
{
	if (index >= 0 && index < NUM_OF_OPCODE)
		return opTable[index].operandNum;
	return -1;
}
int getOpDest(int index)
{
	if ((index >= 0) && (index < NUM_OF_OPCODE))
		return opTable[index].dest;
	return -1;
}

int getOpSrc(int index)
{
	if (index >= 0 && index < NUM_OF_OPCODE)
		return opTable[index].src;
	return -1;
}

void freeList(gNode HEAD) /*will be used after symbol table and after macro table*/
{
	gNode prev = HEAD;
	gNode cur = HEAD;
	while (cur != NULL)
	{
		prev = cur;
		cur = cur->next;
		free(HEAD->name);
		free(HEAD->label1);
		free(HEAD->label2);
		free(HEAD->label3);
		free(prev);
	}
}

bool isMcrValid(char *mcrName)
{
	int i;
	for (i = 0; i <= 15; i++)
	{
		if (strcmp(mcrName, opTable[i].name) == 0)
			return true;
		if (strcmp(mcrName, ".string") == 0)
			return true;
		if (strcmp(mcrName, ".data") == 0)
			return true;
	}
	return false;
}
