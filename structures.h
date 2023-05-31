#ifndef structures
#define NUM_OF_OPCODE 16
#define false 0 /*boolean false*/
#define true 1	/*boolean true*/

typedef int bool;				   /*definition of boolean variable*/
typedef struct genericNode *gNode; /*definition of pointer to generic struct*/
typedef enum types
{ /*definition of instruction rows type*/
  CODE,
  DATA,
  ENTRY,
  EXT,
  JUMP
} types;
typedef enum address
{ /*definition of operands address type*/
  NO_ADDRESS = -1,
  DIRECT = 1 << 0,
  IMMEDIATE = 1 << 1,
  DIRECT_REG = 1 << 2
} address;

/** createNode
 * creates node consist of generic struct and initializing the variables
 * @param name - the name of the node - Macro name/label/row operands etc.
 * @param address - address of the row in ob file (sometimes intialize as 0 = none) (IC,DC)
 * @return pointer to created node
 * */
gNode createNode(char *name, int address);

/** search
 * searching name in linked list
 * @param list - which linked list (macro/labels/row/suspectlabels)
 * @param name - the name of the label for searching
 * @return pointer to founded node or NULL
 * */
gNode search(gNode *list, char *name);

/** cmpListsName
 * compares to linked list - searching for name from toCmp in src
 * @param src - in which linked list looking for
 * @param toCmp - name from here comparing to name from src
 * @return pointer to founded node or NULL
 * */
gNode cmpListsName(gNode *src, gNode toCmp);

/** cmpListDeleteSameName
 * compares to linked list - searching for name from toCmp in src and deleting the node that is the same
 * @param willBeDeleted - name from here comparing to name from src and deleting if true
 * @param src - in which linked list looking for
 * */
void cmpListDeleteSameName(gNode *src, gNode *willBeDeleted);

/** insert
 * inserting node into linked list
 * @param HEAD - to which linked list add the node
 * @param newNode - the node that will be added to linked list
 * */
void insert(gNode *HEAD, gNode newNode);

void deleteHead(gNode *HEAD);					/*delete the head of the linked list*/
void deleteNode(gNode *HEAD, gNode deleteNode); /*delete certain node in linked list*/

void setName(gNode list, char name[]);			 /*setting the name in certain node*/
void setLineNum(gNode list, int lineNum);		 /*setting the line number in the am file*/
void setStart(gNode list, int start);			 /*setting the beginning of the macro*/
void setEnd(gNode list, int end);				 /*setting the end of the macro*/
void setAddress(gNode list, int address);		 /*setting the address of label in ob file*/
void setNumOfOps(gNode list, int NumOps);		 /*setting the number of operands that calculated*/
void setCommand(gNode list, int commandIndex);	 /*setting the command number 0-15*/
void setOpType(gNode list, int index, int type); /*setting the operand type - immidiate/direct/direct_reg*/
void setOp(gNode list, int index, int content);	 /*setting the immidiate and register operands*/
void setARE(gNode list, int ARE);				 /*setting the A/R/E address*/
void setLabel1(gNode list, char label1[]);		 /*setting the label of first operand*/
void setLabel2(gNode list, char label2[]);		 /*setting the label of second operand*/
void setLabel3(gNode list, char label3[]);		 /*setting the label of third operand*/
void setType(gNode list, types type);			 /*setting the label type - CODE/JUMP/DATA/EXT/ENT*/
void setNext(gNode list, gNode toSet);			 /*setting the next pointer to the next struct*/

char *getName(gNode list);			  /*getting the name in certain node*/
int getLineNum(gNode list);			  /*getting the line number in the am file*/
int *getStartMacro(gNode list);		  /*getting the beginning of the macro*/
int *getEndMacro(gNode list);		  /*getting the end of the macro*/
int getAddress(gNode list);			  /*getting the address of label in ob file*/
int getNumOfOps(gNode list);		  /*getting the number of operands that calculated*/
int getCommand(gNode list);			  /*getting the command number 0-15*/
int getOpType(gNode list, int index); /*getting the operand type - immidiate/direct/direct_reg*/
int getOp(gNode list, int index);	  /*getting the immidiate and register operands*/
int getARE(gNode list);				  /*getting the A/R/E address*/
char *getLabel1(gNode list);		  /*getting the label of first operand*/
char *getLabel2(gNode list);		  /*getting the label of second operand*/
char *getLabel3(gNode list);		  /*getting the label of third operand*/
types getType(gNode list);			  /*getting the label type - CODE/JUMP/DATA/EXT/ENT*/
/** getNext
 * getting the next node in the list
 * @param list - which linked list (macro/labels/row/suspectlabels)
 * @return pointer to founded node or NULL
 * */
gNode *getNext(gNode list);

char *getOpName(int index); /*getting the operand name from opCode table*/
int getOpNum(int index);	/*getting the operand numberfrom opCode table*/
int getOpSrc(int index);	/*getting the operand src type (bitmask for IM/DIR/DIR_REG) from opCode table*/
int getOpDest(int index);	/*getting the operand dest type (bitmask for IM/DIR/DIR_REG) from opCode table*/

/** isMcrValid
 * checks if the inserted macro name (token) is valid (not command or .string .data)
 * @param mcrName - pointer to the certain suspect macro name
 * @return true/false
 * */
bool isMcrValid(char *mcrName); /*checks if the macro name is the name of instructor or directive*/

void freeList(gNode HEAD);
#endif