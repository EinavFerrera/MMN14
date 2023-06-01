#ifndef binaryBuild
#define binaryBuild

#define false 0                /*boolean false*/
#define true 1                 /*boolean true*/
#define MAXIMUM_FILE_NAME 1000 /* max length of file name  */

/** writeCODE
 * function that arranging the binary code of CODE type instruction
 * @param obFile - pointer to opened ob extension file - writting to this file the binary code
 * @param rowData - list of rows
 * @param labels - list of labels
 * */
void writeCODE(FILE *obFile, gNode rowData, gNode labels);

/** binaryCode
 * main function for building the binary code for ob file
 * @param obFile - pointer to opened ob extension file - writting to this file the binary code
 * @param rowData - list of rows
 * @param labels - list of labels
 * @param IC - number of IC in the end file
 */
void binaryCode(FILE *obFile, gNode rowData, gNode labels, int IC);

/** DATAparts
 * function that preparing the binary code of DATA type instruction
 * @param obFile - pointer to opened ob extension file - writting to this file the binary code
 * @param rowData - list of rows
 * */
void DATAparts(FILE *obFile, gNode rowData);

/** writeBinaryParts
 * function that writting the binary code of all parts of row instruction
 * @param obFile - pointer to opened ob extension file - writting to this file the binary code
 * @param rowData - list of rows
 * @param labels - list of labels
 * */
void writeBinaryParts(FILE *obFile, gNode row, gNode labels);

/** writeData
 * function that writting the binary code of .data instruction including '\0' row at the end of string
 * @param obFile - pointer to opened ob extension file - writting to this file the binary code
 * @param rowData - list of rows
 * */
void writeData(FILE *obFile, gNode rowData);

/** writeString
 * function that writting the binary code of .string instruction including '\0' row at the end of string
 * @param obFile - pointer to opened ob extension file - writting to this file the binary code
 * @param rowData - list of rows
 * */
void writeString(FILE *obFile, gNode rowData);

/** writeBinary
 * function that actually writting the binary code to ob extension file
 * @param num - the prepared number for 14 bytes to write into the file
 * @param address - the prepared address for 14 bytes to write into the file
 * @param obFile - the path of opened .ob file
 * */
void writeBinary(int num, int address, FILE *obFile);

/** getAddressOfLabel
 * getting the address of certain label and operand - looking for the node in rows and taking the address from labels
 * @param row - the list of rows
 * @param labels - the list of labels
 * @param index - the number of certain operand
 * @return address - IMM/DIR/DIR_REG
 * */
int getAddressOfLabel(gNode row, gNode labels, int index);

/** getAREOfLabel
 * getting the A/R/E of address of certain label and operand - looking for the node in rows and taking the address from labels
 * @param row - the list of rows
 * @param labels - the list of labels
 * @param index - the number of certain operand
 * @return A/R/E - 0/1/2 - (binry - 00/10/01)
 * */
int getAREOfLabel(gNode row, gNode labels, int index);

#endif
