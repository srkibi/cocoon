section .multiboot2_header
align 8
    dd 0xe85250d6                                          ; magic number
    dd 0                                                   ; architecture (0 = i386)
    dd header_end - header_start                           ; header length
    dd -(0xe85250d6 + 0 + (header_end - header_start))     ; checksum

header_start:
    ; no additiona tag
header_end:

section .text
global _start
extern kernel_main

_start:
    call kernel_main
.hang:
    hlt
    jmp .hang