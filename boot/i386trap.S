        .text
        .code16

#define ASM
#include "arch.h"
#include "version.h"

.macro SAVE_CPU_REGS
	movl	%eax,i386_EAX
	movl	%ebx,i386_EBX
	movl	%ecx,i386_ECX
	movl	%edx,i386_EDX
	movl	%esp,i386_ESP
	movl	%ebp,i386_EBP
	movl	%esi,i386_ESI
	movl	%edi,i386_EDI
	movw	%ds,%ax
	movw	%ax,i386_DS
	movw	%es,%ax
	movw	%ax,i386_ES
	movw	%fs,%ax
	movw	%ax,i386_FS
	movw	%gs,%ax
	movw	%ax,i386_GS
	movw	%ss,%ax
	movw	%ax,i386_SS
	popl	%eax
	movl	%eax,i386_EIP
	popl	%eax
	movw	%ax,i386_CS
	popl	%eax
	movl	%eax,i386_EFLAGS
	movl	%cr0,%eax
	movl	%eax,i386_CR0
	//movl	%cr1,%eax
	//movl	%eax,i386_CR1
	movl	%cr2,%eax
	movl	%eax,i386_CR2
	movl	%cr3,%eax
	movl	%eax,i386_CR3
	movl	%dr0,%eax
	movl	%eax,i386_DR0
	movl	%dr1,%eax
	movl	%eax,i386_DR1
	movl	%dr2,%eax
	movl	%eax,i386_DR2
	movl	%dr3,%eax
	movl	%eax,i386_DR3
	movl	%dr6,%eax
	movl	%eax,i386_DR6
	movl	%dr7,%eax
	movl	%eax,i386_DR7
	sgdt	i386_GDTR
	sidt	i386_IDTR
	sldt	i386_LDTR
	str     i386_TR
.endm

/* Set by each exception handler to the address of the description text */
i386ExceptionIndex:
        .long	0

EXTERN(_i386ExceptionHandlerText)
        .ascii "Ein Fehler wurde im Loader festgestellt\n"
        .ascii __KERNEL_VERSION__
        .ascii "\n"
        .asciz "melden Sie einen Fehlerreport an <jkallup@web.de>\n\n"

EXTERN(_i386DebugExceptionText)
        .asciz "Debug Fehler"

EXTERN(_i386NMIExceptionText)
        .asciz "NMI Interrupt"

EXTERN(_i386BreakpointText)
        .asciz "Breakpoint"

EXTERN(_i386OverflowText)
        .asciz "Overflow"

EXTERN(_i386BoundExceptionText)
        .asciz "Bound Range Exception"

EXTERN(_i386InvalidOpcodeText)
        .asciz "Falscher Opcode"

EXTERN(_i386FPUNotAvailableText)
        .asciz "FPU nicht vorhanden"

EXTERN(_i386DoubleFaultText)
        .asciz "Double Fault"

EXTERN(_i386CoprocessorSegmentText)
        .asciz "Coprozessor Segment Ueberlauf"

EXTERN(_i386InvalidTSSText)
        .asciz "Falsches TSS"

EXTERN(_i386SegmentNotPresentText)
        .asciz "Segment nicht vorhandnen"

EXTERN(_i386StackExceptionText)
        .asciz "Stack Segment Fault"

EXTERN(_i386GeneralProtectionFaultText)
        .asciz "General Protection Fault"

EXTERN(_i386PageFaultText)
        .asciz "Page Fault"

EXTERN(_i386CoprocessorErrorText)
        .asciz "FPU Floating-Point Fehler"

EXTERN(_i386AlignmentCheckText)
        .asciz "Align Check"

EXTERN(_i386MachineCheckText)
        .asciz "Machine Check"

EXTERN(_i386DivideByZeroText)
        .asciz "Ausnahmefehler 00: DIVISION DURCH 0\n\n"


EXTERN(_i386testintText)
	.asciz "Hello World!!!"


/* Set by each exception handler to the address of the description text */
_i386ExceptionDescriptionText:
        .long	0


/* Used to store the contents of all the registers when an exception occurs */
i386_EAX:
        .long	0
i386_EBX:
        .long	0
i386_ECX:
        .long	0
i386_EDX:
        .long	0
i386_ESP:
        .long	0
i386_EBP:
        .long	0
i386_ESI:
        .long	0
i386_EDI:
        .long	0
i386_CS:
        .word	0
i386_DS:
        .word	0
i386_ES:
        .word	0
i386_FS:
        .word	0
i386_GS:
        .word	0
i386_SS:
        .word	0
i386_EFLAGS:
        .long	0
i386_EIP:
        .long	0
i386_ERROR_CODE:
        .long	0
i386_CR0:
        .long	0
i386_CR1:
        .long	0
i386_CR2:
        .long	0
i386_CR3:
        .long	0
i386_DR0:
        .long	0
i386_DR1:
        .long	0
i386_DR2:
        .long	0
i386_DR3:
        .long	0
i386_DR6:
        .long	0
i386_DR7:
        .long	0
i386_GDTR:
        .word	0
        .long	0
i386_IDTR:
        .word	0
        .long	0
i386_LDTR:
        .word	0
i386_TR:
        .word	0


/*
 Used to store the current X and Y position on the screen
*/
i386_ScreenPosX:	.long	0
i386_ScreenPosY:	.long	0

