#include "k_defines.h"
#include "stdio.h"

void initregs(struct biosregs *reg)
{
	memset(reg, 0, sizeof *reg);
	reg->eflags |= X86_EFLAGS_CF;
	reg->ds = ds();
	reg->es = ds();
	reg->fs = fs();
	reg->gs = gs();
}

/*
 * Simply clears the screen to ' '
 */
void clear_screen()
{
	char *vidmem = (char *)0xB8000;
	unsigned int i = 0;

	while(i < (80*25*2))
	{
		vidmem[i] = ' ';	i++;
		vidmem[i] = 0x07;	i++;
	}
}

/*
 * Print only supports a-z and 0-9 and other keyboard characters
 */
void print(char *msg, unsigned int line)
{
	char *vidmem = (char *)0xB8000;
	unsigned int i = line*80*2, color = WHITE_TXT;

	while(*msg != 0)
	{
		  vidmem[i] = *msg;
		  i++;
		  vidmem[i] = color;
		  i++;
		  *msg++;
	}
}

void *__memcpy(void *to, const void *from, size_t n)
{
	int d0, d1, d2;
	asm volatile("rep ; movsl\n\t"
		     "movl %4,%%ecx\n\t"
		     "andl $3,%%ecx\n\t"
		     "jz 1f\n\t"
		     "rep ; movsb\n\t"
		     "1:"
		     : "=&c" (d0), "=&D" (d1), "=&S" (d2)
		     : "0" (n / 4), "g" (n), "1" ((long)to), "2" ((long)from)
		     : "memory");
	return to;
}

void *memset(void *s, int c, size_t n)
{
	int i;
	char *ss = s;

	for (i = 0; i < n; i++)
		ss[i] = c;
	return s;
}

/*
void *memmove(void *dest, const void *src, size_t n)
{
	unsigned char *d = dest;
	const unsigned char *s = src;

	if (d <= s || d - s >= n)
		return __memcpy(dest, src, n);

	while (n-- > 0)
		d[n] = s[n];

	return dest;
}*/

/* Detect and warn about potential overlaps, but handle them with memmove. */
void *memcpy(void *dest, const void *src, size_t n)
{
	if (dest > src && dest - src < n) {
	    puts("Avoiding potentially unsafe overlapping memcpy()!");
		return memmove(dest, src, n);
	}
	return __memcpy(dest, src, n);
}

