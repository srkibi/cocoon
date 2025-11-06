#include "vga.h"
#include "terminal.h"

void kernel_main(void) {
    // Inicializar e executar o terminal
    terminal_run();
    
    // Loop infinito
    while(1) {
        asm volatile ("hlt"); // HLT para economizar CPU
    }
}