#define SCREEN_ATTR 0x1f

/************************************************************************/
.global _i386DivideByZero
_i386DivideByZero:
        .code32
        movl	$_i386DivideByZeroText,_i386ExceptionDescriptionText
        jmp	i386CommonExceptionHandler


.global _i386DebugException
_i386DebugException:
        .code32
        movl	$_i386DebugExceptionText,_i386ExceptionDescriptionText
        jmp	i386CommonExceptionHandler

.global _i386NMIException
_i386NMIException:
        .code32
        movl	$_i386DebugExceptionText,_i386ExceptionDescriptionText
        jmp	i386CommonExceptionHandler

.global _i386Breakpoint
_i386Breakpoint:
        .code32
        movl	$_i386BreakpointText,_i386ExceptionDescriptionText
        jmp	i386CommonExceptionHandler

.global _i386Overflow
_i386Overflow:
        .code32
        movl	$_i386OverflowText,_i386ExceptionDescriptionText
        jmp	i386CommonExceptionHandler

.global _i386BoundException
_i386BoundException:
        .code32
        movl	$_i386BoundExceptionText,_i386ExceptionDescriptionText
        jmp	i386CommonExceptionHandler

.global _i386InvalidOpcode
_i386InvalidOpcode:
        .code32
        movl	$_i386InvalidOpcodeText,_i386ExceptionDescriptionText
        jmp	i386CommonExceptionHandler

.global _i386FPUNotAvailable
_i386FPUNotAvailable:
        .code32
        movl	$_i386FPUNotAvailableText,_i386ExceptionDescriptionText
        jmp	i386CommonExceptionHandler

.global _i386DoubleFault
_i386DoubleFault:
        .code32
        movl	$_i386DoubleFaultText,_i386ExceptionDescriptionText
        jmp	i386CommonExceptionHandler

.global _i386CoprocessorSegment
_i386CoprocessorSegment:
        .code32
        movl	$_i386CoprocessorSegmentText,_i386ExceptionDescriptionText
        jmp	i386CommonExceptionHandler

.global _i386InvalidTSS
_i386InvalidTSS:
        .code32
        movl	$_i386InvalidTSSText,_i386ExceptionDescriptionText
        jmp	i386CommonExceptionHandler

.global _i386SegmentNotPresent
_i386SegmentNotPresent:
        .code32
        movl	$_i386SegmentNotPresentText,_i386ExceptionDescriptionText
        jmp	i386CommonExceptionHandler

.global _i386StackException
_i386StackException:
        .code32
        movl	$_i386StackExceptionText,_i386ExceptionDescriptionText
        jmp	i386CommonExceptionHandler

.global _i386GeneralProtectionFault
_i386GeneralProtectionFault:
        .code32
        movl	$_i386GeneralProtectionFaultText,_i386ExceptionDescriptionText
        jmp	i386CommonExceptionHandler

.global _i386PageFault
_i386PageFault:
        .code32
        movl	$_i386PageFaultText,_i386ExceptionDescriptionText
        jmp	i386CommonExceptionHandler

.global _i386CoprocessorError
_i386CoprocessorError:
        .code32
        movl	$_i386CoprocessorErrorText,_i386ExceptionDescriptionText
        jmp	i386CommonExceptionHandler

.global _i386AlignmentCheck
_i386AlignmentCheck:
        .code32
        movl	$_i386AlignmentCheckText,_i386ExceptionDescriptionText
        jmp	i386CommonExceptionHandler

.global _i386MachineCheck
_i386MachineCheck:
        .code32
        movl	$_i386MachineCheckText,_i386ExceptionDescriptionText
        jmp	i386CommonExceptionHandler

.global _i386testint
_i386testint:
	.code32
	call __i386testint
	iret

//        movl	$_i386testintText,_i386ExceptionDescriptionText
//        jmp	i386CommonExceptionHandler


/************************************************************************/
i386CommonExceptionHandler:
        .code32

        SAVE_CPU_REGS

        pushl	$SCREEN_ATTR
        call	_VideoClearScreen
        add	$4,%esp

        movl	$_i386ExceptionHandlerText,%esi
        call	i386PrintText

        movl	_i386ExceptionDescriptionText,%esi
        call	i386PrintText

         cli

i386ExceptionHandlerHang:
        hlt
        jmp	i386ExceptionHandlerHang

        iret


/************************************************************************/
/* AL = Char to display                                                 */
/************************************************************************/
i386PrintChar:
        .code32

        pushl	i386_ScreenPosY
        pushl	i386_ScreenPosX
        pushl	$SCREEN_ATTR
        andl	$0xff,%eax
        pushl	%eax
        call	_VideoPutChar
        addl	$16,%esp

        ret

/************************************************************************/
/* ESI = Address of text to display                                     */
/************************************************************************/
i386PrintText:
        .code32

i386PrintTextLoop:
        lodsb

        // Check for end of string char
        cmp	$0,%al
        je	i386PrintTextDone

        // Check for newline char
        cmp	$0x0a,%al
        jne	i386PrintTextLoop2
        incl	i386_ScreenPosY
        movl	$0,i386_ScreenPosX
        jmp	i386PrintTextLoop

i386PrintTextLoop2:
        call	i386PrintChar
        incl	i386_ScreenPosX

        jmp	i386PrintTextLoop

i386PrintTextDone:

        ret