void *memmove(void *dest, const void *src, size_t n)
{
    int d0,d1,d2,d3,d4,d5;
    char *ret = dest;

    __asm__ __volatile__(
	/* Handle more 16 bytes in loop */
	"cmp $0x10, %0\n\t"
	"jb	1f\n\t"

	/* Decide forward/backward copy mode */
	"cmp %2, %1\n\t"
	"jb	2f\n\t"

	/*
	 * movs instruction have many startup latency
	 * so we handle small size by general register.
	 */
	"cmp  $680, %0\n\t"
	"jb 3f\n\t"
	/*
	 * movs instruction is only good for aligned case.
	 */
	"mov %1, %3\n\t"
	"xor %2, %3\n\t"
	"and $0xff, %3\n\t"
	"jz 4f\n\t"
	"3:\n\t"
	"sub $0x10, %0\n\t"

	/*
	 * We gobble 16 bytes forward in each loop.
	 */
	"3:\n\t"
	"sub $0x10, %0\n\t"
	"mov 0*4(%1), %3\n\t"
	"mov 1*4(%1), %4\n\t"
	"mov  %3, 0*4(%2)\n\t"
	"mov  %4, 1*4(%2)\n\t"
	"mov 2*4(%1), %3\n\t"
	"mov 3*4(%1), %4\n\t"
	"mov  %3, 2*4(%2)\n\t"
	"mov  %4, 3*4(%2)\n\t"
	"lea  0x10(%1), %1\n\t"
	"lea  0x10(%2), %2\n\t"
	"jae 3b\n\t"
	"add $0x10, %0\n\t"
	"jmp 1f\n\t"

	/*
	 * Handle data forward by movs.
	 */
	".p2align 4\n\t"
	"4:\n\t"
	"mov -4(%1, %0), %3\n\t"
	"lea -4(%2, %0), %4\n\t"
	"shr $2, %0\n\t"
	"rep movsl\n\t"
	"mov %3, (%4)\n\t"
	"jmp 11f\n\t"
	/*
	 * Handle data backward by movs.
	 */
	".p2align 4\n\t"
	"6:\n\t"
	"mov (%1), %3\n\t"
	"mov %2, %4\n\t"
	"lea -4(%1, %0), %1\n\t"
	"lea -4(%2, %0), %2\n\t"
	"shr $2, %0\n\t"
	"std\n\t"
	"rep movsl\n\t"
	"mov %3,(%4)\n\t"
	"cld\n\t"
	"jmp 11f\n\t"

	/*
	 * Start to prepare for backward copy.
	 */
	".p2align 4\n\t"
	"2:\n\t"
	"cmp  $680, %0\n\t"
	"jb 5f\n\t"
	"mov %1, %3\n\t"
	"xor %2, %3\n\t"
	"and $0xff, %3\n\t"
	"jz 6b\n\t"

	/*
	 * Calculate copy position to tail.
	 */
	"5:\n\t"
	"add %0, %1\n\t"
	"add %0, %2\n\t"
	"sub $0x10, %0\n\t"

	/*
	 * We gobble 16 bytes backward in each loop.
	 */
	"7:\n\t"
	"sub $0x10, %0\n\t"

	"mov -1*4(%1), %3\n\t"
	"mov -2*4(%1), %4\n\t"
	"mov  %3, -1*4(%2)\n\t"
	"mov  %4, -2*4(%2)\n\t"
	"mov -3*4(%1), %3\n\t"
	"mov -4*4(%1), %4\n\t"
	"mov  %3, -3*4(%2)\n\t"
	"mov  %4, -4*4(%2)\n\t"
	"lea  -0x10(%1), %1\n\t"
	"lea  -0x10(%2), %2\n\t"
	"jae 7b\n\t"
	/*
	 * Calculate copy position to head.
	 */
	"add $0x10, %0\n\t"
	"sub %0, %1\n\t"
	"sub %0, %2\n\t"

	/*
	 * Move data from 8 bytes to 15 bytes.
	 */
	".p2align 4\n\t"
	"1:\n\t"
	"cmp $8, %0\n\t"
	"jb 8f\n\t"
	"mov 0*4(%1), %3\n\t"
	"mov 1*4(%1), %4\n\t"
	"mov -2*4(%1, %0), %5\n\t"
	"mov -1*4(%1, %0), %1\n\t"

	"mov  %3, 0*4(%2)\n\t"
	"mov  %4, 1*4(%2)\n\t"
	"mov  %5, -2*4(%2, %0)\n\t"
	"mov  %1, -1*4(%2, %0)\n\t"
	"jmp 11f\n\t"

	/*
	 * Move data from 4 bytes to 7 bytes.
	 */
	".p2align 4\n\t"
	"8:\n\t"
	"cmp $4, %0\n\t"
	"jb 9f\n\t"
	"mov 0*4(%1), %3\n\t"
	"mov -1*4(%1, %0), %4\n\t"
	"mov  %3, 0*4(%2)\n\t"
	"mov  %4, -1*4(%2, %0)\n\t"
	"jmp 11f\n\t"

	/*
	 * Move data from 2 bytes to 3 bytes.
	 */
	".p2align 4\n\t"
	"9:\n\t"
	"cmp $2, %0\n\t"
	"jb 10f\n\t"
	"movw 0*2(%1), %%dx\n\t"
	"movw -1*2(%1, %0), %%bx\n\t"
	"movw %%dx, 0*2(%2)\n\t"
	"movw %%bx, -1*2(%2, %0)\n\t"
	"jmp 11f\n\t"

	/*
	 * Move data for 1 byte.
	 */
	".p2align 4\n\t"
	"10:\n\t"
	"cmp $1, %0\n\t"
	"jb 11f\n\t"
	"movb (%1), %%cl\n\t"
	"movb %%cl, (%2)\n\t"
	".p2align 4\n\t"
	"11:"
	: "=&c" (d0), "=&S" (d1), "=&D" (d2),
	  "=r" (d3),"=r" (d4), "=r"(d5)
	:"0" (n),
	 "1" (src),
	 "2" (dest)
	:"memory");
    return ret;
}

size_t strnlen(const char *s, size_t count)
{
	const char *sc;

	for (sc = s; count-- && *sc != '\0'; ++sc)
	/* nothing */;
	return sc - s;
}

int isdigit(int ch)
{
	return (ch >= '0') && (ch <= '9');
}

