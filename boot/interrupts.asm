; interrupts.asm
; contains all system exception interrupts
; and other system interface interrupts
; note: exception 15 and 20-31 are Intel Reserved

%define i386
%macro SAVE_REGS 0
    %ifdef i386
	push eax
	push ebx
	push ecx
	push edx
	push esi
	push edi
	push ebp
	push esp
    %else
	push rax
	push rbx
	push rcx
	push rdx
	push rsi
	push rdi
	push rbp
	push rsp
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
    %endif
	push fs
	push gs
%endmacro

%macro RESTORE_REGS 0
	pop gs
	pop fs
    %ifdef i386
	pop esp
	pop ebp
	pop edi
	pop esi
	pop edx
	pop ecx
	pop ebx
	pop eax
    %else
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rsp
	pop rbp
	pop rdi
	pop rsi
	pop rdx
	pop rcx
	pop rbx
	pop rax
    %endif
%endmacro

%macro REGS_CALLEE 1
	mov eax, 0x10
    %ifdef i386
	mov edi, eax
	mov esi, eax
    %else
	mov rdi, rax
	mov rsi, rax
    %endif
	cld
	call %1
%endmacro

extern int_00
global int00
int00:
	SAVE_REGS
	REGS_CALLEE int_00   ; Divide by Zero #DE
	RESTORE_REGS
    iret

[extern int_01]
[global int01]
int01:
	SAVE_REGS
	REGS_CALLEE int_01   ; Test Interrupt Handler
	RESTORE_REGS
    iret

[extern int_02]
[global int02]
int02:
	SAVE_REGS
	REGS_CALLEE int_02   ; Test Interrupt Handler
	RESTORE_REGS
    iret

[extern int_03]
[global int03]
int03:
	SAVE_REGS
	REGS_CALLEE int_03   ; Test Interrupt Handler
	RESTORE_REGS
    iret

[extern int_04]
[global int04]
int04:
	SAVE_REGS
	REGS_CALLEE int_04   ; Test Interrupt Handler
	RESTORE_REGS
    iret

[extern int_05]
[global int05]
int05:
	SAVE_REGS
	REGS_CALLEE int_05   ; Test Interrupt Handler
	RESTORE_REGS
    iret

[extern int_06]
[global int06]
int06:
	SAVE_REGS
	REGS_CALLEE int_06   ; Test Interrupt Handler
	RESTORE_REGS
    iret

[extern int_07]
[global int07]
int07:
	SAVE_REGS
	REGS_CALLEE int_07   ; Test Interrupt Handler
	RESTORE_REGS
    iret

[extern int_08]
[global int08]
int08:
	SAVE_REGS
	REGS_CALLEE int_08   ; Test Interrupt Handler
	RESTORE_REGS
    iret

[extern int_09]
[global int09]
int09:
	SAVE_REGS
	REGS_CALLEE int_09   ; Test Interrupt Handler
	RESTORE_REGS
    iret

[extern int_10]
[global int10]
int10:
	SAVE_REGS
	REGS_CALLEE int_10   ; Test Interrupt Handler
	RESTORE_REGS
    iret

[extern int_11]
[global int11]
int11:
	SAVE_REGS
	REGS_CALLEE int_11   ; Test Interrupt Handler
	RESTORE_REGS
    iret

[extern int_12]
[global int12]
int12:
	SAVE_REGS
	REGS_CALLEE int_12   ; Test Interrupt Handler
	RESTORE_REGS
    iret

[extern int_13]
[global int13]
int13:
	SAVE_REGS
	REGS_CALLEE int_13   ; Test Interrupt Handler
	RESTORE_REGS
    iret

[extern int_14]
[global int14]
int14:
	SAVE_REGS
	REGS_CALLEE int_14   ; Test Interrupt Handler
	RESTORE_REGS
    iret

;; Exception 15 is an Intel Reserved Interrupt

[extern int_16]
[global int16]
int16:
	SAVE_REGS
	REGS_CALLEE int_16   ; Test Interrupt Handler
	RESTORE_REGS
    iret

[extern int_17]
[global int17]
int17:
	SAVE_REGS
	REGS_CALLEE int_17   ; Test Interrupt Handler
	RESTORE_REGS
    iret

[extern int_18]
[global int18]
int18:
	SAVE_REGS
	REGS_CALLEE int_18   ; Test Interrupt Handler
	RESTORE_REGS
    iret

[extern int_19]
[global int19]
int19:
	SAVE_REGS
	REGS_CALLEE int_19   ; Test Interrupt Handler
	RESTORE_REGS
    iret

;; Exceptions 20-31 are Intel Reserved Interrupts

;; E n d   o f   E x c e p t i o n s   I n t e r r u p t s

[extern testint]
[global int32]
int32:
	SAVE_REGS
	REGS_CALLEE testint   ; Test Interrupt Handler
	RESTORE_REGS
    iret

