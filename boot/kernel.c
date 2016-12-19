#include "k_defines.h"
#include "io.h"
#include "stdio.h"
#include "idt.h"
#include "pic.h"
#include "panic.h"
#include "descriptor.h"
#include "interrupts.h"


/* this is what is called by our test int */
void testint(void);

BootMain() 
{
	/* This function turns off interrupts */
	INTS(false);
	/* clears the screen, duh! */
	clear_screen();

	while(1);


	/* Tell the user where we loaded to */
	print("IDT kernel is currently loaded @ 1MB Physical.", 2);
	
	/* Remap PIC IRQs to cover software interrupts 0x20-0x30 */
	remapPIC(0x20,0x28);
	/* Mask the IRQs so they don't mess anything up */
	maskIRQ(ALL);
	/* Ok, we re-routed the IRQs */
	print("8259 PIC Remapped.", 1);


	/* Here we load the Exceptions that are called when certain faulty ocurr */
	LoadExceptions();
	/* Oh Ya! Just a few more steps and we can load our interrupt */
	print("Exceptions Loaded.", 2);



	/* Add our custom interrupt | We will load it as the 48th interrupt and point
	 * to the int32() asm function in interrupts.asm */
	AddInt(48, int32, 0);
	/* Ya Baby! Now we just tell the PC where all the IDT data is and we can call
	 * our custom interrupt */
	print("3 Test Interrupts Loaded.", 3);

	/* Point the IDT register to our IDT array */
	loadIDTR();
	/* We did it. Now to just call our interrupt */
	print("IDT initialised.", 4);

	/* We enable interrupts again so we can call our interrupt */
	INTS(true);

	/* Ok, moment of truth. Drum roll please! */
	asm("int $48");

	/* And that's the end of the IDT Kernel example code :) */
	return;
}

/*
 * Our Cuxtom Interrupt
 */
void testint(void)
{
	/* Clear the screen */
//	clear_screen();
	/* Print out our message */
	print("TEST INT CALLED",2);
}
