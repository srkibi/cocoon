CC = gcc
AS = nasm
LD = ld
CFLAGS = -m64 -ffreestanding -O2 -nostdlib -fno-builtin
LDFLAGS = -T linker.ld

all: isofiles/boot/kernel.bin cocoon.iso

boot.o: boot.asm
	$(AS) -f elf64 boot.asm -o boot.o

kernel.o: kernel.c
	$(CC) $(CFLAGS) -c kernel.c -o kernel.o

kernel.bin: boot.o kernel.o
	$(LD) $(LDFLAGS) boot.o kernel.o -o kernel.bin

isofiles/boot/kernel.bin: kernel.bin grub.cfg
	mkdir -p isofiles/boot/grub
	cp kernel.bin isofiles/boot/
	cp grub.cfg isofiles/boot/grub/

cocoon.iso:
	grub-mkrescue -o cocoon.iso isofiles

clean:
	rm -f *.o kernel.bin
	rm -rf isofiles cocoon.iso
