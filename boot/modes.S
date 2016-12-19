	.text
         .code16

#define ASM
#include "arch.h"

#define CMDLINE_BASE 0x7000
#define KLOADER_BASE 0x8000
#define INITIAL_BASE 0x200000


EXTERN(RealEntryPoint)

	cli

	/* Setup segment registers */
	xorw	%ax,%ax
	movw	%ax,%ds
	movw	%ax,%es
	movw	%ax,%fs
	movw	%ax,%gs
	movw	%ax,%ss
	/* Setup a stack */
	movw	stack16,%sp

	sti

	/* Init pmode */
	call	switch_to_prot

	.code32

	/* Store the boot drive */
	movb	%dl,(_BootDrive)

	/* Store the boot partition */
	movb	%dh,(_BootPartition)


	call	_InitIDT

	/* GO! */
	xorl	%eax,%eax
	pushl	%eax
	call	_BootMain

	call	switch_to_real
	.code16

	int	$0x19

	/* We should never get here */
stop:
	jmp	stop
	nop
	nop

/*
 * Switches the processor to protected mode
 * it destroys eax
 */
EXTERN(switch_to_prot)

	.code16

	cli		/* None of these */

	/* We don't know what values are currently */
	/* in the segment registers. So we are */
	/* going to reload them with sane values. */
	/* Of course CS has to already be valid. */
	/* We are currently in real-mode so we */
	/* need real-mode segment values. */
	movw	$0x0000,%ax
	movw	%ax,%ds
	movw	%ax,%es
	movw	%ax,%fs
	movw	%ax,%gs
	movw	%ax,%ss

	/* Get the return address off the stack */
	popw	(code32ret)

	/* Save 16-bit stack pointer */
	movw	%sp,stack16

	/* Load the GDT */
	lgdt	gdtptr
	/* Load the IDT */
	lidt	i386idtptr

	/* Enable Protected Mode */
	mov	%cr0,%eax
	orl	$CR0_PE_SET,%eax
	mov	%eax,%cr0

	/* Clear prefetch queue & correct CS */
	ljmp	$PMODE_CS, $inpmode

	.code32

inpmode:
	/* Setup segment selectors */
	movw	$PMODE_DS,%ax
	movw	%ax,%ds
	movw	%ax,%es
	movw	%ax,%fs
	movw	%ax,%gs
	movw	%ax,%ss
	movl	stack32,%esp

	/* Put the return address back onto the stack */
	pushl	(code32ret)

	/* Now return in p-mode! */
	ret

/*
 * Switches the processor back to real mode
 * it destroys eax
 */
EXTERN(switch_to_real)

	.code32

	/* We don't know what values are currently */
	/* in the segment registers. So we are */
	/* going to reload them with sane values. */
	/* Of course CS has to already be valid. */
	/* We are currently in protected-mode so we */
	/* need protected-mode segment values. */
	movw	$PMODE_DS,%ax
	movw	%ax,%ds
	movw	%ax,%es
	movw	%ax,%fs
	movw	%ax,%gs
	movw	%ax,%ss

	/* Get the return address off the stack */
	popl	(code16ret)

	/* Save 32-bit stack pointer */
	movl	%esp,stack32

	/* jmp to 16-bit segment to set the limit correctly */
	ljmp	$RMODE_CS, $switch_to_real16

switch_to_real16:
	.code16

	/* Restore segment registers to correct limit */
	movw	$RMODE_DS,%ax
	movw	%ax,%ds
	movw	%ax,%es
	movw	%ax,%fs
	movw	%ax,%gs
	movw	%ax,%ss

	/* Disable Protected Mode */
	mov	%cr0,%eax
	andl	$CR0_PE_CLR,%eax
	mov	%eax,%cr0

	/* Clear prefetch queue & correct CS */
	ljmp	$0, $inrmode

inrmode:
	movw	%cs,%ax
	movw	%ax,%ds
	movw	%ax,%es
	movw	%ax,%fs
	movw	%ax,%gs
	movw	%ax,%ss

	/* Clear out the high 16-bits of ESP */
	/* This is needed because I have one */
	/* machine that hangs when booted to dos if */
	/* anything other than 0x0000 is in the high */
	/* 16-bits of ESP. Even though real-mode */
	/* code should only use SP and not ESP. */
	xorl	%esp,%esp

	movw	stack16,%sp

	/* Put the return address back onto the stack */
	pushw	(code16ret)

	/* Load IDTR with real mode value */
	lidt	rmode_idtptr

	sti		/* These are ok now */

	/* Now return in r-mode! */
	ret





	/*
	 * Needed for enabling the a20 address line
	 */
	.code16
empty_8042:
	.word	0x00eb,0x00eb            // jmp $+2, jmp $+2
	inb	$0x64,%al
	testb	$0x02,%al
	jnz	empty_8042
	ret



	/*
	 * Enable the A20 address line (to allow access to over 1mb)
	 */
EXTERN(_EnableA20)
	.code32

	pushal

	call	switch_to_real
	.code16

	call	empty_8042
	movb	$0xD1,%al                // command write
	outb	%al,$0x64
	call	empty_8042
	mov	$0xDF,%al                // A20 on
	out	%al,$0x60
	call	empty_8042
	call	switch_to_prot
	.code32

	popal

	ret

	/*
	 * Disable the A20 address line
	 */
EXTERN(_DisableA20)
	.code32

	pushal

	call	switch_to_real
	.code16

	call	empty_8042
	movb	$0xD1,%al                // command write
	outb	%al,$0x64
	call	empty_8042
	mov	$0xDD,%al                // A20 off
	out	%al,$0x60
	call	empty_8042
	call	switch_to_prot
	.code32

	popal

	ret



	.code32

stack16:	  .word STACK16ADDR	/* 16-bit stack pointer */
stack32:   .long STACK32ADDR	/* 32-bit stack pointer */
code16ret: .long 0  		/* 16-bit return address */
code32ret: .long 0      		/* 32-bit return address */


	.p2align	2	/* force 4-byte alignment */
gdt:
	/* NULL Descriptor */
	.word	0x0000
	.word	0x0000
	.word	0x0000
	.word	0x0000

	/* 32-bit flat CS */
	.word	0xFFFF
	.word	0x0000
	.word	0x9A00
	.word	0x00CF

	/* 32-bit flat DS */
	.word	0xFFFF
	.word	0x0000
	.word	0x9200
	.word	0x00CF

	/* 16-bit real mode CS */
	.word	0xFFFF
	.word	0x0000
	.word	0x9E00
	.word	0x0000

	/* 16-bit real mode DS */
	.word	0xFFFF
	.word	0x0000
	.word	0x9200
	.word	0x0000

/* GDT table pointer */
gdtptr:
	.word	0x27		/* Limit */
	.long	gdt			/* Base Address */

/* Initial GDT table pointer for multiboot */
gdtptrhigh:
	.word	0x27		/* Limit */
	.long	gdt + INITIAL_BASE - KLOADER_BASE	/* Base Address */

/* Real-mode IDT pointer */
rmode_idtptr:
	.word	0x3ff		/* Limit */
	.long	0			/* Base Address */

EXTERN(_BootDrive)
	.long	0

EXTERN(_BootPartition)
	.long	0
