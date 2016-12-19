#ifndef PANIC
#define PANIC

void LoadExceptions(); // init all exceptions
void panic(char *message, char *mnemonic, bool halt); //exception panic

/*
 * Panic Exception Handlers
 */
void int_00(void);
void int_01(void);
void int_02(void);
void int_03(void);
void int_04(void);
void int_05(void);
void int_06(void);
void int_07(void);
void int_08(void);
void int_10(void);
void int_11(void);
void int_12(void);
void int_13(void);
void int_14(void);
void int_16(void);
void int_17(void);
void int_18(void);
void int_19(void);

/*
 * Extern Exception Functions in interrupts.h
 */

#endif // panic.h
