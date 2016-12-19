#ifndef IDT
#define IDT


void loadIDTR();//sets up IDT by loading IDTR
void AddInt(int number, void (*handler)(), dword dpl); //add interrupt


#endif // idt.h
