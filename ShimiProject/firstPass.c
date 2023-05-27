#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <ctype.h>
#include "structures.h"
#include "registerTable.h"
#include "firstPass.h"
#include "secondPass.h"
#include "errors.h"

void fPass(char *fileName)
{

	/*******************************declaration for the proccess*************************************/
	FILE *modifiedFile; 					/*am file - with extracted macro*/

	char filePath[MAXIMUM_FILE_NAME];		/*contains the file path - recived from the terminal*/
	char line[LINE_LEN]; 					/*array of chars - getting whole line content*/
	char *ptr;								/*pointer of row chars*/
	char *word;								/*pointer word - will be declared forward*/
	
	int wordLength;							/*measuring the length of word*/
	int lineNum = 0; 						/*absolute line number in the am file*/
	int IC = 0;								/*instruction counter*/
	int DC = 0;								/*data counter*/
	int L = 0;								/*utility for calculate IC and DC*/
	int count = 0;							/*counting words in row*/
	int operands[3] = {-1,-1,-1}; 			/*address method of each operand*/
	int commandIndex = -1;					/*will be filled in the command index of the certain line*/
	int validRowInput;						/*getting the possitive value if the row is valid*/

	gNode hSymbol = NULL; 					/*head of symbols/labels list (declared)*/
	gNode hSuspectLabel = NULL;				/*head of suspect symbols/labels list (used labels)*/
	gNode hRow = NULL;						/*head of row list - contain all the data for each row (without the label)*/
	gNode hEntryExtern = NULL;				/*head of ENT/EXT labels*/
	gNode temp = NULL;	 					/*temp node*/
	
	bool valid;								/*getting value for valid operands type in instruction*/
	bool label = false;						/*flag for label declared in certain line*/
	bool specialCommand = false;			/*flag when jmp/bne/jsr command declared in certain line*/
	bool error = false;						/*flag for error - if true - ob file will not be created*/
	/*************************************************************************************************/

	sprintf(filePath, "%s.am", fileName);
	modifiedFile = fopen(filePath, "r");

	/***************runs on all the lines*****************/
	while (fgets(line, sizeof(line), modifiedFile) != NULL)
	{
		/*intializing the variables*/
		lineNum += 1;
		L = 0;
		count = 0;
		emptyIntArray(operands,3);
		commandIndex = -1;
		label = false;
		specialCommand = false;
		ptr = line;
		/***************************/	

		validRowInput = validRow(line,lineNum);
		if (validRowInput==0)
			continue;
		else if (validRowInput<0)
			error = true;

		nextExpression(&ptr,&count,&wordLength,&word);

		if (labelCheck(ptr,word,count,wordLength,&hSymbol,temp,lineNum)==true){		
			label = true;
			ptr+=1;
			temp = createNode(word, 0);			
			nextExpression(&ptr,&count,&wordLength,&word);
		}
		else	{label = false;}

		if ((isEntryExtern(word) == ENTRY) && (!label)){
			nextExpression(&ptr,&count,&wordLength,&word);
			temp = createNode(word, 0);			
			if (validLabel(&hEntryExtern,word,lineNum)){
				setType(temp,ENTRY);
				insert(&hEntryExtern,temp);
				continue;
			}
			else
				error = true;
		}
		else if (isEntryExtern(word) == EXT){
			nextExpression(&ptr,&count,&wordLength,&word);
			temp = createNode(word, 0);			
			if (validLabel(&hEntryExtern,word,lineNum)){
				setType(temp,EXT);
				insert(&hEntryExtern,temp);
				continue;
			}
			else
				error = true;
		}

		commandIndex = isCommand(word);

		/*data / string / command / entry / extern*/
		if (isData(word)){				

			L = analyzeData(ptr,&hRow,lineNum);

			if (!label){
				printf("ERROR: Invalid declration - has to be label before .data in line %d\n",lineNum);
				error = true;
			}
			if (label){
				setAddress(temp,DC);
				setType(temp, DATA);
				insert(&hSymbol,temp);
			}
			if (L<0)	{error = true;}		

			setType(hRow, DATA);
			ptr = ptr + strlen(ptr);
			setAddress(hRow,DC);
			DC = DC + L;
			continue;
		}
		else if (isString(word)){

			if (!label){
				printf("ERROR: Invalid declration - has to be label before .string in line %d\n",lineNum);
				error = true;
			}

			L = analyzeString(ptr,&hRow,lineNum); 

			if (L<0)	{error = true;}		
			if (label){
				setAddress(temp,DC);
				setType(temp, DATA);
				insert(&hSymbol,temp);
			}

			ptr = ptr + L + 1;
			setAddress(hRow,DC);	
			setLineNum(hRow,lineNum);
			DC = DC + L;
			continue;
		}
		else if (commandIndex>=0){
			if ((commandIndex == 9) || (commandIndex == 10) || (commandIndex == 13)) {specialCommand = true;}
		
			L = analyzeOperands(specialCommand,ptr,commandIndex ,operands,&hSuspectLabel,&hRow,lineNum); 

			if (L<0)	{error = true;}
			if (label) {
				setAddress(temp,IC);
				insert(&hSymbol,temp);
			}				
			ptr = ptr + L + 1;
			setAddress(hRow,IC);	
			setLineNum(hRow,lineNum);
			IC = IC + L;
			continue;
		}
	
		else if (((!label) && (count==1)) || ((label) && (count==2))) /*definitely not a command now*/{
			printf("ERROR: Invalid instruction - supose to be command \\ .string \\ .data in line %d\n", lineNum);
			error = true;
		}

	}

	valid = opernadsTypeCheck(hRow);

	if (!valid)
		error = true;
	
	if (!error){
		addICtoDataAddress(&hSymbol, IC);
		addICtoDataAddress(&hRow,IC);
		secPass(fileName,&hRow,&hSuspectLabel,&hSymbol,&hEntryExtern,IC,DC);
	}		
	else{
		printf("\n\n**********************************************************************************\n");
		printf("\tThe process does not continue due to certain errors - see below\n");
		printf("**********************************************************************************\n");
		return;
	}

	fclose(modifiedFile);
}

