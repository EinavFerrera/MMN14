#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "structures.h"
#include "firstPass.h"
#include "binaryBuild.h"
/*
index:
0   1  2  3  4  5  6  7  8  9  10  11
11 10  9  8  7  6  5  4  3  2  1   0
[src   ]  [opcode  ]  [dest ]  [ARE]
*/

/*
index:
 0   1   2   3  4  5  6  7  8  9  10  11 12  13
13  12  11  10  9  8  7  6  5  4  3   2  1   0
[pa1  ] [par2] [opcode   ] [src] [dest]  [ARE]
*/

void overWrite(FILE *obFile, char *place);

void buildFromBinary(FILE *obFile, FILE *otFile, int IC)
{
    char line[20];
    char newLine[16], newLineA[7], newLineB[7];
    char src[4], dest[4], opt[5], temp[3], ARE[3];
    char A, B;
    int counter = 0;

    while (fgets(line, sizeof(line), obFile) != NULL)
    {

        printf("this line:_%s_\n", line);
        if (counter == 0) /*first line AS IS*/
        {
            fputs(line, otFile);
            fputs("\n", otFile);
            counter++;
            continue;
        }

        if (counter <= IC)
        {                                                                             /*IC part*/
            if (line[0] != '0' || line[1] != '0' || line[2] != '0' || line[3] != '0') /* had a prob that prints illigel commands*/
            {
                printf("fail !!!\n");
                counter++;
                continue;
            }
            strncpy(temp, line + 8, 2); /*src*/
            printf("this is temp _%s_\n", temp);
            strcpy(src, srcDestConvert(temp));
            src[3] = '\0';
            printf("this is src  _%s_\n", src);
            printf("this is temp after src  _%s_\n", temp);

            strncpy(temp, line + 10, 2); /*dest*/
            strcpy(dest, srcDestConvert(temp));
            dest[3] = '\0';
            printf("this is dest  _%s_\n", dest);
            printf("this is temp after dest  _%s_\n", temp);

            strncpy(opt, line + 4, 4);
            opt[4] = '\0';
            printf("this is opt  _%s_\n", opt);
            strncpy(ARE, line + 12, 2);
            ARE[3] = '\0';
            printf("this is ARE  _%s_\n", ARE);
            sprintf(&newLine, "%s%s%s%s", src, opt, dest, ARE);
            newLine[12] = '\0';
            printf("this is new line  _%s_\n", newLine);
            strncpy(newLineA, newLine, 6);
            newLineA[6] = '\0';
            strncpy(newLineB, newLine + 6, 6);
            newLineB[6] = '\0';
            printf("i made it ! _%s_%s_\n", newLineA, newLineB);
        }
        else /* DC part*/
        {
            strncpy(newLineA, line + 2, 6);
            newLineA[6] = '\0';
            strncpy(newLineB, line + 8, 6);
            newLineB[6] = '\0';
            fputc('-', otFile);
        }

        A = digitToBase64(newLineA); /* bits 11,10,9,8,7,6 ->base 64*/
        B = digitToBase64(newLineB); /* bits  5,4,3,2,1,0 ->base 64*/
        fputs(newLineA, otFile);
        fputc(' ', otFile);
        fputs(newLineB, otFile);
        fputs("\t", otFile);
        fputc(A, otFile);
        fputc(B, otFile);
        fputs("\n", otFile);
        counter++;
    }
}
char *srcDestConvert(char *temp)
{
    char *stringToReturn;
    printf("are you her ?!\n");
    if (temp[0] == '0' && temp[1] == '0')
    {
        stringToReturn = "001";
        return stringToReturn;
    }
    if (temp[0] == '0' && temp[1] == '1')
    {
        stringToReturn = "011";
        return stringToReturn;
    }
    if (temp[0] == '1' && temp[1] == '1')
    {
        stringToReturn = "101";
        return stringToReturn;
    }
}
char digitToBase64(char *digit)
{
    int number;
    number = (int)strtol(digit, NULL, 2);
    printf("line in:_%s_\t", digit);
    printf("number:_%d_\n", number);
    if (number >= 0 && number <= 25) /* number between A(0 - 65) and Z(25 - 90)*/
        return number + 65;
    if (number >= 26 && number <= 51) /* number between a(26 - 97) and z(51 - 122)*/
        return number + 71;
    if (number >= 52 && number <= 61) /* number between 0(52 - 48) and 9(61 - 57)*/
        return number - 4;
    if (number == 62) /* return + */
        return 43;
    if (number == 63) /* return / */
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
