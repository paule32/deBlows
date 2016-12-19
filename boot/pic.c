#include "pic.h"


void remapPIC(int pic1, int pic2)
{
	byte md,sd;

	md=inportb(MASTERDATA);                     // save state of MASTER DATA
	sd=inportb(SLAVEDATA);                      // save state of SLAVE DATA

	outportb(MASTER, EOI);                      // Send EOI | resets the chip

	outportb(MASTER, ICW1_INIT+ICW1_ICW4);      // ICW1 control word setup | just basic PIC stuff
	outportb(SLAVE, ICW1_INIT+ICW1_ICW4);       // see pic.h for more details about the values

	outportb(MASTERDATA, pic1);                 // ICW2 maps IRQs 0-7 to whatever kernel passes
	outportb(SLAVEDATA, pic2);                  // and same here except with IRQs 8-15 

	outportb(MASTERDATA, 0x04);                 // ICW3 
	outportb(SLAVEDATA, 0x02);

	outportb(MASTERDATA, ICW4_8086);            // ICW4 control word setup
	outportb(SLAVEDATA, ICW4_8086);

	outportb(MASTERDATA,md);                    // restore both MASTER DATA
	outportb(SLAVEDATA,sd);                     // restore SLAVE DATA
}
	
/* makes IRQ unavailable so it can't fire an interrupt */
void maskIRQ(byte irq)
{
	if(irq==ALL)
	{
		outportb(MASTERDATA,0xFF);
		outportb(SLAVEDATA,0xFF);
	}
	else
	{
		irq = irq | (1<<irq);
		if(irq < 8)
			outportb(MASTERDATA, irq&0xFF);
		else
			outportb(SLAVEDATA, irq>>8);
	}
}

/* opposite of above */
void unmaskIRQ(byte irq)
{
	if(irq==ALL)
	{
		outportb(MASTERDATA,0x00);
		outportb(SLAVEDATA,0x00);
	}
	else
	{
		irq = irq & (1<<irq);
		if(irq < 8)
			outportb(MASTERDATA, irq&0xFF);
		else
			outportb(SLAVEDATA, irq>>8);
	}
}
 /* enables/disables software interrupts */
void INTS(bool on)
{
	if(on)
	{
		asm("sti");
	}
	else
	{
		asm("cli");
	}
}
