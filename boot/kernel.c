/* Check if the compiler thinks we are targeting the wrong operating system. */
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

/* This tutorial will only work for the 32-bit ix86 targets. */
#if !defined(__i386__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif


#include "k_defines.h"
#include "io.h"
#include "stdio.h"
#include "idt.h"
#include "pic.h"
#include "panic.h"
#include "descriptor.h"
#include "interrupts.h"

#define STACK_SIZE	512	/* Minimum number of bytes for stack */

struct boot_params boot_params __attribute__((aligned(16)));

char *HEAP = _end;
char *heap_end = _end;		/* Default end of heap = no heap */


/* this is what is called by our test int */
void testint(void);

void init_keyboard(void)
{
    struct biosregs ireg, oreg;
    initregs(&ireg);

    ireg.ah = 0x02;	// get status
    intcall(0x16, &ireg, &oreg);
    boot_params.kbd_status = oreg.al;

    ireg.ax = 0x0305;	// set keyboard repeat rate
    intcall(0x16, &ireg, NULL);
}

void init_heap(void)
{
    char *stack_end;
	if (boot_params.hdr.loadflags & CAN_USE_HEAP) {
		asm("leal %P1(%%esp),%0"
		    : "=r" (stack_end) : "i" (-STACK_SIZE));

		heap_end = (char *)
			((size_t)boot_params.hdr.heap_end_ptr + 0x200);
		if (heap_end > stack_end)
			heap_end = stack_end;
	} else {
		/* Boot protocol 2.00 only, no heap available */
		puts("WARNING: Ancient bootloader, some functionality "
		     "may be limited!\n");
	}
}

BootMain()
{
    init_console();
    init_heap();
    init_keyboard();


    puts("KALLUP.NET\nTEST");

	for (;;);

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
