; Multiboot Header
section .multiboot
align 4
    dd 0x1BADB002              ; magic
    dd 0x00000000              ; flags
    dd -(0x1BADB002 + 0x00000000) ; checksum

; Kernel Entry Point
section .text
global _start
extern kernel_main

_start:
    ; Setup stack
    mov esp, stack_top
    
    ; Call kernel main
    call kernel_main
    
    ; Halt if kernel returns
    cli
.hang:
    hlt
    jmp .hang

; BSS Section
section .bss
align 16
stack_bottom:
    resb 16384 ; 16KB stack
stack_top:

section .note.GNU-stack noalloc noexec nowrite progbits