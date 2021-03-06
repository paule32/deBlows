#include "../boot/stdio.h"

typedef union {
    uint32_t cv;
    struct {
        uint8_t r: 5;
        uint8_t g: 6;
        uint8_t b: 5;
    } ; //__attribute__((packed));
} color;
 
color
blend(color a, color b, float alpha) {
    const float invalpha = 1.0-alpha;
    float tmp;
    color rv;
   
    tmp = invalpha * a.r + alpha * b.r;
    rv.r = tmp;
   
    tmp = invalpha * a.g + alpha * b.g;
    rv.g = tmp;
 
    tmp = invalpha * a.b + alpha * b.b;
    rv.b = tmp;
    return rv;
}

extern "C" uint32_t setColor(uint8_t r, uint8_t g, uint8_t b)
{
    color currentColor;
    
    currentColor.r = r;
    currentColor.g = g;
    currentColor.b = b;
    
    return currentColor.cv;
}

extern "C" int  getpixel(uint32_t x, uint32_t y);
extern "C" void setpixel(uint32_t x, uint32_t y, uint32_t c);

extern "C" void cc_test(uint32_t xpos, uint32_t ypos, uint32_t sx, uint32_t sy, uint32_t col, float alpha)
{
    color c,a,n;
    
    int x = xpos;
    int y = ypos;
    
    for (y = ypos; y <= ypos+sy; y++) {
    for (x = xpos; x <= xpos+sx; x++) {
        if (x >= xpos+sx) {
            x  = xpos;
            break;
        }
        c.cv = getpixel(x,y);
        a.cv = col;
        n    = blend(c,a,alpha);
        setpixel(x,y,n.cv);
    }   
        if (y >= ypos+sy)
        break;
    }
}

