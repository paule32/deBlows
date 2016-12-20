TOOLS = ./tools
CC32 = i686-elf-gcc -O2 -nostdlib -nostdinc -ffreestanding -I./boot
AS32 = i686-elf-as

all: ./boot/bootmain.img ./boot/kernel.img
	gcc -o ./tools/cdmake ./tools/cdmake.c ./tools/dirhash.c ./tools/llmosrt.c

#	mkisofs -pad -l -r -J -v -V "DemoCD" -no-emul-boot -boot-load-size 4 \
#	-boot-info-table -b kernel.img -hide-rr-moved -o bootcd.iso ./cdrom

clean:
	rm -rf ./boot/*.o
	rm -rf ./boot/*.img
	rm -rf ./cdrom/kernel.img
	rm -rf bootcd.iso

./boot/bootmain.img: ./boot/bootmain.asm
	nasm -f bin -o ./boot/bootmain.img ./boot/bootmain.asm

./boot/k_init.o: ./boot/k_init.S
	$(AS32) -o ./boot/k_init.o ./boot/k_init.S

./boot/boot.o: ./boot/boot.c
	$(CC32) -o ./boot/boot.o -c ./boot/boot.c

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

./boot/pmJump.o: ./boot/pmJump.S
	$(CC32) -o ./boot/pmJump.o -c ./boot/pmJump.S

./boot/video.o: ./boot/video.c
	$(CC32) -o ./boot/video.o -c ./boot/video.c

./boot/kernel.img: \
	./boot/boot.o \
	./boot/idt.o \
	./boot/pic.o \
	./boot/panic.o \
	./boot/stdio.o \
	./boot/kernel.o \
	./boot/k_init.o \
	./boot/interrupts.o ./boot/io.o \
	./boot/bioscall.o ./boot/video.o ./boot/pmJump.o
	i686-elf-gcc -T ./boot/kernel.ld -o ./boot/kernel.img -ffreestanding -O2 -nostdlib -lgcc \
	./boot/k_init.o ./boot/boot.o ./boot/bioscall.o ./boot/video.o \
	./boot/kernel.o ./boot/idt.o ./boot/pic.o ./boot/panic.o \
	./boot/stdio.o ./boot/interrupts.o ./boot/io.o ./boot/pmJump.o
	mkdir -p isodir/boot/grub
	cp ./boot/kernel.img isodir/boot/kernel.img
	cp grub.cfg isodir/boot/grub/grub.cfg
	grub-mkrescue -o bootcd.iso isodir

#	cp ./boot/bootmain.img ./cdrom/kernel.img
#	cat ./boot/kernel.img >> ./cdrom/kernel.img