void emptyIntArray(int array[],int n){
	int i=0;
	for (i=0;i<n;i++)
		array[i]=-1;
}

void nextExpression(char **ptr,int *count,int *wordLength, char **word){
	ignoreSpaceTab(ptr);
	*count = *count + 1;
	*wordLength = wordLen(*ptr);
	*word = (char *)calloc(*wordLength, sizeof(char));
	strncpy(*word,*ptr,*wordLength);
	*ptr = *ptr + *wordLength;
}

void ignoreSpaceTab(char **ptr){
	int buffer = 0;
	while (((**ptr == ' ') || (**ptr == '\t')) && (buffer <= 80)){
		buffer += 1;
		*ptr = *ptr + 1;
	}
}

bool validRow(char *ptr,int lineNum){
	int i = 0;
	while(endOfLine((ptr+i))!=EOL)
		i++;
	if (i>=(LINE_LEN-1)){
		printf("ERROR: Too many chars (above 80) in line %d\n",lineNum);
		return TOO_MANY_CHARS;
	}
	if ((*ptr == '\r') || (*ptr == '\n') || (*ptr == ';'))
		return false;
	else
		return true;
}

bool labelCheck(char *ptr,char *word, int count, int wordLength,gNode *hSymbol,gNode temp,int lineNum){
	/*label check*/
	if (((*ptr ==':') && count==1) || (wordLength==1 && count==1)){
		if ((validLabel(hSymbol,word,lineNum))){
			ptr+=1;
			return true;
		}
		else {return false;}
	}
}

bool validLabel(gNode *list, char *ptr, int lineNum){
	int i = 0;
	if (isdigit(*ptr)) {
		printf("ERROR: The label '%s' starts with digit and not a number in line %d\n",ptr,lineNum);		
		return false;}
	if (list!=NULL){
		if (search(list,ptr)!=NULL){
			printf("ERROR: The label '%s' has already been defined in line %d\n",ptr,lineNum);
			return false;
		}
	}
	while (i<strlen(ptr)){
		if (i==30){
			printf("ERROR: Too many letters in label (above 30) in line %d\n",lineNum);
			return false;
		}
		if (!isalpha(*(ptr+i)) && (!isdigit(*(ptr+i))) && ((endOfLine(ptr+i))!=EOL)){
			printf("ERROR: The label '%s' contains not only numbers or letters in line %d\n",ptr,lineNum);
			return false;
		}
		i++;
	}
	return true;
}

