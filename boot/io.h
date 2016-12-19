#ifndef IO
#define IO

/*
 * I/O Functions
 */
void outportb(dword port,byte value);
void outportw(dword port,dword value);
byte inportb(dword port);

#endif //io.h
