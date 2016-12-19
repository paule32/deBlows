#include "k_defines.h"

void
BootMain_older()
{
	INTS(false);
	print("IDT kernel is currently loaded @ 1MB Physical.", 2);

	for (;;);
}

