#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "structures.h"
#include "binaryBuild.h"
#include "firstPass.h"
#include "registerTable.h"

void binaryCode(FILE *obFile, gNode rowData, gNode labels, int IC)
{
    writeCODE(obFile, rowData, labels);
    DATAparts(obFile, rowData);
    buildFromBinary(obFile, IC);
    printf("\tob file was created successfully\n");
}

void writeCODE(FILE *obFile, gNode rowData, gNode labels)
{
    /**
     * definition of bytes in the general binary code
     * @param opDest define the destination operand type (bytes 2-3)
     * @param opSrc define the source operand type (bytes 4-5)
     * @param opCode define the opcode of command (bytes 6-9)
     * @param param2 define the second param type (bytes 10-11)
     * @param param1 define the first param type (bytes 12-13)
     * */

    int address;
    int param1 = 0;
    int param2 = 0;
    int opCode, opSrc, opDest, opType1, opType2, opType3, op1, op2, op3, final;

    if (rowData == NULL)
    {
        return;
    }
    writeCODE(obFile, getNext(rowData), labels);

    if (getType(rowData) != DATA)
    {
        address = getAddress(rowData);
        opType1 = getOpType(rowData, 1);
        opType2 = getOpType(rowData, 2);
        opType3 = getOpType(rowData, 3);

        if (opType1 == DIRECT_REG)
            opType1 = 3;
        else if ((opType1 == IMMEDIATE) || (opType1 == NO_ADDRESS))
            opType1 = 0;

        if (opType2 == DIRECT_REG)
            opType2 = 3;
        else if ((opType2 == IMMEDIATE) || (opType2 == NO_ADDRESS))
            opType2 = 0;

        if (opType3 == DIRECT_REG)
            opType3 = 3;
        else if ((opType3 == IMMEDIATE) || (opType3 == NO_ADDRESS))
            opType3 = 0;

        if (getType(rowData) == JUMP)
        {
            param1 = opType2;
            param2 = opType3;
            opSrc = 0;
            opDest = 2;
        }
        else
        {
            if ((getNumOfOps(rowData) == 1) && ((opType1 & opType2 & 3) == 0))
            {
                opSrc = 0;
                opDest = opType1;
            }
            else
            {
                opSrc = opType1;
                opDest = opType2;
            }
        }

        opCode = getCommand(rowData);
        /*********************************************************************************************/

        opDest = opDest << 2;
        opSrc = opSrc << 4;
        opCode = opCode << 6;
        param2 = param2 << 10;
        param1 = param1 << 12;
        final = param1 + param2 + opCode + opSrc + opDest;

        writeBinary(final, address, obFile);
        writeBinaryParts(obFile, rowData, labels);
    }
}

void writeBinaryParts(FILE *obFile, gNode row, gNode labels)
{
    int i = getNumOfOps(row);
    int count = 1;
    int rowAddress;
    int op1, op2, op3, final, ARE;
    rowAddress = getAddress(row);
    if (getType(row) == JUMP)
    {
        ARE = getAREOfLabel(row, labels, 1);
        op1 = getAddressOfLabel(row, labels, 1) << 2;
        op1 = op1 + ARE;

        if (getOpType(row, 2) == DIRECT)
        {
            ARE = getAREOfLabel(row, labels, 2);
            op2 = getAddressOfLabel(row, labels, 2) << 2;
            op2 = op2 + ARE;
        }
        if (getOpType(row, 3) == DIRECT)
        {
            ARE = getAREOfLabel(row, labels, 3);
            op3 = getAddressOfLabel(row, labels, 3) << 2;
            op3 = op3 + ARE;
        }

        if (getOpType(row, 2) == IMMEDIATE)
        {
            op2 = getOp(row, 2) << 2;
        }
        if (getOpType(row, 3) == IMMEDIATE)
        {
            op3 = getOp(row, 3) << 2;
        }

        if (getOpType(row, 2) == DIRECT_REG)
        {
            op2 = getOp(row, 2) << 8;
        }
        if (getOpType(row, 3) == DIRECT_REG)
        {
            op3 = getOp(row, 3) << 2;
        }

        if (getOpType(row, 2) == DIRECT_REG && getOpType(row, 3) == DIRECT_REG) /*registers are at source and destination*/
        {
            i--;
            op2 = op2 + op3;
        }
    }
    else
    {
        if (getOpType(row, 1) == DIRECT)
        {
            ARE = getAREOfLabel(row, labels, 1);
            op1 = getAddressOfLabel(row, labels, 1) << 2;
            op1 = op1 + ARE;
        }
        if (getOpType(row, 2) == DIRECT)
        {
            ARE = getAREOfLabel(row, labels, 2);
            op2 = getAddressOfLabel(row, labels, 2) << 2;
            op2 = op2 + ARE;
        }

        if (getOpType(row, 1) == IMMEDIATE)
        {
            op1 = getOp(row, 1) << 2;
        }
        if (getOpType(row, 2) == IMMEDIATE)
        {
            op2 = getOp(row, 2) << 2;
        }

        if (getOpType(row, 1) == DIRECT_REG)
        {
            op1 = getOp(row, 1) << 8;
        }
        if (getOpType(row, 2) == DIRECT_REG)
        {
            op2 = getOp(row, 2) << 2;
        }

        if (getOpType(row, 1) == DIRECT_REG && getOpType(row, 2) == DIRECT_REG) /*registers are at source and destination*/
        {
            i--;
            op1 = op1 + op2;
        }
    }
    while (count <= i)
    {
        if (count == 1)
        {
            writeBinary(op1, (rowAddress + count), obFile);
        }
        else if (count == 2)
        {
            writeBinary(op2, (rowAddress + count), obFile);
        }
        else if (count == 3)
        {
            writeBinary(op3, (rowAddress + count), obFile);
        }
        count++;
    }
}

