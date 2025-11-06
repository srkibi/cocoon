CC = i686-linux-gnu-gcc
ASM = nasm
LD = i686-linux-gnu-ld

CFLAGS = -ffreestanding -nostdlib -nostartfiles -nodefaultlibs -std=gnu99 -m32 -Iinclude
ASFLAGS = -f elf32
LDFLAGS = -nostdlib -T src/boot/linker.ld

SRCDIR = src
OBJDIR = obj
ISO_DIR = isodir

KERNEL = kernel.bin
ISO = cocoon.iso

OBJS = $(OBJDIR)/kernel_entry.o $(OBJDIR)/kernel.o $(OBJDIR)/vga.o \
       $(OBJDIR)/terminal.o $(OBJDIR)/keyboard.o $(OBJDIR)/ports.o \
       $(OBJDIR)/fs.o $(OBJDIR)/string.o \
       $(OBJDIR)/package.o $(OBJDIR)/wget.o

.PHONY: all clean run

all: $(ISO)

$(OBJDIR)/%.o: $(SRCDIR)/boot/%.asm
	@mkdir -p $(OBJDIR)
	$(ASM) $(ASFLAGS) $< -o $@

$(OBJDIR)/%.o: $(SRCDIR)/kernel/%.c
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@
	
$(KERNEL): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^

$(ISO): $(KERNEL)
	@mkdir -p $(ISO_DIR)/boot/grub
	cp $(KERNEL) $(ISO_DIR)/boot/
	cp $(SRCDIR)/boot/grub.cfg $(ISO_DIR)/boot/grub/
	grub-mkrescue -o $(ISO) $(ISO_DIR)

run: $(ISO)
	qemu-system-x86_64 -cdrom $(ISO)

clean:
	rm -rf $(OBJDIR) $(ISO_DIR) $(KERNEL) $(ISO)