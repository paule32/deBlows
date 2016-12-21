#include "stdio.h"



static void bios_putchar(int ch)
{

}

void putchar(int ch)
{
    if (ch == '\n')
    putchar('\r');

    bios_putchar(ch);
}

void puts(const char *str)
{
    while (*str)
    putchar(*str++);
}


int getchar(void)
{
    return 0;
}

static u8 gettime(void)
{

    return 0;
}

static int kbd_pending(void)
{

}

void kbd_flush(void)
{
    for (;;) {
	if (!kbd_pending())
	break;
	getchar();
    }
}

int set_kbdrate_timeout(void)
{
    int cnt = 20;
    int t0, t1;

    t0 = gettime();

    while (cnt) {
	if (kbd_pending())
	return getchar();

	t1 = gettime();
	if (t0 != t1) {
	    cnt--;
	    t0 = t1;
	}
    }

    return 0;	// timout
}
