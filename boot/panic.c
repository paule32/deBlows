#include "k_defines.h"
#include "interrupts.h"
#include "descriptor.h"
#include "pic.h"


void LoadExceptions()
{
	/*
	 * Add all Exception Interrupts
	 */
    AddInt(0x00, int00, 0);
    AddInt(0x01, int01, 0);
    AddInt(0x02, int02, 0);
    AddInt(0x03, int03, 0);
    AddInt(0x04, int04, 0);
    AddInt(0x05, int05, 0);
    AddInt(0x06, int06, 0);
    AddInt(0x07, int07, 0);
    AddInt(0x08, int08, 0);
    AddInt(0x09, int09, 0);
    AddInt(0x0a, int10, 0);
    AddInt(0x0b, int11, 0);
    AddInt(0x0c, int12, 0);
    AddInt(0x0d, int13, 0);
    AddInt(0x0e, int14, 0);
    AddInt(0x0f, int15, 0);

    AddInt(0x10, int16, 0);	// video interrupt !

    AddInt(0x11, int17, 0);
    AddInt(0x12, int18, 0);
    AddInt(0x13, int19, 0);
    AddInt(0x14, 0, 0);
    AddInt(0x15, 0, 0);

    AddInt(0x16, 0, 0);	// keyboard interrupt !

    AddInt(0x17, 0, 0);
    AddInt(0x18, 0, 0);
    AddInt(0x19, 0, 0);
    AddInt(0x1a, 0, 0);
    AddInt(0x1b, 0, 0);
    AddInt(0x1c, 0, 0);
    AddInt(0x1d, 0, 0);
    AddInt(0x1e, 0, 0);
    AddInt(0x1f, 0, 0);
    AddInt(0x20, 0, 0);
    AddInt(0x21, 0, 0);
}

void panic(char *message, char *code, bool halt)
{

//     clear_screen();

	print("<SYSTEM ERROR>:",8);
	print(message,9);

	if(halt == false)
	{
		print("A Fatal!!!",10);
		__asm__(
		"cli\n"
		"hlt\n");
	}
}

/*
 * Exception Handlers
 */
void int_00(void)
{
	panic("Divide By Zero Error","#00", false);
}

void int_01(void)
{
	panic("Debug Error","#DB", false);
}

void int_02(void)
{
	panic("NMI Interrupt","#--", false);
}

void int_03(void)
{
	panic("Breakpoint","#BP", false);
}

void int_04(void)
{
	panic("Overflow","#OF", false);
}

void int_05(void)
{
	panic("BOUND Range Exceeded","#BR", false);
}

void int_06(void)
{
	panic("Invalid Opcode","#UD", false);
}

void int_07(void)
{
	panic("Device Not Available","#NM", false);
}

void int_08(void)
{
	panic("Double Fault","#DF", true);
}

void int_09(void)
{
	panic("Coprocessor Segment Overrun", "#NA", false);
}

void int_10(void)
{
	panic("Invalid TSS","#TS", false);
}

void int_11(void)
{
	panic("Segment Not Present","#NP", false);
}

void int_12(void)
{
	panic("Stack Segment Fault","#SS", false);
}

void int_13(void)
{
	panic("Gneral Protection Fault","#GP", false);
}

void int_14(void)
{
	panic("Page Fault","#PF", false);
}

void int_15(void)
{
	panic("reserved INT - not implemented!","#ND", false);
}

void int_16(void)
{
	vga_test();
	print("inter 0x10 called",10);

	//panic("FPU Floating-Point Error","#MF", false);
}

void int_17(void)
{
	panic("Alignment Check","#AC", false);
}

void int_18(void)
{
	panic("Machine Check","#MC", true);
}

void int_19(void)
{
	panic("SIMD Floating-Point","#XF", false);
}
