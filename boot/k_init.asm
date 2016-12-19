	[bits 64]

[global start]
[extern BootMain]

start:     

xor eax, eax     ; jump to boot.c
int 0x10


hlt              ; halt the cpu