int isxdigit(int ch)
{
	if (isdigit(ch))
	return true;

	if ((ch >= 'a') && (ch <= 'f'))
	return true;

	return (ch >= 'A') && (ch <= 'F');
 }


/* -*- linux-c -*- ------------------------------------------------------- *
 *
 *   Copyright (C) 1991, 1992 Linus Torvalds
 *   Copyright 2007 rPath, Inc. - All Rights Reserved
 *
 *   This file is part of the Linux kernel, and is made available under
 *   the terms of the GNU General Public License version 2.
 *
 * ----------------------------------------------------------------------- */

/*
 * Oh, it's a waste of space, but oh-so-yummy for debugging.  This
 * version of printf() does not include 64-bit support.  "Live with
 * it."
 *
 */


static int skip_atoi(const char **s)
{
    int i = 0;

    while (isdigit(**s))
	i = i * 10 + *((*s)++) - '0';
    return i;
}

#define ZEROPAD	1		/* pad with zero */
#define SIGN	2		/* unsigned/signed long */
#define PLUS	4		/* show plus */
#define SPACE	8		/* space if plus */
#define LEFT	16		/* left justified */
#define SMALL	32		/* Must be 32 == 0x20 */
#define SPECIAL	64		/* 0x */

#define __do_div(n, base) ({ \
int __res; \
__res = ((unsigned long) n) % (unsigned) base; \
n = ((unsigned long) n) / (unsigned) base; \
__res; })

static char *number(char *str, long num, int base, int size, int precision,
	    int type)
{
    /* we are called with base 8, 10 or 16, only, thus don't need "G..."  */
    static const char digits[16] = "0123456789ABCDEF"; /* "GHIJKLMNOPQRSTUVWXYZ"; */

    char tmp[66];
    char c, sign, locase;
    int i;

    /* locase = 0 or 0x20. ORing digits or letters with 'locase'
     * produces same digits or (maybe lowercased) letters */
    locase = (type & SMALL);
    if (type & LEFT)
	type &= ~ZEROPAD;
    if (base < 2 || base > 16)
	return NULL;
    c = (type & ZEROPAD) ? '0' : ' ';
    sign = 0;
    if (type & SIGN) {
	if (num < 0) {
	    sign = '-';
	    num = -num;
	    size--;
	} else if (type & PLUS) {
	    sign = '+';
	    size--;
	} else if (type & SPACE) {
	    sign = ' ';
	    size--;
	}
    }
    if (type & SPECIAL) {
	if (base == 16)
	    size -= 2;
	else if (base == 8)
	    size--;
    }
    i = 0;
    if (num == 0)
	tmp[i++] = '0';
    else
	while (num != 0)
	    tmp[i++] = (digits[__do_div(num, base)] | locase);
    if (i > precision)
	precision = i;
    size -= precision;
    if (!(type & (ZEROPAD + LEFT)))
	while (size-- > 0)
	    *str++ = ' ';
    if (sign)
	*str++ = sign;
    if (type & SPECIAL) {
	if (base == 8)
	    *str++ = '0';
	else if (base == 16) {
	    *str++ = '0';
	    *str++ = ('X' | locase);
	}
    }
    if (!(type & LEFT))
	while (size-- > 0)
	    *str++ = c;
    while (i < precision--)
	*str++ = '0';
    while (i-- > 0)
	*str++ = tmp[i];
    while (size-- > 0)
	*str++ = ' ';
    return str;
}

