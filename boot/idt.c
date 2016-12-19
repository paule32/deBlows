#include "k_defines.h"
#include "descriptor.h"

idtr IDTR;
x86_interrupt IDT[256];// = (x86_interrupt*)0xD00;

void loadIDTR()
{
    IDTR.limit  = 256*(sizeof(x86_interrupt)-1);
    IDTR.base   = IDT;

    idtr *IDTRptr = &IDTR;
    /* load IDTR with lidt */
	asm volatile("LIDT %0 ": :"m" (IDTR));

//    asm volatile("LIDT (%0) ": :"p" (IDTRptr));
}

void AddInt(int number, void (*handler)(), dword dpl)
{
	 word selector = 0x08;
	 word settings;

	dword offset = (dword)handler;

	 /* get CS selector */
	asm volatile("movw %%cs,%0" :"=g"(selector));

	 /* set settings options depending on dpl */
	 switch(dpl)
	 {
	 case 0: settings = INT_0; break;
	 case 1:
	 case 2:
	 case 3: settings = INT_3; break;
	 }

	 /* set actual values of int */
	 IDT[number].low_offset   = (offset & 0xFFFF);
	 IDT[number].selector     = selector;
	 IDT[number].settings     = settings;
 	 IDT[number].high_offset  = (offset >> 16);
}
