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

void SetVideoMode (int xres, int yres, uint16_t bpp)
{
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
 

char *VGA = (char *)0xA0000;

void plot_fast(int x,int y,byte color)
{
  VGA[y*320+x]=color;
}

void plot(uint16_t x, uint16_t y, uint16_t color)
{
    unsigned char* fb   = (unsigned char*) 0xa0000;	static
	unsigned int old_bank  = 0;
	unsigned int bank_size = 0xffff+1;
	unsigned int pixel  = y * 800 + x;
	unsigned int bank   = pixel / bank_size ;
	unsigned int offset = pixel - bank * bank_size;

	if (bank != old_bank) {
		old_bank  = bank;
		SetVideoBank(bank);
	}

    fb[offset] = color;
}

void setpixel(uint16_t x, uint16_t y, uint16_t color) { plot(x,y,color); }

int getpixel(uint16_t x, uint16_t y)
{
	unsigned char* fb   = (unsigned char*) 0xa0000; static
	unsigned int old_bank = 0;
	unsigned int pixel  = y * 800 + x;
	unsigned int bank_size = 0xffff+1;
	unsigned int bank   = pixel / bank_size ;
	unsigned int offset = pixel - bank * bank_size;


	if (bank != old_bank) {
		old_bank  = bank;
		SetVideoBank(bank);
	}
	
	return fb[pixel];
}

int abs(int j)
{
	return (j < 0 ? -j : j);
}

double powerOfTen(int num)
{
   double rst = 1.0;
   int i;
   if(num >= 0){
       for(i = 0; i < num ; i++){
           rst *= 10.0;
       }
   } else {
	   int a = 0-num;
		int i;
       for(i = 0; i < a; i++){
           rst *= 0.1;
       }
   }
   return rst;
}
double sqrt(double a)
{
   /*
         find more detail of this method on wiki methods_of_computing_square_roots
         *** Babylonian method cannot get exact zero but approximately value of the square_root
   */
   double z = a; 
   double rst = 0.0;
   int max = 8;     // to define maximum digit 
   int i;
   double j = 1.0;
   for(i = max ; i > 0 ; i--){
       // value must be bigger then 0
       if(z - (( 2 * rst ) + ( j * powerOfTen(i)))*( j * powerOfTen(i)) >= 0)
       {
           while( z - (( 2 * rst ) + ( j * powerOfTen(i)))*( j * powerOfTen(i)) >= 0)
           {
               j++;
               if(j >= 10) break;
           }
           j--; //correct the extra value by minus one to j
           z -= (( 2 * rst ) + ( j * powerOfTen(i)))*( j * powerOfTen(i)); //find value of z

           rst += j * powerOfTen(i);     // find sum of a
           j = 1.0;
       }
  }
  for(i = 0 ; i >= 0 - max ; i--)
  {
      if(z - (( 2 * rst ) + ( j * powerOfTen(i)))*( j * powerOfTen(i)) >= 0)
      {
           while( z - (( 2 * rst ) + ( j * powerOfTen(i)))*( j * powerOfTen(i)) >= 0)
           {
               j++;
           }
           j--;
           z -= (( 2 * rst ) + ( j * powerOfTen(i)))*( j * powerOfTen(i)); //find value of z
          rst += j * powerOfTen(i);     // find sum of a
          j = 1.0;
      }
  }
  return rst;
}



void drawline(int x0, int y0, int x1, int y1, int colr)
{
	int dx = abs(x1-x0), sx = x0<x1 ? 1 : -1;
	int dy = abs(y1-y0), sy = y0<y1 ? 1 : -1; 
	int err = (dx>dy ? dx : -dy)/2, e2;
 
	for(;;) {
   		plot(x0,y0,colr);
    	if (x0 == x1 && y0 == y1) break;
    	e2 = err;
    	if (e2 >-dx) { err -= dy; x0 += sx; }
    	if (e2 < dy) { err += dx; y0 += sy; }
	}
}

void fillcircle(int x, int y, int r, int colr)
{
	int ox = x;
	int oy = y;

	for(y = -r; y <= r; y++)
    for(x = -r; x <= r; x++)
    if(x*x+y*y <= r*r)
    plot(ox+x,oy+y,colr);
}

void drawrect(int left,int top, int right, int bottom, byte color)
{
    drawline(left,top,right,top,color);
    drawline(left,top,left,bottom,color);
    drawline(right,top,right,bottom,color);
    drawline(left,bottom,right,bottom,color);
}


void fillrect(const int x1,const int y1,const int x2,const int y2, int colr)
{
    int y_min = ( (y1 >= y2) ? y2 : y1);
    int y_max = ( (y1 <= y2) ? y2 : y1);

    int x_min = ( (x1 >= x2) ? x2 : x1);
    int x_max = ( (x1 <= x2) ? x2 : x1);

	int farbe;
	int xc,yc;
	
	/*
	RGB(1,0,0) = RGB(255,   0,   0) = R
	RGB(1,1,0) = RGB(255, 255,   0) = G
	RGB(0,0,1) = RGB(0  ,   0, 255) = B
	
	1.0 = 255
	---------  x = ((0.5 * 255) / 1) = 128  R = 128
	0.5 = x
	*/

    int R  = 0;
    int G  = 0;
    int B  = 255;
    
    int a  = 0.5;
    int aw = B * a;
    
    B = aw; if (B >= 127) { B = (((64 * B)-1) / B); } else B = 15;
	
    for (yc = y_min; yc <= y_max; yc++) {
        for (xc = x_min; xc <= x_max; xc++) {

            plot(xc+x1,yc+y1,B);
        }
	}
}


void drawcircle(int x0, int y0, int radius, int colr)
{
    int x = radius;
    int y = 0;
    int err = 0;

    while (x >= y)
    {
        plot(x0 + x, y0 + y, colr);
        plot(x0 + y, y0 + x, colr);
        plot(x0 - y, y0 + x, colr);
        plot(x0 - x, y0 + y, colr);
        plot(x0 - x, y0 - y, colr);
        plot(x0 - y, y0 - x, colr);
        plot(x0 + y, y0 - x, colr);
        plot(x0 + x, y0 - y, colr);

        if (err <= 0)
        {
            y += 1;
            err += 2*y + 1;
        }
        if (err > 0)
        {
            x -= 1;
            err -= 2*x + 1;
        }
    }
}

void triangle(const int x1, const int y1,
			  const int x2, const int y2,
		      const int x3, const int y3, int colr)
{
    drawline(x1,y1,x2,y2,colr);
    drawline(x2,y2,x3,y3,colr);
    drawline(x3,y3,x1,y1,colr);
}

/*
void fillScan(const int scan,const struct Edge active,const int fill_color)
{
    const struct Edge p1;
    const struct Edge p2;

      p1 = active.next;

      while(p1)
	  {
	     p2=p1.next;

	     for(int count=p1.xIntersect;count<=p2.xIntersect;count++)
		 plot(count,scan,fill_color);

	     p1=p2.next;
	  }
}*/

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


extern uint16_t setColor(uint8_t r, uint8_t g, uint8_t b);
extern void cc_test(uint16_t xpos, uint16_t ypos, uint16_t sx, uint16_t sy, uint16_t color, float alpha);
void
BootMain()
{

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
drawcircle(30,30,30,14);
plot(30,30,15);
fillcircle(230,340,30,15);
drawrect(100,150,130,140,14);

fillrect(100,90,230,240,14);
triangle(300,100,360,200,200,100,1);


cc_test(10,10,300,200, setColor(100,200,0), 0.5);

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
