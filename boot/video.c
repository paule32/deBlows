#include "stdio.h"

#define XMTRDY  0x20

#define TXT	0
#define LSR	5
struct screen_info *screen_info = &boot_params.screen_info;

#ifdef USE_TERMINAL
int early_serial_base;
static void serial_putchar(int ch)
{
    unsigned timeout = 0xffff;

    while ((inb(early_serial_base + LSR) & XMTRDY) == 0 && --timeout)
    cpu_relax();

    outb(ch, early_base + TXR);
}
#endif

static void bios_putchar(int ch)
{
    struct biosregs ireg;

    initregs(&ireg);
    ireg.bx = 0x0007;	// page/color
    ireg.cx = 0x0001;	// count
    ireg.al = ch;	// character
    intcall(0x10, &ireg, NULL);
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
    struct biosregs ireg, oreg;
    initregs(&ireg);
    return oreg.al;
}

static u8 gettime(void)
{
    struct biosregs ireg, oreg;

    initregs(&ireg);
    ireg.ah = 0x02;
    intcall(0x1a, &ireg, &oreg);

    return oreg.dh;
}

static int kbd_pending(void)
{
    struct biosregs ireg, oreg;

    initregs(&ireg);
    ireg.ah = 0x01;
    intcall(0x16, &ireg, &oreg);

    return !(oreg.eflags & X86_EFLAGS_ZF);
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
    int cnt = 10;
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


__videocard video_bios;

/* Set a conventional BIOS mode */
//int set_bios_mode(u8 mode);

int set_bios_mode(u8 mode);
int bios_set_mode(struct mode_info *mi)
{
    return set_bios_mode(mi->mode - VIDEO_FIRST_BIOS);
}

int set_bios_mode(u8 mode)
{
    struct biosregs ireg, oreg;
    u8 new_mode;

    initregs(&ireg);
    ireg.al = mode;		/* AH=0x00 Set Video Mode */
    intcall(0x10, &ireg, NULL);

    ireg.ah = 0x0f;		/* Get Current Video Mode */
    intcall(0x10, &ireg, &oreg);

    do_restore = 1;		/* Assume video contents were lost */

    /* Not all BIOSes are clean with the top bit */
    new_mode = oreg.al & 0x7f;

    if (new_mode == mode)
	return 0;	/* Mode change OK */

    return -1;
}

static int bios_probe(void)
{
    u8 mode;
    u8 saved_mode = boot_params.screen_info.orig_video_mode;

    u16 crtc;
    struct mode_info *mi;
    int nmodes = 0;

    if (adapter != ADAPTER_EGA && adapter != ADAPTER_VGA)
	return 0;

    set_fs(0);
    crtc = vga_crtc();

    video_bios.modes = GET_HEAP(struct mode_info, 0);

    for (mode = 0x14; mode <= 0x7f; mode++) {
	if (!heap_free(sizeof(struct mode_info)))
	    break;

	if (mode_defined(VIDEO_FIRST_BIOS+mode))
	    continue;

	if (set_bios_mode(mode))
	    continue;

	/* Try to verify that it's a text mode. */

	/* Attribute Controller: make graphics controller disabled */
	if (in_idx(0x3c0, 0x10) & 0x01)
	    continue;

	/* Graphics Controller: verify Alpha addressing enabled */
	if (in_idx(0x3ce, 0x06) & 0x01)
	    continue;

	/* CRTC cursor location low should be zero(?) */
	if (in_idx(crtc, 0x0f))
	    continue;

	mi = GET_HEAP(struct mode_info, 1);
	mi->mode = VIDEO_FIRST_BIOS+mode;
	mi->depth = 0;	/* text */
	mi->x = rdfs16(0x44a);
	mi->y = rdfs8(0x484)+1;
	nmodes++;
    }

    set_bios_mode(saved_mode);

    return nmodes;
}

__videocard video_bios =
{
    .card_name	= "BIOS",
    .probe		= bios_probe,
    .set_mode	= bios_set_mode,
    .unsafe		= 1,
    .xmode_first	= VIDEO_FIRST_BIOS,
    .xmode_n	= 0x80,
};


/* -*- linux-c -*- ------------------------------------------------------- *
 *
 *   Copyright (C) 1991, 1992 Linus Torvalds
 *   Copyright 2007-2008 rPath, Inc. - All Rights Reserved
 *
 *   This file is part of the Linux kernel, and is made available under
 *   the terms of the GNU General Public License version 2.
 *
 * ----------------------------------------------------------------------- */

/*
 * Common variables
 */
int adapter;		/* 0=CGA/MDA/HGC, 1=EGA, 2=VGA+ */
int force_x, force_y;	/* Don't query the BIOS for cols/rows */
int do_restore;		/* Screen contents changed during mode flip */
int graphic_mode;	/* Graphic mode with linear frame buffer */

/* Probe the video drivers and have them generate their mode lists. */
void probe_cards(int unsafe)
{
    struct card_info *card;
    static u8 probed[2];

    if (probed[unsafe])
	return;

    probed[unsafe] = 1;

    for (card = video_cards; card < video_cards_end; card++) {
	if (card->unsafe == unsafe) {
	    if (card->probe)
		card->nmodes = card->probe();
	    else
		card->nmodes = 0;
	}
    }
}

/* Test if a mode is defined */
int mode_defined(u16 mode)
{
    struct card_info *card;
    struct mode_info *mi;
    int i;

    for (card = video_cards; card < video_cards_end; card++) {
	mi = card->modes;
	for (i = 0; i < card->nmodes; i++, mi++) {
	    if (mi->mode == mode)
		return 1;
	}
    }

    return 0;
}

/* Set mode (without recalc) */
static int raw_set_mode(u16 mode, u16 *real_mode)
{
    int nmode, i;
    struct card_info *card;
    struct mode_info *mi;

    /* Drop the recalc bit if set */
    mode &= ~VIDEO_RECALC;

    /* Scan for mode based on fixed ID, position, or resolution */
    nmode = 0;
    for (card = video_cards; card < video_cards_end; card++) {
	mi = card->modes;
	for (i = 0; i < card->nmodes; i++, mi++) {
	    int visible = mi->x || mi->y;

	    if ((mode == nmode && visible) ||
	        mode == mi->mode ||
	        mode == (mi->y << 8)+mi->x) {
		*real_mode = mi->mode;
		return card->set_mode(mi);
	    }

	    if (visible)
		nmode++;
	}
    }

    /* Nothing found?  Is it an "exceptional" (unprobed) mode? */
    for (card = video_cards; card < video_cards_end; card++) {
	if (mode >= card->xmode_first &&
	    mode < card->xmode_first+card->xmode_n) {
	    struct mode_info mix;
	    *real_mode = mix.mode = mode;
	    mix.x = mix.y = 0;
	    return card->set_mode(&mix);
	}
    }

    /* Otherwise, failure... */
    return -1;
}

/*
 * Recalculate the vertical video cutoff (hack!)
 */
static void vga_recalc_vertical(void)
{
    unsigned int font_size, rows;
    u16 crtc;
    u8 pt, ov;

    set_fs(0);
    font_size = rdfs8(0x485); /* BIOS: font size (pixels) */
    rows = force_y ? force_y : rdfs8(0x484)+1; /* Text rows */

    rows *= font_size;	/* Visible scan lines */
    rows--;			/* ... minus one */

    crtc = vga_crtc();

    pt = in_idx(crtc, 0x11);
    pt &= ~0x80;		/* Unlock CR0-7 */
    out_idx(pt, crtc, 0x11);

    out_idx((u8)rows, crtc, 0x12); /* Lower height register */

    ov = in_idx(crtc, 0x07); /* Overflow register */
    ov &= 0xbd;
    ov |= (rows >> (8-1)) & 0x02;
    ov |= (rows >> (9-6)) & 0x40;
    out_idx(ov, crtc, 0x07);
}

/* Set mode (with recalc if specified) */
int set_mode(u16 mode)
{
    int rv;
    u16 real_mode;

	mode = VIDEO_80x60;


    rv = raw_set_mode(mode, &real_mode);
    if (rv)
	return rv;

    if (mode & VIDEO_RECALC)
	vga_recalc_vertical();

    /* Save the canonical mode number for the kernel, not
       an alias, size specification or menu position */
    boot_params.hdr.vid_mode = real_mode;
    return 0;
}


static struct mode_info vga_modes[] = {
	{ VIDEO_80x25,  80, 25, 0 },
	{ VIDEO_8POINT, 80, 50, 0 },
	{ VIDEO_80x43,  80, 43, 0 },
	{ VIDEO_80x28,  80, 28, 0 },
	{ VIDEO_80x30,  80, 30, 0 },
	{ VIDEO_80x34,  80, 34, 0 },
	{ VIDEO_80x60,  80, 60, 0 },
};

static struct mode_info ega_modes[] = {
	{ VIDEO_80x25,  80, 25, 0 },
	{ VIDEO_8POINT, 80, 43, 0 },
};

static struct mode_info cga_modes[] = {
	{ VIDEO_80x25,  80, 25, 0 },
};

static __videocard video_vga;

/* Set basic 80x25 mode */
static u8 vga_set_basic_mode(void)
{
	struct biosregs ireg, oreg;
	u8 mode;

	initregs(&ireg);

	/* Query current mode */
	ireg.ax = 0x0f00;
	intcall(0x10, &ireg, &oreg);
	mode = oreg.al;

	if (mode != 3 && mode != 7)
		mode = 3;

	/* Set the mode */
	ireg.ax = mode;		/* AH=0: set mode */
	intcall(0x10, &ireg, NULL);
	do_restore = 1;
	return mode;
}

static void vga_set_8font(void)
{
	/* Set 8x8 font - 80x43 on EGA, 80x50 on VGA */
	struct biosregs ireg;

	initregs(&ireg);

	/* Set 8x8 font */
	ireg.ax = 0x1112;
	/* ireg.bl = 0; */
	intcall(0x10, &ireg, NULL);

	/* Use alternate print screen */
	ireg.ax = 0x1200;
	ireg.bl = 0x20;
	intcall(0x10, &ireg, NULL);

	/* Turn off cursor emulation */
	ireg.ax = 0x1201;
	ireg.bl = 0x34;
	intcall(0x10, &ireg, NULL);

	/* Cursor is scan lines 6-7 */
	ireg.ax = 0x0100;
	ireg.cx = 0x0607;
	intcall(0x10, &ireg, NULL);
}

static void vga_set_14font(void)
{
	/* Set 9x14 font - 80x28 on VGA */
	struct biosregs ireg;

	initregs(&ireg);

	/* Set 9x14 font */
	ireg.ax = 0x1111;
	/* ireg.bl = 0; */
	intcall(0x10, &ireg, NULL);

	/* Turn off cursor emulation */
	ireg.ax = 0x1201;
	ireg.bl = 0x34;
	intcall(0x10, &ireg, NULL);

	/* Cursor is scan lines 11-12 */
	ireg.ax = 0x0100;
	ireg.cx = 0x0b0c;
	intcall(0x10, &ireg, NULL);
}

static void vga_set_80x43(void)
{
	/* Set 80x43 mode on VGA (not EGA) */
	struct biosregs ireg;

	initregs(&ireg);

	/* Set 350 scans */
	ireg.ax = 0x1201;
	ireg.bl = 0x30;
	intcall(0x10, &ireg, NULL);

	/* Reset video mode */
	ireg.ax = 0x0003;
	intcall(0x10, &ireg, NULL);

	vga_set_8font();
}

/* I/O address of the VGA CRTC */
u16 vga_crtc(void)
{
	return (inb(0x3cc) & 1) ? 0x3d4 : 0x3b4;
}

static void vga_set_480_scanlines(void)
{
	u16 crtc;		/* CRTC base address */
	u8  csel;		/* CRTC miscellaneous output register */

	crtc = vga_crtc();

	out_idx(0x0c, crtc, 0x11); /* Vertical sync end, unlock CR0-7 */
	out_idx(0x0b, crtc, 0x06); /* Vertical total */
	out_idx(0x3e, crtc, 0x07); /* Vertical overflow */
	out_idx(0xea, crtc, 0x10); /* Vertical sync start */
	out_idx(0xdf, crtc, 0x12); /* Vertical display end */
	out_idx(0xe7, crtc, 0x15); /* Vertical blank start */
	out_idx(0x04, crtc, 0x16); /* Vertical blank end */
	csel = inb(0x3cc);
	csel &= 0x0d;
	csel |= 0xe2;
	outb(csel, 0x3c2);
}

static void vga_set_vertical_end(int lines)
{
	u16 crtc;		/* CRTC base address */
	u8  ovfw;		/* CRTC overflow register */
	int end = lines-1;

	crtc = vga_crtc();

	ovfw = 0x3c | ((end >> (8-1)) & 0x02) | ((end >> (9-6)) & 0x40);

	out_idx(ovfw, crtc, 0x07); /* Vertical overflow */
	out_idx(end,  crtc, 0x12); /* Vertical display end */
}

static void vga_set_80x30(void)
{
	vga_set_480_scanlines();
	vga_set_vertical_end(30*16);
}

static void vga_set_80x34(void)
{
	vga_set_480_scanlines();
	vga_set_14font();
	vga_set_vertical_end(34*14);
}

static void vga_set_80x60(void)
{
	vga_set_480_scanlines();
	vga_set_8font();
	vga_set_vertical_end(60*8);
}

static int vga_set_mode(struct mode_info *mode)
{
	/* Set the basic mode */
	vga_set_basic_mode();

	/* Override a possibly broken BIOS */
	force_x = mode->x;
	force_y = mode->y;

	switch (mode->mode) {
	case VIDEO_80x25:
		break;
	case VIDEO_8POINT:
		vga_set_8font();
		break;
	case VIDEO_80x43:
		vga_set_80x43();
		break;
	case VIDEO_80x28:
		vga_set_14font();
		break;
	case VIDEO_80x30:
		vga_set_80x30();
		break;
	case VIDEO_80x34:
		vga_set_80x34();
		break;
	case VIDEO_80x60:
		vga_set_80x60();
		break;
	}

	return 0;
}

/*
 * Note: this probe includes basic information required by all
 * systems.  It should be executed first, by making sure
 * video-vga.c is listed first in the Makefile.
 */
static int vga_probe(void)
{
	static const char *card_name[] = {
		"CGA/MDA/HGC", "EGA", "VGA"
	};
	static struct mode_info *mode_lists[] = {
		cga_modes,
		ega_modes,
		vga_modes,
	};
	static int mode_count[] = {
		sizeof(cga_modes)/sizeof(struct mode_info),
		sizeof(ega_modes)/sizeof(struct mode_info),
		sizeof(vga_modes)/sizeof(struct mode_info),
	};

	struct biosregs ireg, oreg;

	initregs(&ireg);

	ireg.ax = 0x1200;
	ireg.bl = 0x10;		/* Check EGA/VGA */
	intcall(0x10, &ireg, &oreg);

#ifndef _WAKEUP
	boot_params.screen_info.orig_video_ega_bx = oreg.bx;
#endif

	/* If we have MDA/CGA/HGC then BL will be unchanged at 0x10 */
	if (oreg.bl != 0x10) {
		/* EGA/VGA */
		ireg.ax = 0x1a00;
		intcall(0x10, &ireg, &oreg);

		if (oreg.al == 0x1a) {
			adapter = ADAPTER_VGA;
#ifndef _WAKEUP
			boot_params.screen_info.orig_video_isVGA = 1;
#endif
		} else {
			adapter = ADAPTER_EGA;
		}
	} else {
		adapter = ADAPTER_CGA;
	}

	video_vga.modes = mode_lists[adapter];
	video_vga.card_name = card_name[adapter];
	return mode_count[adapter];
}

static __videocard video_vga = {
	.card_name	= "VGA",
	.probe		= vga_probe,
	.set_mode	= vga_set_mode,
};
