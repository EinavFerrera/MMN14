#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "structures.h"
#include "firstPass.h"
#include "binaryBuild.h"

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
                counter++;
                continue;
            }
            strncpy(temp, line + 8, 2); /*src*/
            strcpy(src, srcDestConvert(temp));
            src[3] = '\0';

            strncpy(temp, line + 10, 2); /*dest*/
            strcpy(dest, srcDestConvert(temp));
            dest[3] = '\0';

            strncpy(opt, line + 4, 4);
            opt[4] = '\0';
            strncpy(ARE, line + 12, 2);
            ARE[3] = '\0';
            sprintf(&newLine, "%s%s%s%s", src, opt, dest, ARE);
            newLine[12] = '\0';
            strncpy(newLineA, newLine, 6);
            newLineA[6] = '\0';
            strncpy(newLineB, newLine + 6, 6);
            newLineB[6] = '\0';
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

void overWrite(FILE *obFile, char *place)
{
    FILE *file = fopen("test.ob", "w"); /* Replace "example.txt" with your file path*/

    if (file == NULL)
    {
        printf("Failed to open the file.\n");
        return;
    }
    /* Determine the file size*/
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);

    place = (char *)malloc(size + 1);
    if (place == NULL)
    {
        printf("Failed to allocate memory.\n");
        fclose(file);
        return;
    }

    fread(place, size, 1, file);
    place[size] = '\0';
    printf("print place_%s_\n", place);
    return;
}
