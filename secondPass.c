#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "structures.h"
#include "secondPass.h"
#include "binaryBuild.h"

void secPass(char *fileName, gNode *hRow,gNode *hSuspectLabel,gNode *hSymbol, gNode *hEntryExtern, int IC, int DC)
{
	/***********************declaration for the proccess************************/	
	char filePath[MAXIMUM_FILE_NAME];
	FILE *binaryFile, *entFile, *extFile;
	gNode temp;
	int count=0;
	int status;
	bool error = false;
	/***************************************************************************/	

	/***checks if there is undeclared labels but written inside instrucions***/	
	cmpListDeleteSameName(hSymbol,hSuspectLabel);	
	cmpListDeleteSameName(hEntryExtern,hSuspectLabel);

	if (getName(hSuspectLabel)!=NULL){
		printf("ERROR: Invalid labels was founded - the next labels are not declare properly\n");
		printLabels(*hSuspectLabel);
		return;
	}
	/***************************************************************************/	

	updateEntExtInSymbols(hEntryExtern, hSymbol);

	/**************************create an ent file*******************************/	
	temp = *hEntryExtern;
	while (temp!=NULL){
		if (getType(temp)==ENTRY)
			count++;
		temp = getNext(temp);
	}
	if (count>0){
		count = 0;
		sprintf(filePath, "%s.ent", fileName);
		entFile = fopen(filePath, "w");
		createEntFile(*hEntryExtern,*hSymbol,entFile);
		fclose(entFile);
	}
	/***************************************************************************/	

	/**************************create an ext file*******************************/	
	temp = *hEntryExtern;
	while (temp!=NULL){
		if (getType(temp)==EXT){
			if (search(hSymbol,getName(*hEntryExtern))!=NULL)
				count++;}
		temp = getNext(temp);
	}
	if (count>0){
		count = 0;
		sprintf(filePath, "%s.ext", fileName);
		extFile = fopen(filePath, "w");
		createExtFile(*hEntryExtern,*hSymbol,extFile);
		fclose(extFile);
	}
	/***************************************************************************/

	/**************************create an ob file********************************/	
	sprintf(filePath, "%s.ob", fileName);
	binaryFile = fopen(filePath, "w");
	fprintf(binaryFile,"\t%d\t%d\n",IC,DC);

	binaryCode(binaryFile,*hRow,*hSymbol);
	fclose(binaryFile);
	/***************************************************************************/	
	
	printf("\nThe Second pass phase has been completed successfully for file: %s\n\nAll the process has been finished without errors!!!",filePath);
	printf("\n******************************************************************************************\n");
}

void createEntFile(gNode hEntryExtern,gNode hSymbols, FILE *entFile)
{
	if (hEntryExtern==NULL) {
		printf("\tent file was created successfully\n");
		return;}
	createEntFile(getNext(hEntryExtern),hSymbols,entFile);
	if (getType(hEntryExtern)==ENTRY){
		if (search(&hSymbols,getName(hEntryExtern))!=NULL)
			fprintf(entFile,"%s\t0%d\n",getName(hEntryExtern),100 + getAddress(search(&hSymbols,getName(hEntryExtern))));
	}
	return;
}

void createExtFile(gNode hEntryExtern,gNode hSymbols, FILE *extFile)
{
	if (hEntryExtern==NULL) {
		printf("\text file was created successfully\n");
		return;}
	createExtFile(getNext(hEntryExtern),hSymbols,extFile);
	if (getType(hEntryExtern)==EXT){
		if (search(&hSymbols,getName(hEntryExtern))!=NULL){
			fprintf(extFile,"%s\t0%d\n",getName(hEntryExtern),100 + getAddress(search(&hSymbols,getName(hEntryExtern))));}
	}
	return;
}

void updateEntExtInSymbols(gNode *hEntExt,gNode *hSymbol){
	gNode temp = *hSymbol;
	gNode temp2 = NULL;
	while ((temp)!=NULL){
		setARE(temp,1<<1);
		temp2 = search(hEntExt,getName(temp));
		if (temp2!=NULL){
			if (getType(temp2)==EXT) {setARE(temp,1);}
		}
		temp = getNext(temp);
	}
}

void printLabels(gNode HEAD){	
	gNode tmp = HEAD;
	int i=0;
	while ((tmp != NULL)&& (getName(tmp)!=NULL)){
		printf("\n");
		printf("-> %s", getName(tmp));
		printf("\tLine Num: %d\n", getLineNum(tmp));

		tmp = getNext(tmp);
	}
	printf("\n");
}