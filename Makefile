TOOLS = ./tools
CC32 = i686-elf-gcc -O2 -nostdlib -nostdinc -ffreestanding -I./boot
AS32 = i686-elf-as

all: ./boot/kernel.img

clean:
	rm -rf ./boot/*.o
	rm -rf ./boot/*.img
	rm -rf ./cdrom/kernel.img
	rm -rf bootcd.iso

./boot/k_init.o: ./boot/k_init.S
	$(AS32) -o ./boot/k_init.o ./boot/k_init.S

./boot/kernel.o: ./boot/kernel.c
	$(CC32) -o ./boot/kernel.o -c ./boot/kernel.c

./boot/panic.o: ./boot/panic.c
	$(CC32) -o ./boot/panic.o -c ./boot/panic.c

./boot/stdio.o: ./boot/stdio.c
	$(CC32) -o ./boot/stdio.o -c ./boot/stdio.c

./boot/pic.o: ./boot/pic.c
	$(CC32) -o ./boot/pic.o -c ./boot/pic.c

./boot/idt.o: ./boot/idt.c
	$(CC32) -o ./boot/idt.o -c ./boot/idt.c

./boot/io.o: ./boot/io.c
	$(CC32) -o ./boot/io.o -c ./boot/io.c

./boot/interrupts.o: ./boot/interrupts.asm
	nasm -f elf32 -o ./boot/interrupts.o ./boot/interrupts.asm

./boot/bioscall.o: ./boot/bioscall.S
	$(AS32) -o ./boot/bioscall.o -c ./boot/bioscall.S

./boot/video.o: ./boot/video.c
	$(CC32) -o ./boot/video.o -c ./boot/video.c

./boot/vga.o: ./boot/vga.asm
	nasm -t -f elf32 -o ./boot/vga.o ./boot/vga.asm

./boot/kernel.img: \
	./boot/idt.o \
	./boot/pic.o \
	./boot/panic.o \
	./boot/stdio.o \
	./boot/kernel.o \
	./boot/k_init.o  \
	./boot/interrupts.o ./boot/io.o \
	./boot/video.o ./boot/vga.o
	i686-elf-gcc -T ./boot/kernel.ld -o ./boot/kernel.img -ffreestanding -O2 -nostdlib  \
	./boot/k_init.o ./boot/video.o -lgcc \
	./boot/kernel.o ./boot/idt.o ./boot/pic.o ./boot/panic.o \
	./boot/stdio.o ./boot/interrupts.o ./boot/io.o \
	./boot/vga.o
	mkdir -p isodir/boot/grub
	cp ./boot/kernel.img isodir/boot/kernel.img
	cp grub.cfg isodir/boot/grub/grub.cfg
	grub-mkrescue -o bootcd.iso isodir

