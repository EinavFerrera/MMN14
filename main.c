#include "preAssembler.h"

int main(int argc, char *argv[])
{
	if (argc == 1)
	{
		printf("Please enter a name of file!\n");
		return 0;
	}

	/*loop that call compiling method for each file*/
	else
	{
		int i;
		/* start from 1 because the first one is the name of the program */
		for (i = 1; i < argc; i++)
		{
			compileFile(argv[i]);
		}
	}
	return 0;
}