bool isEntryExtern(char *ptr){
	char entry[] = ".entry";
	char ext[] = ".extern";
	if (strncmp(ptr, entry, strlen(entry)) == 0)
		return ENTRY;
	else if (strncmp(ptr, ext, strlen(ext)) == 0)
		return EXT;
	else
		return NOT_ENT_EXT;
}

bool isData(char *ptr){				
	if (strcmp(ptr, ".data") == 0)
		return true;
	return false;
}

bool isString(char *ptr){
	if (strcmp(ptr, ".string") == 0)
		return true;
	return false;
}

int isCommand (char *ptr){
	int i=0;
	int length;
	while (i<NUM_OF_OPCODE){
		if ((strcmp(ptr,getOpName(i)))==0)
			return i;
		i++;
	}
	return -1;
}

int analyzeData (char *ptr,gNode *hRow, int lineNum){
	int i = 0;
	int j=0;
	int count = 0;
	int number;
	char *ptrOrigin = ptr;
	char *word;
	gNode rowToBinary = NULL;
	char expression[strlen(ptr)];
	bool error = false;
	strcpy(expression,ptr);

	rowToBinary = createNode(expression,0);
	setLineNum(rowToBinary,lineNum);

	while ((j<=strlen(ptrOrigin))){
		ignoreSpaceTab(&ptr);
		if ((*(ptr+i)==',') || j==strlen(ptrOrigin)){
			if (i==0){
				printf("ERROR: invalid number declaration - no number was declared before the comma in line %d\n",lineNum);
				error = true;
				i++;
			}
			else{
				word = (char *)calloc(i, sizeof(char));
				strncpy(word,ptr,i);
				if (numCheck(word,rowToBinary,UNNECESSARY)>=0){
					count ++;
					ptr = ptr+i+1;
					if ((ptr-ptrOrigin)<strlen(ptrOrigin))
						ignoreSpaceTab(&ptr);
					i = 0;
					free(word);
				}
				else{
					error = true;
					i++;}
			}
		}
		else
			i++;
		j++;
	}

	if (count==0){
		printf("ERROR: no number declared at all in line %d\n",lineNum);
		error = true;
	}

	if (!error){
		setType(rowToBinary,DATA);
		insert(hRow,rowToBinary);
		return count;}
	else
		return false;
}

int analyzeString (char *ptr,gNode *hRow, int lineNum){
	ignoreSpaceTab(&ptr);
	char *ptrOrigin = ptr;
	int i=0;
	int count = 0; 
	bool comma[] = {false,false};	
	gNode rowToBinary = NULL;
	char expression[strlen(ptr)];
	strcpy(expression,ptr);

	if(*ptr == 34){
		comma[0] = true;
		ptr+=1;
	}
	else{
		printf("ERROR: String not valid - no quotation marks before string in line %d\n",lineNum);
		return NOT_VALID_STRING;		
	}

	while (endOfLine((ptr+i))!=EOL){
		if (*(ptr+i) == 34){
			if (!comma[1])
				comma[1] = true;
			else{
				printf("ERROR: String not valid - too much quotation marks in line %d\n",lineNum);
				return NOT_VALID_STRING;
			}
		}
		else
			count++;
		i++;
	}

	if (count==0){
		printf("ERROR: String not valid - empty in line %d\n",lineNum);
		return NOT_VALID_STRING;
	}
	else if (comma[0]!=comma[1]){
		printf("ERROR: String not valid - no quotation marks after string '%s' in line %d\n",ptrOrigin,lineNum);
		return NOT_VALID_STRING;		
	}

	rowToBinary = createNode(expression,0);
	setType(rowToBinary,DATA);
	insert(hRow,rowToBinary);

	return count+1; /* +1 for '\0' */
}

