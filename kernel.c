typedef unsigned long size_t;

void kernel_main(void) {
    volatile unsigned short *video = (unsigned short*)0xB8000;
    const char *msg = "Hello, Cocoon kernel!";
    for (size_t i = 0; msg[i]; ++i)
        video[i] = (unsigned short)(msg[i] | (0x0F << 8));

    for (;;) __asm__ __volatile__("hlt");
}
