#ifndef REGISTER_TABLE
#define REGISTER_TABLE

#define REGISTER_NUM 8
#define false 0
#define true 1

int getRegisterNum(char *name);   /*getting the register number (0/1/2/.../7) from register table struct*/
char *getRegisterName(int index); /*getting the register full name ('r1'/'r2'/..etc.)*/

#endif