int analyzeOperands (bool special, char*ptr,int commandIndex, int *operandType, gNode *hSuspectLabel,gNode *hRow,int lineNum){
	bool firstOp = true;
	bool secOp = false;
	bool reg = false;

	gNode rowToBinary = NULL;
	gNode temp = NULL;
	
	int isImidiate;
	int opNum = 0;

	char *token = NULL;
	char delims[] = ", \r\n\t";
	char delims2[] = " (,) \r\n\t";
	char expression[strlen(ptr)];

	strcpy(expression,ptr);
	rowToBinary = createNode(expression,0);
	setLineNum(rowToBinary,lineNum);
	
	if (!special) {token = strtok(ptr,delims);}
	else {token = strtok(ptr,delims2);}
	while ((token!=NULL) && ((endOfLine(token)!=EOL))){

		isImidiate = immidiateCheck(token,rowToBinary,opNum,lineNum);
		
		if (isImidiate==true){
			if (firstOp)	{operandType[0]=IMMEDIATE;}
			else if (secOp) {operandType[1]=IMMEDIATE;}
			else 			{operandType[2]=IMMEDIATE;}
			opNum+=1;
		}

		else if (isRegister(token,rowToBinary,opNum)){
			if (firstOp) 	{operandType[0]=DIRECT_REG;}
			else if (secOp) {operandType[1]=DIRECT_REG;}
			else 			{operandType[2]=DIRECT_REG;}
			opNum+=1;
		}
		else if (validLabel(NULL,token,lineNum)){
			if (firstOp)	{operandType[0]=DIRECT;}
			else if (secOp)	{operandType[1]=DIRECT;}
			else 			{operandType[2]=DIRECT;}
			temp = createNode(token, 0);
			setLineNum(temp,lineNum);
			insert(hSuspectLabel,temp);
			opNum+=1;
		}
		
		if (firstOp){
			firstOp = false;
			secOp = true;
			setLabel1(rowToBinary,token);				
			setOpType(rowToBinary,1,operandType[0]);
		}
		else if (secOp)	{
			secOp = false;
			setLabel2(rowToBinary,token);				
			setOpType(rowToBinary,2,operandType[1]);
		}
		else{
			setLabel3(rowToBinary,token);				
			setOpType(rowToBinary,3,operandType[2]);
			setType(rowToBinary,JUMP);
		}
		if (!special) {token = strtok(NULL,delims);}
		else {token = strtok(NULL,delims2);}
	}
	setCommand(rowToBinary,commandIndex);
	setNumOfOps(rowToBinary,opNum);
	insert(hRow,rowToBinary);

	if ((operandType[0]==NO_ADDRESS) && (commandIndex<14)){
		printf("ERROR: invalid instrucion - first operand is not valid in line %d\n",lineNum);
		return -1;
	}
	if (checkValidInstrucion(expression, opNum, commandIndex,operandType,lineNum)<=0)
		return-1;
	else if (isImidiate<0) {return-1;}

	if (((operandType[0]==DIRECT_REG) && (operandType[1]==DIRECT_REG)) && (!special)) {opNum-=1;} /*two registers in on opcode*/
	if (((operandType[1]==DIRECT_REG) && (operandType[2]==DIRECT_REG)) && (special)) {opNum-=1;} /*two registers in on opcode*/

	return opNum+1; /* +1 for command */
}

int immidiateCheck(char *word, gNode row, int opNum, int lineNum){
	if (*word=='#'){
		if (numCheck((word+1),row, opNum)>=0)
			return true;
		else {return -1;}
	}
	else if ((isdigit(*word)) && (getOpType(row,opNum)==IMMEDIATE)){
		printf("ERROR: Invalid declare of number - supose to be '#' before number %s in line %d\n",word,lineNum);
		return -1;
	}
	return false;
}

