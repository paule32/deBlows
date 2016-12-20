#ifndef IO
#define IO

/*
 * I/O Functions
 */
void outportb(dword port,byte value);
void outportw(dword port,dword value);
void outb(dword port,dword value);
void outw(dword port,dword value);

byte inportb(dword port);
byte inb(dword port);

#endif //io.h
