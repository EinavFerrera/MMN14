#include "structures.h"
#include "preAssembler.h"
#include "firstPass.h"
#include "errors.h"
void extractMcr(gNode, char **, FILE **);

void compileFile(char *fileName)
{
	/***********************declaration for the proccess************************/
	FILE *originalFile; /*original file from the terminal*/
	FILE *modifiedFile; /*modified file of the original - without spaces, tabs with the opened macros*/

	char *ptr, *beginLine; /*ptr - pointer of line		/	beginLine - define the new begining of line without spaces or tabs*/
	char line[LINE_LEN];   /*array of chars - getting whole line content*/
	char *mcrName;		   /*macro name*/

	bool isName = false; /*flag for correct name of macro (exp. m1 as the name of macro alredy signed)*/
	bool isMCR = false;	 /*flag for macro*/
	bool error = false;

	int mcr = NONE;	 /*note for macro - NONE / MCR / ENDMCR*/
	int macroLines;	 /*counting the num of lines in each macro*/
	int lineNum = 0; /*row number*/

	gNode hMacro = NULL; /*head of macro list*/
	gNode temp = NULL;	 /*getting the address of the willing node (exp. serach function etc.)*/
	/**************************************************************************/

	openFiles(&fileName, &originalFile, &modifiedFile); /*openning original and modified files*/
	if (modifiedFile == NULL)
	{
		return;
	} /*the file path in the terminal is the am file*/

	/***************************runs on all the lines**************************/
	while (fgets(line, sizeof(line), originalFile) != NULL)
	{
		/*initializing the variables of new line*/
		lineNum += 1;
		ptr = &line;
		/***************************************/
		if ((*ptr == '\r') || (*ptr == '\n'))
			continue;

		printf("this is ptr1 _%s_\n" , ptr); 
		ignoreSpaceTab(&ptr);
		printf("this is ptr2 _%s_\n" , ptr);
		beginLine = ptr;

		mcr = checkMcrFun(ptr, &isMCR); /*MCR, ENDMCR, NONE, NESTED_MACRO*/

		if (isMCR == true)
		{
			if (mcr == MCR) /*begining of macro declaration*/
			{
				ptr = ptr + 3;
				ignoreSpaceTab(&ptr);
				mcrName = (char *)calloc(strlen(ptr) - 1, sizeof(char));
				strncpy(mcrName, ptr, strlen(ptr) - 1);

				if (isMcrValid(mcrName) == true) /*The name of the macro is the name of the directive or instructor*/
				{
					printf("ERROR line %d: The name of the macro is the name of the directive or instructor - %s\n", lineNum, mcrName);
					error = true;
				}

				if ((hMacro != 0) && (search(&hMacro, mcrName) != NULL)) /*multiple declaration of same macro name*/
				{
					printf("ERROR line %d: Multiple declaration of the same macro\n", lineNum);
					error = true;
				}
				temp = createNode(mcrName, 0);
				insert(&hMacro, temp);
				setStart(temp, lineNum + 1);
			}
			if (mcr == NONE)
			{
				setEnd(temp, lineNum);
			} /*macro functions*/
		}
		else if (mcr == ENDMCR)
		{
		}
		else
		{
			mcrName = (char *)calloc(strlen(ptr) - 1, sizeof(char));
			strncpy(mcrName, ptr, strlen(ptr) - 1);
			temp = search(&hMacro, mcrName);
			if (temp == NULL)
			{
				fputs(beginLine, modifiedFile);
			}
			else
			{
				extractMcr(temp, &fileName, &modifiedFile);
			}
		}
	}

	fclose(originalFile);
	fclose(modifiedFile);

	if (!error)
	{
		printf("Pre assembler phase has been completed - am file was created successfully\n");
		fPass(fileName);
		return;
	}
	else
	{
		printf("The process for file: %s - does not continue due to certain errors - see below\n", fileName);
		printf("**********************************************************************************\n");
		return;
	}
}

void openFiles(char **fileName, FILE **originalFile, FILE **modifiedFile)
{
	char filePath[MAXIMUM_FILE_NAME];

	/************************opening original file*********************/
	sprintf(filePath, "%s.am", *(fileName));
	*originalFile = fopen(filePath, "r");
	if (*originalFile == NULL)
	{
		sprintf(filePath, "%s.as", *(fileName));
		*originalFile = fopen(filePath, "r");
		if (*originalFile == NULL)
		{
			printf("ERROR: Assembler file %s not found\n", filePath);
			return;
		}
	}
	else
	{
		*modifiedFile = NULL;
		fclose(*originalFile);
		printf("\n******************************************************************************************\n");
		printf("\t\t\t\t%s", filePath);
		printf("\n******************************************************************************************\n");
		printf("The system detected an %s file - Skipping 'pre assembler' phase to first pass\n\n", filePath);
		fPass(*(fileName));
		return;
	}
	printf("\n******************************************************************************************\n");
	printf("\t\t\t\t%s", filePath);
	printf("\n******************************************************************************************\n");
	printf("\n******************************************************************************************\n");
	printf("Starting 'pre assembler' phase for file: %s\n\n", filePath);
	/************************opening modified file*********************/
	sprintf(filePath, "%s.am", *(fileName));
	*modifiedFile = fopen(filePath, "w");
	if (modifiedFile == NULL)
	{
		printf("Assembler file %s not found\n", filePath);
		return;
	}
}

/** extractMcr
 * writting the macro that declared to certain place inside am file
 * @param node - node of certain macro - the macros are stored inside linked list
 * @param fileName - the name of file as written in terminal
 * @param modified - the modified file with .am extenssion after extracting the macros
 * */
void extractMcr(gNode node, char **fileName, FILE **modified)
{
	char filePath[MAXIMUM_FILE_NAME];
	sprintf(filePath, "%s.as", *(fileName));
	FILE *original;
	original = fopen(filePath, "r");

	int i = 1;
	int start, end;
	char line[LINE_LEN];
	start = getStartMacro(node);
	end = getEndMacro(node);
	char *ptr;

	while (fgets(line, sizeof(line), original) != NULL)
	{
		if (i < start)
		{
			i += 1;
		}
		else
		{
			if (start <= end)
			{
				ptr = &line;
				ignoreSpaceTab(&ptr);
				fputs(ptr, *modified);
				start += 1;
				i = start;
			}
			else
				break;
		}
	}
	fclose(original);
}

int checkMcrFun(char *beginLine, bool *isMCR)
{
	char mcr[] = "mcr";
	char endmcr[] = "endmcr";

	if (((strncmp(beginLine, mcr, 3)) == 0) && ((*(beginLine + 3) == ' ') || (*(beginLine + 3) == '\t'))) /*checks if equal to 'mcr'*/
	{
		if (*isMCR == true)
		{
			printf("ERROR: NESTED MACRO\n");
			return NESTED_MACRO;
		}

		*isMCR = true;
		return MCR;
	}
	else if (((strncmp(beginLine, endmcr, 6)) == 0) && ((*(beginLine + 6) == ' ') || (*(beginLine + 6) == '\t') || (*(beginLine + 6) == '\r') || (*(beginLine + 6) == '\n'))) /*checks if equal to 'endmcr'*/
	{
		*isMCR = false;
		return ENDMCR;
	}
	else /*none of them*/
	{
		return NONE;
	}
}