bool isRegister(char *name,gNode row,int index){
	char reg[2];
    int i=0;
	if (strlen(name)>2){
		if ((endOfLine((name+2))!=EOL) && (*(name+2)!=' ') && (*(name+2)!='\t')) {false;}}
	strncpy(reg,name,2);
    while (i<REGISTER_NUM){
        if (strcmp(reg,getRegisterName(i))==0){
			setOp(row,index+1,i);
            return true;
        }
        i++;
    }
    return false;
}

bool opernadsTypeCheck(gNode row){
	bool valid = true;
	if (row==NULL) {return true;}
	valid = opernadsTypeCheck(getNext(row));

	int i;
	int opType;
	int command = getCommand(row);

	if (getType(row)==DATA){
		return valid&true;}

	for (i = 1;i<=getOpNum(command);i++){
		
		opType = getOpType(row,i);

		if (getType(row)==JUMP){
			if ((i==1)&&(opType!=DIRECT)) {
					printf("ERROR: invalid instrucion declaration - supose to be label after command in line %d\n",getLineNum(row));
					valid = valid&false;}
			else if ((i==2) && (opType==NO_ADDRESS)){
					printf("ERROR: invalid instrucion declaration - first operand inside brackets supose to be immidiate\\direct\\register in line %d\n",getLineNum(row));
					valid = valid&false;}
			else if ((i==3) && ((opType!=IMMEDIATE) && (opType!=DIRECT_REG) && (opType!=DIRECT))){
					printf("ERROR: invalid instrucion declaration - second operand inside brackets supose to be immidiate\\direct\\register in line %d\n",getLineNum(row));
					valid = valid&false;}
		}
		else if (getOpNum(getCommand(row))==3){
			if ((i==1)&&(opType!=DIRECT)) {
					printf("ERROR: invalid instrucion declaration - supose to be label after command in line %d\n",getLineNum(row));
					valid = valid&false;}}
		else if (getOpNum(getCommand(row))==2){
			if ((i==1)&&((opType&getOpSrc(command))==0)){
					printf("ERROR: invalid instrucion declaration - first operand has invalid source address in line %d\n",getLineNum(row));
					valid = valid&false;}
			else if ((i==2)&&((opType&getOpDest(command))==0)){
					printf("ERROR: invalid instrucion declaration - second operand has invalid source address in line %d\n",getLineNum(row));
					valid = valid&false;}
		}
		else if (getOpNum(getCommand(row))==1){
			if ((i==1)&&((opType&getOpDest(command))==0)){
					printf("ERROR: invalid instrucion declaration - first operand has invalid destination address in line %d\n",getLineNum(row));
					valid = valid&false;}
		}
	}
	return valid&true;
}

bool checkValidInstrucion(char*ptr, int opNum, int commandIndex,int *operandType,int lineNum)
{	
	int bracketOp = 0;
	int bracketCl = 0;
	int comma = 0;
	int i = 0;
	bool special = false;
	ignoreSpaceTab(&ptr);
	if ((commandIndex == 9) || (commandIndex == 10) || (commandIndex == 13)) {special=true;}

	if (special) {
		if ((opNum!=1) && (opNum!=3)){
			printf("ERROR: Invalid instruction - no match between command and num of operands in line %d\n",lineNum);
			return false;
		}
	}
	else if	(getOpNum(commandIndex)!=opNum){
		printf("ERROR: Invalid instruction - no match between command and num of operands in line %d\n",lineNum);
		return false;
	}

	while (endOfLine(ptr+i)!=EOL)
	{		
		if (*(ptr+i) == ',')
			comma++;
		if (*(ptr+i) == '(')
			bracketOp++;
		if (*(ptr+i) == ')')
			bracketCl++;

		if ((special) && ((*(ptr+i)==' ') || (*(ptr+i)=='\t')) && (bracketCl==0))
		{
			printf("ERROR: Invalid instruction - in jmp/bne/jsr commands, spaces or tabs are prohibited: '%s' in line %d\n",ptr,lineNum);
			return NOT_VALID_INSTRUCTION;
		}
		i++;
	}

	if (special){
		if (bracketOp>bracketCl){
			printf("ERROR: Invalid instruction - missing close bracket: in line %d\n",lineNum);
			return NOT_VALID_INSTRUCTION;
		}
		if (bracketOp<bracketCl){
			printf("ERROR: Invalid instruction - missing open bracket in line %d\n",lineNum);
			return NOT_VALID_INSTRUCTION;
		}
		if (((comma>=(opNum-1)) && (opNum==3)) || ((comma>=opNum) && (opNum==1))){
			printf("ERROR: Invalid instruction - Too many commas in line %d\n",lineNum);
			return NOT_VALID_INSTRUCTION;
		}		
	}
	else{
		if (((comma>=opNum) && (commandIndex<14)) || ((comma>=(opNum+1)) && (commandIndex>13))){
			printf("ERROR: Invalid instruction - Too many commas in line %d\n",lineNum);
			return NOT_VALID_INSTRUCTION;
		}
		if (bracketCl!=bracketOp){
			printf("ERROR: Invalid instruction - not supose to be bracket in line %d\n",lineNum);
			return NOT_VALID_INSTRUCTION;
		}		
	}

	return true;
}

