#ifndef INTERRUPTS
#define INTERRUPTS


/*
 * External Asm Interrupt Handlers
 * int00()-int19() = Exception Handlers
 * We use these for interrupt handling
 * ie- int ocurrs, it calls one of these functions
 * these are all assembly functions in interrupts.asm
 */
extern void int00(void);
extern void int01(void);
extern void int02(void);
extern void int03(void);
extern void int04(void);
extern void int05(void);
extern void int06(void);
extern void int07(void);
extern void int08(void);
extern void int09(void);
extern void int10(void);
extern void int11(void);
extern void int12(void);
extern void int13(void);
extern void int14(void);
extern void int15(void);
extern void int16(void);
extern void int17(void);
extern void int18(void);
extern void int19(void);
extern void int32(void);


#endif //interrupts.h