void DATAparts(FILE *obFile, gNode rowData)
{
    int address, num;
    int count = 0;
    char *expression, *originExp;

    if (rowData == NULL)
    {
        return;
    }
    DATAparts(obFile, getNext(rowData));

    if (getType(rowData) == DATA)
    {
        expression = getName(rowData);
        address = getAddress(rowData);
        if ((*expression) != '"') /*data*/
        {
            writeData(obFile, rowData);
        }
        else /*String*/
        {
            writeString(obFile, rowData);
        }
    }
}

void writeData(FILE *obFile, gNode rowData)
{
    int address, num;
    int count = 0;
    char *expression, *originExp;
    expression = getName(rowData);
    address = getAddress(rowData);
    originExp = expression;
    while (true)
    {
        if (*expression == ',')
        {
            num = atoi(originExp);
            writeBinary(num, (address + count), obFile);
            originExp = expression + 1;
            count++;
        }
        if (endOfLine(expression) == EOL)
        {
            num = atoi(originExp);
            writeBinary(num, (address + count), obFile);
            originExp = expression + 1;
            count++;
            break;
        }
        expression += 1;
    }
}

void writeString(FILE *obFile, gNode rowData)
{
    int address, num;
    int count = 0;
    char *expression, *originExp;
    expression = getName(rowData);
    address = getAddress(rowData);
    expression += 1;
    while (*expression != '"')
    {
        num = *expression;
        writeBinary(num, (address + count), obFile);
        expression += 1;
        count++;
    }
    writeBinary(0, (address + count), obFile); /* writting '\0' at the end*/
}

void writeBinary(int num, int address, FILE *obFile)
{
    int i = 0;
    int numToCompare = pow(2, 13);

    while (i < 14)
    {
        if (num & numToCompare)
            fputs("1", obFile);
        else
            fputs("0", obFile);
        i++;
        numToCompare = numToCompare >> 1;
    }
    fputs("\n", obFile);
}

int getAddressOfLabel(gNode row, gNode labels, int index)
{
    if (index == 1)
    {
        if (search(&labels, getLabel1(row)) == NULL)
            return 0;
        else
            return getAddress(search(&labels, getLabel1(row))) + 100;
    }
    else if (index == 2)
    {
        if (search(&labels, getLabel2(row)) == NULL)
            return 0;
        else
            return getAddress(search(&labels, getLabel2(row))) + 100;
    }
    else if (index == 3)
    {
        if (search(&labels, getLabel3(row)) == NULL)
            return 0;
        else
            return getAddress(search(&labels, getLabel3(row))) + 100;
    }
}

int getAREOfLabel(gNode row, gNode labels, int index)
{
    if (index == 1)
    {
        if (search(&labels, getLabel1(row)) == NULL)
            return 0;
        else
            return getARE(search(&labels, getLabel1(row)));
    }
    else if (index == 2)
    {
        if (search(&labels, getLabel2(row)) == NULL)
            return 0;
        else
            return getARE(search(&labels, getLabel2(row)));
    }
    else if (index == 3)
    {
        if (search(&labels, getLabel3(row)) == NULL)
            return 0;
        else
            return getARE(search(&labels, getLabel3(row)));
    }
}