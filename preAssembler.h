#ifndef preAssembler
#define preAssembler
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "types.h"

#define MAXIMUM_FILE_NAME 1000 /* max length of file name  */
#define LINE_LEN 80			   /* max line length */
#define WORD_LEN 30			   /* max length of word  */

enum macro
{
	MCR,
	ENDMCR,
	NONE,
	NESTED_MACRO,
	MCR_NO_NAME
};

/** compileFile
 * get a file and convert it to .am extension with extract macros
 * @param fileName - the name of file as written in terminal
 * */
void compileFile(char *fileName);

/** openFiles
 * openning original and modified files
 * @param fileName - the name of file as written in terminal
 * @param originalFile - the file with .as extenssion
 * @param modifiedFile - the modified file with .am extenssion after extracting the macros
 * */
void openFiles(char **fileName, FILE **originalFile, FILE **modifiedFile);

/** checkMcrFun
 * cheking if the the row is related to macro function
 * @param ptr - pointer of row chars
 * @param isMCR - boolean variable for validating the macro declaration
 * @return - MCR, ENDMCR, NONE, NESTED_MACRO
 * */
int checkMcrFun(char *ptr, bool *isMCR);

#endif
