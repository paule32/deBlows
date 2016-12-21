#include "k_defines.h"
#include "io.h"
#include "stdio.h"
#include "idt.h"
#include "pic.h"
#include "panic.h"
#include "descriptor.h"
#include "interrupts.h"

void testint(void);

char *VGA = (char *)0xA0000;

void plot_fast(int x,int y,byte color)
{
  VGA[y*320+x]=color;
}


void pixel_p(unsigned char color, unsigned int x, unsigned int y)
{
   unsigned char* fb        = (unsigned char*) 0xa0000;
   unsigned int offset     = y * 800 + x;
   fb[offset] = color;
}



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

void SetVideoMode (uint16_t xres, uint16_t yres, uint16_t bpp) {

   VbeWrite (VBE_DISPI_INDEX_ENABLE, VBE_DISPI_DISABLED);
   VbeWrite (VBE_DISPI_INDEX_XRES, xres);
   VbeWrite (VBE_DISPI_INDEX_YRES, yres);
   VbeWrite (VBE_DISPI_INDEX_BPP, bpp);
   VbeWrite (VBE_DISPI_INDEX_ENABLE, VBE_DISPI_ENABLED | VBE_DISPI_LFB_ENABLED);   
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

	/* We enable interrupts again so we can call our interrupt */
//	INTS(true);
//	asm("int $0x10");
//	INTS(false);

SetVideoMode(800,600,8);

unsigned char* fb = (unsigned char*) 0xa0000;
VbeWrite(5,0);memset(fb,1,0xffff);
VbeWrite(5,1);memset(fb,1,0xffff);
VbeWrite(5,2);memset(fb,1,0xffff);
VbeWrite(5,3);memset(fb,1,0xffff);
VbeWrite(5,4);memset(fb,1,0xffff);
VbeWrite(5,5);memset(fb,1,0xffff);
VbeWrite(5,6);memset(fb,1,0xffff);

// ------------------------------------------
// calculate pixel in center of screen ...
// ------------------------------------------
int banks_used  = 6;
int buffer_used = (banks_used * 0xffff);   // 64 kb limit

int width  = 800 / 2;
int height = 600 / 2;
int halfpt = (buffer_used / 2);
int pixoff = halfpt - 0xffff;
int bankpt = (banks_used / 2)  - 1;
int result = (pixoff / bankpt) - (width / 2);

VbeWrite(5,bankpt);
fb[result] = 14;


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
