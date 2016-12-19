TOOLS = ./tools
CC = gcc -O2

all: ./boot/bootmain.img ./boot/kernel.img
	$(CC) -o ./tools/cdmake ./tools/cdmake.c ./tools/dirhash.c ./tools/llmosrt.c
	mkisofs -pad -l -r -J -v -V "DemoCD" -no-emul-boot -boot-load-size 4 \
	-boot-info-table -b kernel.img -hide-rr-moved -o bootcd.iso ./cdrom

clean:
	rm -rf ./boot/*.o
	rm -rf ./boot/*.img
	rm -rf ./cdrom/kernel.img
	rm -rf bootcd.iso

./boot/bootmain.img: ./boot/bootmain.asm
	nasm -f bin -o ./boot/bootmain.img ./boot/bootmain.asm

./boot/k_init.o: ./boot/k_init.asm
	nasm -f elf64 -o ./boot/k_init.o ./boot/k_init.asm

./boot/boot.o: ./boot/boot.c
	$(CC) -o ./boot/boot.o -c ./boot/boot.c

./boot/kernel.o: ./boot/kernel.c
	$(CC) -o ./boot/kernel.o -c ./boot/kernel.c

./boot/panic.o: ./boot/panic.c
	$(CC) -o ./boot/panic.o -c ./boot/panic.c

./boot/stdio.o: ./boot/stdio.c
	$(CC) -o ./boot/stdio.o -c ./boot/stdio.c

./boot/pic.o: ./boot/pic.c
	$(CC) -o ./boot/pic.o -c ./boot/pic.c

./boot/idt.o: ./boot/idt.c
	$(CC) -o ./boot/idt.o -c ./boot/idt.c

./boot/io.o: ./boot/io.c
	$(CC) -o ./boot/io.o -c ./boot/io.c

./boot/interrupts.o: ./boot/interrupts.asm
	nasm -f elf64 -o ./boot/interrupts.o ./boot/interrupts.asm

./boot/kernel.img: \
	./boot/boot.o \
	./boot/idt.o \
	./boot/pic.o \
	./boot/panic.o \
	./boot/stdio.o \
	./boot/kernel.o \
	./boot/k_init.o \
	./boot/interrupts.o ./boot/io.o
	ld -T ./boot/kernel.ld -nostdlib -o ./boot/kernel.img \
	./boot/k_init.o ./boot/boot.o \
	./boot/kernel.o ./boot/idt.o ./boot/pic.o ./boot/panic.o \
	./boot/stdio.o ./boot/interrupts.o ./boot/io.o
	cp ./boot/bootmain.img ./cdrom/kernel.img
	cat ./boot/kernel.img >> ./cdrom/kernel.img

