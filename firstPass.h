#ifndef firstPass
#define firstPass

#define MAXIMUM_FILE_NAME 1000 /* max length of file name  */
#define LINE_LEN 80			   /* max line length */
#define WORD_LEN 30			   /* max length of word  */
#define MAXIMUM_INST 7		   /* max length of instructor (.string)  */
#define false 0
#define true 1

typedef int bool; /*definition of boolean variable*/

enum
{
	UNNECESSARY = -1,
	NOTE,
	NOT_NOTE,
	EOL,
	NOT_EOL,
	LABEL,
	NOT_LABEL,
	NOT_ENT_EXT,
	NOT_STRING,
	NOT_DATA
};

/** fPass
 * first pass
 * @param fileName - the name of file as written in terminal
 * */
void fPass(char *fileName);

/** emptyIntArray
 * function that initialize the array
 * @param array - the array for initial
 * @param n - number of cells need to initial
 * */
void emptyIntArray(int array[], int n);

/** nextExpression
 * getting the next expression to word variable and advanced pointer to the next expression
 * @param ptr - pointer to char in certain line
 * @param count - pointer count of words in line
 * @param wordLength - pointer to the length of word variable
 * @param word - double pointer to word variable
 * */
void nextExpression(char **ptr, int *count, int *wordLength, char **word);

/** ignoreSpaceTab
 * ignorring spaces and tabs before and after expressions
 * @param ptr - double pointer to char in certain line
 * */
void ignoreSpaceTab(char **ptr);

/** validRow
 * checks if the row is valid
 * @param ptr - pointer to char in certain line
 * @param lineNum - line number in am file (for errors)
 * @return true/false
 * */
bool validRow(char *ptr, int lineNum); /*checks if the row is empty or note*/

/** labelCheck
 * checks if the expression is label and valid label
 * @param ptr - pointer to char in certain line
 * @param word - pointer to word variable
 * @param count - pointer count of words in line
 * @param wordLength - the length of word variable
 * @param hSymbol - head of symbol/label list
 * @param temp - temporary node
 * @param lineNum - line number in am file (for errors)
 * @return true/false
 * */
bool labelCheck(char *ptr, char *word, int count, int wordLength, gNode *hSymbol, gNode temp, int lineNum);

/** validLabel
 * inside function of label check - the function that checks label validity
 * @param list - A double pointer to symbol/label list - for updating the list (adding the label if true)
 * @param ptr - pointer to char in certain line
 * @param lineNum - line number in am file (for errors)
 * @return true/false
 * */
bool validLabel(gNode *list, char *ptr, int lineNum);

/** isEntryExtern
 * checks if the expression is entry or extern declare
 * @param ptr - pointer to char in certain line
 * @return true/false
 * */
bool isEntryExtern(char *ptr); /*checks if the row is entry or extern declare of label*/

/** isData
 * checks if the expression is data
 * @param ptr - pointer to char in certain line
 * @return true/false
 * */
bool isData(char *ptr);

/** isString
 * checks if the expression is string
 * @param ptr - pointer to char in certain line
 * @return true/false
 * */
bool isString(char *ptr); /*checks if the row is string declare*/

/** isCommand
 * checks if the inserted word (token) is command
 * @param ptr - pointer to the certain word
 * @return true/false
 * */
bool isCommand(char *ptr);

/** analyzeData
 * analyzing the numbers are written
 * @param ptr - pointer to the certain expression
 * @param hRow - pointer to the head of linked list of rows
 * @param lineNum - line number in am file (for errors)
 * @return true/false
 * */
int analyzeData(char *ptr, gNode *hRow, int lineNum);

/** analyzeString
 * analyzing the letters are written
 * @param ptr - pointer to the certain expression
 * @param hRow - pointer to the head of linked list of rows
 * @param lineNum - line number in am file (for errors)
 * @return true/false
 * */
int analyzeString(char *ptr, gNode *hRow, int lineNum);

/** analyzeOperands
 * analyzing the all kinds of instruction expressions
 * @param special - flag for special command - jmp/bne/jsr
 * @param ptr - pointer to the certain expression
 * @param commandIndex - the index of command is analyzed in certain row (0-15)
 * @param operandType - updating here the type of each operand
 * @param hSuspectLabel - A double pointer to the top of the linked list of suspect labels to update the labels used
 * @param hRow - A double pointer to the top of the linked list of rows to update the analyzed row
 * @param lineNum - line number in am file (for errors)
 * @return number of operands including the command itself
 * */
int analyzeOperands(bool special, char *ptr, int commandIndex, int *operandType, gNode *hSuspectLabel, gNode *hRow, int lineNum);

/** immidiateCheck
 * checks if the operand has immidiate address
 * @param word - pointer to the analyzed word (operand)
 * @param row - A pointer to the node of the row to update the analyzed row
 * @param opNum - the index of operand
 * @param lineNum - line number in am file (for errors)
 * @return number of operands including the command itself
 * */
int immidiateCheck(char *word, gNode row, int opNum, int lineNum);

/** isRegister
 * checks if the operands register
 * @param name - pointer to expression (operand)
 * @param row - A pointer to the node of the row - all the data of the row is alredy here
 * @param index - index of operand 1-3
 * @return true/false
 * */
bool isRegister(char *name, gNode row, int index);

/** opernadsTypeCheck
 * checks if the operands valid by type
 * @param row - A pointer to the node of the row - all the data of the row is alredy here
 * @return true/false
 * */
bool opernadsTypeCheck(gNode row);

/** checkValidInstrucion
 * Checks the instruction structure is valid - number of opernads, spaces and tabs at the right place etc.
 * @param ptr - pointer to the certain expression
 * @param opNum - the index of operand
 * @param commandIndex - the index of command is analyzed in certain row (0-15)
 * @param operandType - updating here the type of each operand
 * @param lineNum - line number in am file (for errors)
 * @return true/false
 * */
bool checkValidInstrucion(char *ptr, int opNum, int commandIndex, int *operandType, int lineNum);

/** isSigns
 * Checks if the first char in the expression has ':' '(' ')' ',' ' " '
 * @param ptr - pointer to the certain expression
 * @return true/false
 * */
bool isSigns(char *ptr);

/** numCheck
 * extract the number between the commas
 * @param ptr - pointer to the certain expression
 * @param row - A pointer to the node of the row - all the data of the row is alredy here
 * @param opNum - the index of operand
 * @return true/false
 * */
int numCheck(char *ptr, gNode row, int opNum);

/** wordLen
 * extract the exact chars count of certain expression without '"' , spaces and tabs
 * @param ptr - pointer to the certain expression
 * @return length of word/expression
 * */
int wordLen(char *ptr);

/** endOfLine
 * checks if the certain place (pointer) is end of line
 * @param ptr - pointer to the certain expression
 * @return enum{EOL/NOT_EOL} EOL - end of line
 * */
int endOfLine(char *ptr);

/** addICtoDataAddress
 * after the whole process of first pass - adding the instrucor counter to data counter
 * - data will come after instructions in ob fil
 * @param HEAD - head of symbol and row lists
 * @param IC - instruction counter
 * */
void addICtoDataAddress(gNode *HEAD, int IC);

#endif
