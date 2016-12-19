	[bits 16]
	[org 0x7c00]

;---------------------------------------
%macro PRINT 1
	lea  si, [%1] 		; load source string
	call print_str
%endmacro
;--------------------------------------

	jmp boot		      ; jump over the data to our code

;-----------------------Data-----------------------;
;------------GDT Table---------------;
GDTR:
GDTsize DW GDT_END-GDT-1
GDTbase DD 0x500

GDT:
NULL_SEL	 EQU $-GDT  ; null descriptor is required (64bit per entry)
      DD 0x0
      DD 0x0
CODESEL 	 EQU $-GDT  ; 4GB Flat Code at 0x0 with max 0xFFFFF limit
      DW     0xFFFF	      ; Limit(2):0xFFFF
      DW     0x0	      ; Base(3)
      DB     0x0	      ; Base(2)
      DB     0x9A	      ; Type: present,ring0,code,exec/read/accessed (10011000)
      DB     0xCF	      ; Limit(1):0xF | Flags:4Kb inc,32bit (11001111)
      DB     0x0	      ; Base(1)
DATASEL 	 EQU $-GDT  ; 4GB Flat Data at 0x0 with max 0xFFFFF limit
      DW     0xFFFF	      ; Limit(2):0xFFFF
      DW     0x0	      ; Base(3)
      DB     0x0	      ; Base(2)
      DB     0x92	      ; Type: present,ring0,data/stack,read/write (10010010)
      DB     0xCF	      ; Limit(1):0xF | Flags:4Kb inc,32bit (11001111)
      DB     0x0	      ; Base(1)
GDT_END:
;----------End GDT Table-------------;

disk_addr_packet:
	db 0x10 			; (00h) size of packet
	db 0x00				; (01h) reserved
	dw 0x0001			; (02h) number of blocks to transfer
	dw 0x8000, 0x0000	; (04h) DWORD, transfer buffer
	dw 0x0010, 0x0000	; (08h) QWORD, starting absolute block number
	dw 0x0000, 0x0000	; (10h)

drive	DB 0		      ; boot drive number  [usually 0]

;------------Functions---------------;
;******************
;* Opens A20 Gate *
;******************
openA20:
    in al, 0x93         ; switch A20 gate via fast A20 port 92
    or al, 2            ; set A20 Gate bit 1
    and al, ~1          ; clear INIT_NOW bit
    out 0x92, al
    ret
;------------------------------------------------------------------------
; Write the string pointed to by %si
; Each char is wrote by using BIOS INT 0x10.
; BIOS INT 0x10:
; AH = 0x0e
; AL = Character to write.
; BH = Page Number (Should be 0)
; BL = Foreground color (Graphics Modes Only)
; When using the function, put the string address to SI. The string
; should end with 0.
;------------------------------------------------------------------------
next_chr:
	mov bx, 0x0001
	mov ah, 0x0e
	int 0x10		; print char

print_str:
	lodsb			; load single byte of string in SI into AL
	cmp al, 0		; string terminated by \0 ?
	jne next_chr
	ret

read_cd_content:
	PRINT message_loading_img
	pop dx
	mov si, disk_addr_packet
	mov ah, 0x42
	int 0x13
	jc  error_read_sectors
	jmp boot_kernel

	ret

int13_ext_check:
	mov ah, 0x41
	mov bx, 0x55aa
	push dx
	int 0x13
	jc  int13_ext_check_failed
	cmp cx, 0xaa55
	jne int13_ext_check_failed
	and cx, 0x01
	jz  int13_ext_check_failed
	jmp read_cd_content
	ret

int13_ext_check_failed:
	PRINT message_no_int13_ext
	jmp   boot_kernel

error_read_sectors
	PRINT message_sector_error
	jmp boot_kernel

message_loading_img:
	db "Booting image ...", 0x0a, 0x0d
message_halt:
	db "Booting Halt!", 0x0a, 0x0d
message_sector_error:
	db "sector read error.", 0x0a, 0x0d

message_no_int13_ext:
	db "No INT13 extension available. Boot failed", 0x0a, 0x0d

;----------End Functions-------------;

;---------------------End Data---------------------;

	[bits 16]
boot_kernel:
	PRINT message_halt
	cli
	hlt
	jmp boot_kernel

;------------------------------------------------
boot:
;	mov [drive],dl		  ; save boot drive number(0x00=floppy 0x80=hard drive)
	mov ax, cs		 	  ; setup ds segment
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov ax, 0x1D00		  ; stack is at 0x1D00
	mov ss, ax		   	  ; align stack also
	mov sp, 0x2000		  ; 5120 byte stack

	mov ax, 0xb800		   ; setup video segment
	mov gs, ax

	PRINT message_sector_error
	call int13_ext_check

	jmp init		   ; Some BIOSes jump to 0x7c0:0x0 rather than 0x0:0x7c0

init:

;---%<----------
;mov ax, 0x03
;int 0x10
;call print_string
;--->%-------

	; move GDT to 0x500
	xor ax,ax
	mov ds,ax
	mov es,ax
	mov si,GDT		      	; Move From [DS:SI]
	mov di,[GDTbase]	    ; Move to [ES:DI]
	mov cx,[GDTsize]	    ; size of GDT
	cld			      		; Clear the Direction Flag
	rep movsb		      	; Move it


	cli
	;enter pmode
	mov eax,cr0
	or al,1
	mov cr0,eax


	;load gdt
	lgdt[GDTR]


	;clear cs/ip/eip
	jmp CODESEL:FLUSH	 ; set cs to CODESEL

[bits 32]
FLUSH:

;refresh all segment registers
mov eax,DATASEL
mov ds,eax
mov es,eax
mov fs,eax
mov gs,eax
mov ss,eax
mov esp,0xffff

;jump to k_init.asm
jmp CODESEL:0x100000

hlt



TIMES 510-($-$$) DB 0

SIGNATURE DW 0xAA55