int vsprintf(char *buf, const char *fmt, va_list args)
{
    int len;
    unsigned long num;
    int i, base;
    char *str;
    const char *s;

    int flags;		/* flags to number() */

    int field_width;	/* width of output field */
    int precision;		/* min. # of digits for integers; max
		   number of chars for from string */
    int qualifier;		/* 'h', 'l', or 'L' for integer fields */

    for (str = buf; *fmt; ++fmt) {
	if (*fmt != '%') {
	    *str++ = *fmt;
	    continue;
	}

	/* process flags */
	flags = 0;
          repeat:
	++fmt;		/* this also skips first '%' */
	switch (*fmt) {
	case '-':
	    flags |= LEFT;
	    goto repeat;
	case '+':
	    flags |= PLUS;
	    goto repeat;
	case ' ':
	    flags |= SPACE;
	    goto repeat;
	case '#':
	    flags |= SPECIAL;
	    goto repeat;
	case '0':
	    flags |= ZEROPAD;
	    goto repeat;
	}

	/* get field width */
	field_width = -1;
	if (isdigit(*fmt))
	    field_width = skip_atoi(&fmt);
	else if (*fmt == '*') {
	    ++fmt;
	    /* it's the next argument */
	    field_width = va_arg(args, int);
	    if (field_width < 0) {
		field_width = -field_width;
		flags |= LEFT;
	    }
	}

	/* get the precision */
	precision = -1;
	if (*fmt == '.') {
	    ++fmt;
	    if (isdigit(*fmt))
		precision = skip_atoi(&fmt);
	    else if (*fmt == '*') {
		++fmt;
		/* it's the next argument */
		precision = va_arg(args, int);
	    }
	    if (precision < 0)
		precision = 0;
	}

	/* get the conversion qualifier */
	qualifier = -1;
	if (*fmt == 'h' || *fmt == 'l' || *fmt == 'L') {
	    qualifier = *fmt;
	    ++fmt;
	}

	/* default base */
	base = 10;

	switch (*fmt) {
	case 'c':
	    if (!(flags & LEFT))
		while (--field_width > 0)
		    *str++ = ' ';
	    *str++ = (unsigned char)va_arg(args, int);
	    while (--field_width > 0)
		*str++ = ' ';
	    continue;

	case 's':
	    s = va_arg(args, char *);
	    len = strnlen(s, precision);

	    if (!(flags & LEFT))
		while (len < field_width--)
		    *str++ = ' ';
	    for (i = 0; i < len; ++i)
		*str++ = *s++;
	    while (len < field_width--)
		*str++ = ' ';
	    continue;

	case 'p':
	    if (field_width == -1) {
		field_width = 2 * sizeof(void *);
		flags |= ZEROPAD;
	    }
	    str = number(str,
		     (unsigned long)va_arg(args, void *), 16,
		     field_width, precision, flags);
	    continue;

	case 'n':
	    if (qualifier == 'l') {
		long *ip = va_arg(args, long *);
		*ip = (str - buf);
	    } else {
		int *ip = va_arg(args, int *);
		*ip = (str - buf);
	    }
	    continue;

	case '%':
	    *str++ = '%';
	    continue;

	    /* integer number formats - set up the flags and "break" */
	case 'o':
	    base = 8;
	    break;

	case 'x':
	    flags |= SMALL;
	case 'X':
	    base = 16;
	    break;

	case 'd':
	case 'i':
	    flags |= SIGN;
	case 'u':
	    break;

	default:
	    *str++ = '%';
	    if (*fmt)
		*str++ = *fmt;
	    else
		--fmt;
	    continue;
	}
	if (qualifier == 'l')
	    num = va_arg(args, unsigned long);
	else if (qualifier == 'h') {
	    num = (unsigned short)va_arg(args, int);
	    if (flags & SIGN)
		num = (short)num;
	} else if (flags & SIGN)
	    num = va_arg(args, int);
	else
	    num = va_arg(args, unsigned int);
	str = number(str, num, base, field_width, precision, flags);
    }
    *str = '\0';
    return str - buf;
}

int sprintf(char *buf, const char *fmt, ...)
{
    va_list args;
    int i;

    va_start(args, fmt);
    i = vsprintf(buf, fmt, args);
    va_end(args);
    return i;
}

int printf(const char *fmt, ...)
{
    char printf_buf[1024];
    va_list args;
    int printed;

    va_start(args, fmt);
    printed = vsprintf(printf_buf, fmt, args);
    va_end(args);

    puts(printf_buf);

    return printed;
}




int query_apm_bios(void)
{
    struct biosregs ireg, oreg;

    /* APM BIOS installation check */
    initregs(&ireg);
    ireg.ah = 0x53;
    intcall(0x15, &ireg, &oreg);

    if (oreg.flags & X86_EFLAGS_CF)
	return -1;		/* No APM BIOS */

    if (oreg.bx != 0x504d)		/* "PM" signature */
	return -1;

    if (!(oreg.cx & 0x02))		/* 32 bits supported? */
	return -1;

    /* Disconnect first, just in case */
    ireg.al = 0x04;
    intcall(0x15, &ireg, NULL);

    /* 32-bit connect */
    ireg.al = 0x03;
    intcall(0x15, &ireg, &oreg);

    boot_params.apm_bios_info.cseg        = oreg.ax;
    boot_params.apm_bios_info.offset      = oreg.ebx;
    boot_params.apm_bios_info.cseg_16     = oreg.cx;
    boot_params.apm_bios_info.dseg        = oreg.dx;
    boot_params.apm_bios_info.cseg_len    = oreg.si;
    boot_params.apm_bios_info.cseg_16_len = oreg.hsi;
    boot_params.apm_bios_info.dseg_len    = oreg.di;

    if (oreg.flags & X86_EFLAGS_CF)
	return -1;

    /* Redo the installation check as the 32-bit connect;
       some BIOSes return different flags this way... */

    ireg.al = 0x00;
    intcall(0x15, &ireg, &oreg);

    if ((oreg.eflags & X86_EFLAGS_CF) || oreg.bx != 0x504d) {
	/* Failure with 32-bit connect, try to disconect and ignore */
	ireg.al = 0x04;
	intcall(0x15, &ireg, NULL);
	return -1;
    }

    boot_params.apm_bios_info.version = oreg.ax;
    boot_params.apm_bios_info.flags   = oreg.cx;
    return 0;
}


/*
 * Invoke the realmode switch hook if present; otherwise
 * disable all interrupts.
 */
static void realmode_switch_hook(void)
{
    if (boot_params.hdr.realmode_swtch) {
	asm volatile("lcallw *%0"
	         : : "m" (boot_params.hdr.realmode_swtch)
	         : "eax", "ebx", "ecx", "edx");
    } else {
	asm volatile("cli");
	outb(0x80, 0x70); /* Disable NMI */
	io_delay();
    }
}

/*
 * Disable all interrupts at the legacy PIC.
 */
static void mask_all_interrupts(void)
{
    outb(0xff, 0xa1);	/* Mask all interrupts on the secondary PIC */
    io_delay();
    outb(0xfb, 0x21);	/* Mask all but cascade on the primary PIC */
    io_delay();
}

/*
 * Reset IGNNE# if asserted in the FPU.
 */
static void reset_coprocessor(void)
{
    outb(0, 0xf0);
    io_delay();
    outb(0, 0xf1);
    io_delay();
}

/*
 * Set up the GDT
 */

struct gdt_ptr {
    u16 len;
    u32 ptr;
} __attribute__((packed));

static void setup_gdt(void)
{
    /* There are machines which are known to not boot with the GDT
       being 8-byte unaligned.  Intel recommends 16 byte alignment. */
    static const u64 boot_gdt[] __attribute__((aligned(16))) = {
	/* CS: code, read/execute, 4 GB, base 0 */
	[GDT_ENTRY_BOOT_CS] = GDT_ENTRY(0xc09b, 0, 0xfffff),
	/* DS: data, read/write, 4 GB, base 0 */
	[GDT_ENTRY_BOOT_DS] = GDT_ENTRY(0xc093, 0, 0xfffff),
	/* TSS: 32-bit tss, 104 bytes, base 4096 */
	/* We only have a TSS here to keep Intel VT happy;
	   we don't actually use it for anything. */
	[GDT_ENTRY_BOOT_TSS] = GDT_ENTRY(0x0089, 4096, 103),
    };
    /* Xen HVM incorrectly stores a pointer to the gdt_ptr, instead
       of the gdt_ptr contents.  Thus, make it static so it will
       stay in memory, at least long enough that we switch to the
       proper kernel GDT. */
    static struct gdt_ptr gdt;

    gdt.len = sizeof(boot_gdt)-1;
    gdt.ptr = (u32)&boot_gdt + (ds() << 4);

    asm volatile("lgdtl %0" : : "m" (gdt));
}

void setup_idt(void)
{
    static const struct gdt_ptr null_idt = {0, 0};
    asm volatile("lidtl %0" : : "m" (null_idt));
}

/*
 * Actual invocation sequence
 */
void go_to_protected_mode(void)
{
    /* Hook before leaving real mode, also disables interrupts */
    realmode_switch_hook();

    /* Enable the A20 gate */
    if (enable_a20()) {
	puts("A20 gate not responding, unable to boot...\n");
	die();
    }

    /* Reset coprocessor (IGNNE#) */
    reset_coprocessor();

    /* Mask all interrupts in the PIC */
    mask_all_interrupts();

    /* Actual transition to protected mode... */
    setup_idt();
    setup_gdt();
    protected_mode_jump(boot_params.hdr.code32_start,
		(u32)&boot_params + (ds() << 4));
}