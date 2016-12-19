#ifndef DESCRIPTOR
#define DESCRIPTOR

/*
 * Interrupt types
 */
#define INT_0 0x8E00     // 1000111000000000 = present,ring0,int_gate
#define INT_3 0xEE00     // 1110111000000000 = present,ring3,int_gate


/* structure for an interrupt */
typedef struct
{
	word low_offset;                         // low nibble of offset to handler of interrupt 
	word selector;                           // GDT selector used 
	word settings;                           // settings for int 
	word high_offset;                        // high nibble to handler code 
}  __attribute__ ((packed)) x86_interrupt;

/* structure for the IDTR */
typedef struct
{
     word limit;                             // limit or Size of IDT 
     x86_interrupt *base;                    // a pointer to the base of the IDT 
} __attribute__ ((packed)) idtr;

#endif // descriptor.h
