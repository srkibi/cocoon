#ifndef VGA_H
#define VGA_H

#include <stdint.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_MEMORY 0xB8000

// Cores VGA
enum vga_color {
    VGA_BLACK = 0,
    VGA_BLUE = 1,
    VGA_GREEN = 2,
    VGA_CYAN = 3,
    VGA_RED = 4,
    VGA_MAGENTA = 5,
    VGA_BROWN = 6,
    VGA_LIGHT_GREY = 7,
    VGA_DARK_GREY = 8,
    VGA_LIGHT_BLUE = 9,
    VGA_LIGHT_GREEN = 10,
    VGA_LIGHT_CYAN = 11,
    VGA_LIGHT_RED = 12,
    VGA_LIGHT_MAGENTA = 13,
    VGA_YELLOW = 14,
    VGA_WHITE = 15
};

// Função para criar cor VGA
uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg);

// Funções básicas de vídeo
void clear_screen();
void putchar(char c);
void puts(const char *str);
void set_color(uint8_t color);
void update_cursor();
void enable_cursor(uint8_t cursor_start, uint8_t cursor_end);
void disable_cursor();
void get_cursor_position(int *x, int *y);
void set_cursor_xy(int x, int y);

#endif