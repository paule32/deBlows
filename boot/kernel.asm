org 0x8000     ; code's start offset
[BITS 16]      ; 16 Bit Code

;;;;;;;;;;;;;
; Real Mode ;
;;;;;;;;;;;;;

RealMode:
    xor ax, ax         ; set up segments
    mov es, ax
    mov ds, ax
    mov ss, ax
    mov sp, ax

    add sp, -0x40     ; make room for input buffer (64 chars)

.pm:
    cli               ; clear interrupts
    lgdt [gdtr]       ; load GDT via GDTR (defined in file "gtd.inc")

    mov eax, cr0      ; switch-over to Protected Mode
    or  eax, 1        ; set bit 0 of CR0 register
    mov cr0, eax      ;

    jmp 0x8:ProtectedMode
 
;;;;;;;;;;;;;;;;;;
; Protected Mode ;
;;;;;;;;;;;;;;;;;;

[Bits 32]

ProtectedMode:
    mov    ax, 0x10
    mov    ds, ax      ; data descriptor --> data, stack and extra segment
    mov    ss, ax
    mov    es, ax
    xor    eax, eax    ; null desriptor --> FS and GS
    mov    fs, ax
    mov    gs, ax
    mov    esp, 0x200000 ; set stack below 2 MB limit

	jmp    BootMain
	hlt

%include "./boot/gdt.inc"

;;;;;;;;;;;;;;;;;;;
; Set Bits to NOP ;
;;;;;;;;;;;;;;;;;;;
 
times 1024-($-$$) nop

BootMain:
