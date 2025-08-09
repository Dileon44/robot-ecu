#include "main.h"
#include "platform.h"

#if DEBUG_ENABLE
#warning DEBUG_ENABLE
#endif

int main(void)
{
 	// Pl_JumpToAddr();
}

void ErrorHandler(char *pFile, int line)
{
	DISCARD_UNUNSED(pFile);
	DISCARD_UNUNSED(line);

	Pl_IrqOff();
	PL_SET_BKPT();
	while(1);
}