bool isSigns(char *ptr)
{
	if ((*ptr==':')||(*ptr=='(')||(*ptr==')')||(*ptr==',')||(*ptr=='"'))
		return true;
	else
		return false;
}

int numCheck(char *ptr,gNode row, int opNum){
	int i=0;
	bool sign = false;
	bool digit = false;
	bool spaceTab = false;

	while (i<strlen(ptr)){
		if (endOfLine(ptr+i)==EOL){
			if (!digit){
				printf("ERROR: invalid number declaration - no number after comma in line %d\n",getLineNum(row));
				return NO_NUMBER_AFTER_COMMA;
			}
			break;
		}

		if (((*(ptr+i))==' ') || ((*(ptr+i))=='\t')){
			spaceTab = true;
			i++;
		}
		else if ((*(ptr+i))=='-'||(*(ptr+i))=='+'){
			if (spaceTab){
				printf("ERROR: invalid number declaration - no comma seperation between numbers: in line %d\n",getLineNum(row));
				return NO_COMMA_SEPERATE;	
			}
			else if (!sign){
				if (digit){
					printf("ERROR: invalid number declaration - sign after digit: in line %d\n",getLineNum(row));
					return SIGN_AFTER_DIGIT;	
				}
				else{
					sign = true;
					i++;
				}
			}
			else{
				printf("ERROR: invalid number declaration - too many signs: in line %d\n",getLineNum(row));
				return TOO_MANY_SIGNS;
			}
		}
		else if (isdigit(*(ptr+i))){
			if (spaceTab){
				printf("ERROR: invalid number declaration - no comma seperation between numbers: in line %d\n",getLineNum(row));
				return NO_COMMA_SEPERATE;	
			}
			digit = true;
			i++;
		}
		else{ 
			printf("ERROR: invalid number declaration - supose to be number instead in line %d\n",getLineNum(row));
			return NOT_DIGIT_SIGN;
		}
	}

	if (strlen(ptr)==0){
		printf("ERROR: invalid number declaration - supose to be number instead in line %d\n",getLineNum(row));
		return NO_NUMBER;
	}
	if (opNum!=UNNECESSARY)
		setOp(row,opNum+1,atoi(ptr));
	return true;
}

int wordLen (char *ptr){
	int i=0;
	if (isSigns(ptr))
		return 1;
	while ((*(ptr+i)!=' ')&&(*(ptr+i)!='\t') && (isSigns(ptr+i)==false) &&(*(ptr+i)!='\n')&&(*(ptr+i)!='\r')&&(*(ptr+i)!='\r')&&(*(ptr+i)!='\0'))
		i++;
	return i;
}

int endOfLine(char *ptr){			
	if ((*ptr=='\r')||(*ptr=='\n')||(*ptr=='\0')){
		return EOL;
	}
	else{
		return NOT_EOL;
	}
}

void addICtoDataAddress(gNode *HEAD, int IC){
	gNode temp = *HEAD;
	while(temp!=NULL){
		if (getType(temp)==DATA)
			setAddress(temp,getAddress(temp)+IC);
		temp = getNext(temp);
	}
}