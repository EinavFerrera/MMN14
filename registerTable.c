#include "registerTable.h"
#include "errors.h"
#include <string.h>

static const struct registersTable
{
    const char *name;
    int num;
} registers[] = {{"@r0", 0}, // added @ to registers
                 {"@r1", 1},
                 {"@r2", 2},
                 {"@r3", 3},
                 {"@r4", 4},
                 {"@r5", 5},
                 {"@r6", 6},
                 {"@r7", 7}};

int getRegisterNum(char *name)
{
    int i = 0;
    while (i < REGISTER_NUM)
    {
        if (strcmp(name, registers[i].name) == 0)
        {
            return registers[i].num;
        }
        i++;
    }
    return NOT_REGISTER;
}

char *getRegisterName(int index)
{
    if ((index < REGISTER_NUM) && (index >= 0))
        return registers[index].name;
    else
        return NOT_REGISTER;
}