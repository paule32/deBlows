#include "k_defines.h"
#include "io.h"
#include "stdio.h"
#include "idt.h"
#include "pic.h"
#include "panic.h"
#include "descriptor.h"
#include "interrupts.h"

#define VBE_DISPI_IOPORT_INDEX 0x01CE
#define VBE_DISPI_IOPORT_DATA  0x01CF
#define VBE_DISPI_INDEX_ID              0x0
#define VBE_DISPI_INDEX_XRES            0x1
#define VBE_DISPI_INDEX_YRES            0x2
#define VBE_DISPI_INDEX_BPP             0x3
#define VBE_DISPI_INDEX_ENABLE          0x4
#define VBE_DISPI_INDEX_BANK            0x5
#define VBE_DISPI_INDEX_VIRT_WIDTH      0x6
#define VBE_DISPI_INDEX_VIRT_HEIGHT     0x7
#define VBE_DISPI_INDEX_X_OFFSET        0x8
#define VBE_DISPI_INDEX_Y_OFFSET        0x9

#define VBE_DISPI_DISABLED              0x00
#define VBE_DISPI_ENABLED               0x01
#define VBE_DISPI_GETCAPS               0x02
#define VBE_DISPI_8BIT_DAC              0x20
#define VBE_DISPI_LFB_ENABLED           0x40
#define VBE_DISPI_NOCLEARMEM            0x80

void VbeWrite (uint16_t index, uint16_t value) {

   outw(VBE_DISPI_IOPORT_INDEX, index);
   outw(VBE_DISPI_IOPORT_DATA, value);
}

void SetVideoMode (int xres, int yres, uint16_t bpp) {

   VbeWrite (VBE_DISPI_INDEX_ENABLE, VBE_DISPI_DISABLED);
   VbeWrite (VBE_DISPI_INDEX_XRES, xres);
   VbeWrite (VBE_DISPI_INDEX_YRES, yres);
   VbeWrite (VBE_DISPI_INDEX_BPP, bpp);
   VbeWrite (VBE_DISPI_INDEX_ENABLE, VBE_DISPI_ENABLED | VBE_DISPI_LFB_ENABLED);   
}

void SetVideoBank(uint16_t bank) { VbeWrite(VBE_DISPI_INDEX_BANK, bank); }

typedef struct __attribute__ ((packed)) {
    unsigned short di, si, bp, sp, bx, dx, cx, ax;
    unsigned short gs, fs, es, ds, eflags;
} regs16_t;
 
// tell compiler our int32 function is external
extern void pm_int32(unsigned char intnum, regs16_t *regs);

void testint(void);

char *VGA = (char *)0xA0000;

void plot_fast(int x,int y,byte color)
{
  VGA[y*320+x]=color;
}


void plot(uint16_t x, uint16_t y, uint16_t color)
{
    unsigned char* fb   = (unsigned char*) 0xa0000;
	unsigned int offset = y * 800 + x;
	unsigned int gap    = 64;
	unsigned int bank   = 0;
	unsigned int real   = offset - 0xffff;

    if (y > (90*1)) { SetVideoBank(bank+1); } else
	if (y > (90*2)) { SetVideoBank(bank+2); } else
    if (y > (90*3)) { SetVideoBank(bank+3); } else
	if (y > (90*4)) { SetVideoBank(bank+4); }
	else {
		SetVideoBank(bank); // kreis 0
		real = offset;
	}

    fb[real] = color;
}

void
circle(
	int xcent,
	int ycent,
	int rad  ,
	int colr )
{
	int x, y, d;
	int dE, dSE;

	x = 0;
	y = rad;

	d = 1 - rad;
	dE = 3;
	dSE = -(rad << 1) + 5;

	plot(xcent-x, ycent-y, colr);
	plot(xcent-x, ycent+y, colr);
	plot(xcent+y, ycent-x, colr);
	plot(xcent-y, ycent+x, colr);

	while (y > x)
    {
		if (d < 0) {
			d += dE;
            dE += 2;
            dSE +=2;
            ++x;
		}
		else {
        	d += dSE;
            dE += 2;
            dSE += 4;
            ++x;
            --y;
		}

    	plot(xcent+x, ycent-y, colr);
    	plot(xcent-x, ycent+y, colr);
    	plot(xcent-x, ycent-y, colr);
    	plot(xcent+x, ycent+y, colr);
   		plot(xcent+y, ycent-x, colr);
    	plot(xcent-y, ycent+x, colr);
    	plot(xcent-y, ycent-x, colr);
    	plot(xcent+y, ycent+x, colr);
	}
}




void
BootMain()
{
	/* This function turns off interrupts */
	INTS(false);

	/* clears the screen, duh! */
	clear_screen();

	/* Tell the user where we loaded to */
	print("IDT kernel is currently loaded @ 1MB Physical.", 0);

	/* Remap PIC IRQs to cover software interrupts 0x20-0x30 */
	remapPIC(0x20,0x28);
	maskIRQ(ALL);
	print("8259 PIC Remapped.", 1);


	/* Here we load the Exceptions that are called when certain faulty ocurr */
	LoadExceptions();
	print("Exceptions Loaded.", 2);


	/* Point the IDT register to our IDT array */
	loadIDTR();
	/* We did it. Now to just call our interrupt */
	print("IDT initialised.", 4);


unsigned char* fb   = (unsigned char*) 0xa0000;
/*VbeWrite(5,0);memset(fb,1,0xffff);
VbeWrite(5,1);memset(fb,1,0xffff);
VbeWrite(5,2);memset(fb,1,0xffff);
VbeWrite(5,3);memset(fb,1,0xffff);
VbeWrite(5,4);memset(fb,1,0xffff);
VbeWrite(5,5);memset(fb,1,0xffff);
VbeWrite(5,6);memset(fb,1,0xffff);*/

SetVideoMode(800,600,8);
VbeWrite(5,0);memset(fb,1,0xffff);
VbeWrite(5,1);memset(fb,1,64);
VbeWrite(5,0);
circle(30,30,30,14);
plot(30,30,15);
circle(30,30+30+30+30+30,30,15);

	return;
}

/*
 * Our Cuxtom Interrupt
 */
void testint(void)
{
	clear_screen();
	print("TEST INT CALLED",2);
}
