#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "structures.h"
#include "firstPass.h"
#include "binaryBuild.h"

/* 0,1 - ARE
2,3,4 - dest
5,6,7,8 - opt
9,10,11 - src

SHIMI:
0,1 - ARE
2,3 - dest
4,5 - src
6,7,8,9 - opt
10,11 - par2
12,13 - par1
*/
void overWrite(FILE *obFile, char *place);

void buildFromBinary(FILE *obFile, int IC)
{

    char line[14];
    char newLine[12], newLineA[6], newLineB[6];
    char src[3], dest[3], opt[4], temp[3], ARE[2];
    char A, B;
    int counter = 0;
    char *obFileBinary;
    printf("1print_%s_\n", obFileBinary);
    overWrite(obFile, obFileBinary);
    printf("2print_%s_\n", obFileBinary);
    // fgets(line, sizeof(line), obFileBinary);
    printf("3print_%s_\n", line);
    char *obFileBase64 = (char *)(malloc(sizeof(obFileBinary) * (sizeof(char))));

    while (fgets(line, sizeof(line), obFile) != NULL) // stops here !
    {
        {
            printf("this line:_%s__\n", line);

            if (counter < IC)
            {                                         /*IC part*/
                if (line[0] != '.' || line[1] != '.') /* had a prob that prints illigel commands*/
                    continue;
                strncpy(temp, line[4], 2);
                strcpy(src, srcDestConvert(temp));
                strncpy(temp, line[2], 2);
                strcpy(dest, srcDestConvert(temp));
                strncpy(opt, line[6], 4);
                strncpy(ARE, line[0], 2);
                sprintf(&newLine, "%s%s%s%s", src, opt, dest, ARE);
                strncpy(newLineA, newLine[6], 6);
                strncpy(newLineB, newLine[0], 6);
            }
            else /* DC part*/
            {
                strncpy(newLineA, line[6], 6);
                strncpy(newLineB, line[0], 6);
            }

            A = digitToBase64(newLineA); /* bits 11,10,9,8,7,6 ->base 64*/
            B = digitToBase64(newLineB); /* bits  5, 4,3,2,1,0 ->base 64*/
            fputs(A, obFileBase64);
            fputs(B, obFileBase64);
            fputs("\n", obFileBase64);
            counter++;
        }

        fputs(obFileBase64, obFileBinary); /* overwrite obFile with the base 64*/
        free(obFileBase64);
        // fclose("test.ob");
    }
}
char *srcDestConvert(char *temp)
{
    if (temp[0] == '0' && temp[1] == '0')
        return "001";
    if (temp[0] == '0' && temp[1] == '1')
        return "011";
    if (temp[0] == '1' && temp[1] == '1')
        return "101";
}
char digitToBase64(char *digit)
{
    int number;
    number = (int)strtol(digit, NULL, 2);
    if (number >= 0 && number <= 25) /* number between A(0 - 65) and Z(25 - 90)*/
        return number + 65;
    if (number >= 26 && number <= 51) /* number between a(26 - 97) and z(51 - 122)*/
        return number + 71;
    if (number >= 52 && number <= 61) /* number between 0(52 - 48) and 9(61 - 57)*/
        return number - 4;
    if (number = 62) /* return + */
        return 43;
    if (number = 63) /* return / */
        return 47;
}
void overWrite(FILE *obFile, char *place)
{
    FILE *file = fopen("test.ob", "w"); // Replace "example.txt" with your file path

    if (file == NULL)
    {
        printf("Failed to open the file.\n");
        return;
    }
    // Determine the file size
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);

    // Allocate memory to store the file contents
    place = (char *)malloc(size + 1); // Add 1 for the null terminator
    if (place == NULL)
    {
        printf("Failed to allocate memory.\n");
        fclose(file);
        return;
    }

    // Read the file contents into the buffer
    fread(place, size, 1, file);
    place[size] = '\0'; // Null-terminate the buffer
    printf("print place_%s_\n", place);
    return;
}
