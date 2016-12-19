#include "k_defines.h"
#include "interrupts.h"
#include "descriptor.h"
#include "pic.h"


void LoadExceptions()
{
	/*
	 * Add all Exception Interrupts
	 */
	AddInt(0, int00, 0);
    AddInt(1, int01, 0);
    AddInt(2, int02, 0);
    AddInt(3, int03, 0);
    AddInt(4, int04, 0);
    AddInt(5, int05, 0);
    AddInt(6, int06, 0);
    AddInt(7, int07, 0);
    AddInt(8, int08, 0);
    AddInt(9, int09, 0);
    AddInt(10, int10, 0);
    AddInt(11, int11, 0);
    AddInt(12, int12, 0);
    AddInt(13, int13, 0);
    AddInt(14, int14, 0);
    AddInt(16, int16, 0);
    AddInt(17, int17, 0);
    AddInt(18, int18, 0);
    AddInt(19, int19, 0);
	AddInt(20, 0, 0);
	AddInt(21, 0, 0);
	AddInt(22, 0, 0);
	AddInt(23, 0, 0);
	AddInt(24, 0, 0);
	AddInt(25, 0, 0);
	AddInt(26, 0, 0);
	AddInt(27, 0, 0);
	AddInt(28, 0, 0);
	AddInt(29, 0, 0);
	AddInt(30, 0, 0);
	AddInt(31, 0, 0);
}

void panic(char *message, char *code, bool halt)
{

     clear_screen();

     print("<SYSTEM ERROR>\n",0);

     if(halt==true)
     {
          print("A Fatal ",2);
     }
	 if(halt==false)
	 {
		 print("A Non Fatal ",2);
	 }

     print("Exception Has Occured(your system has messed up)",5);



     if(halt==true)
     {
          print("\n\n\n\n\n\n\t\t\t\t\t>>SYSTEM HALTED<<",15);
		  asm("cli\n");
		  asm("hlt\n"); //stop pc
     } 
	 else if(halt==false)
	 {
          print("\n\n\n\n\n\t\t\t\t\t<Press Any Key To Continue>",15);
	 }
	 outportb(MASTER, EOI); //send PIC EOI command
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

void int_16(void)
{
	panic("FPU Floating-Point Error","#MF", false);
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
