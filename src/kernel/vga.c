#include "vga.h"
#include "ports.h"

#include <stddef.h>

#define WIDTH VGA_WIDTH
#define HEIGHT VGA_HEIGHT
#define VIDEO_MEMORY VGA_MEMORY

#define VGA_CTRL_REGISTER 0x3D4
#define VGA_DATA_REGISTER 0x3D5

static uint16_t *video_memory = (uint16_t*)VIDEO_MEMORY;
static uint8_t current_color = 0x0F;
static uint16_t cursor_pos = 0;
static int cursor_x = 0, cursor_y = 0;

static size_t strlen(const char *str) {
    size_t len = 0;
    while (str[len] != '\0') {
        len++;
    }
    return len;
}

uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) {
    return fg | bg << 4;
}

void vga_put_char_at(char c, int x, int y) {
    size_t index = y * WIDTH + x;
    video_memory[index] = (uint16_t)c | (uint16_t)current_color << 8;
}

void get_cursor_position(int *x, int *y) {
    *x = cursor_x;
    *y = cursor_y;
}

void set_cursor_xy(int x, int y) {
    if (x < 0) x = 0;
    if (x >= WIDTH) x = WIDTH - 1;
    if (y < 0) y = 0;
    if (y >= HEIGHT) y = HEIGHT - 1;
    
    cursor_x = x;
    cursor_y = y;
    cursor_pos = y * WIDTH + x;
    update_cursor();
}

void scroll_screen() {
    for (int y = 1; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            video_memory[(y - 1) * WIDTH + x] = video_memory[y * WIDTH + x];
        }
    }
    
    for (int x = 0; x < WIDTH; x++) {
        video_memory[(HEIGHT - 1) * WIDTH + x] = (uint16_t)' ' | (uint16_t)current_color << 8;
    }
    
    if (cursor_y > 0) {
        cursor_y--;
    }
    cursor_pos = cursor_y * WIDTH + cursor_x;
    update_cursor();
}

void putchar(char c) {
    int x, y;
    get_cursor_position(&x, &y);

    if (c == '\n') {
        x = 0;
        y++;
        if (y >= VGA_HEIGHT) {
            y = VGA_HEIGHT - 1;
            scroll_screen();
        }
        set_cursor_xy(x, y);

        cursor_x = x;
        cursor_y = y;

        return;
    }

    vga_put_char_at(c, x, y);
    x++;
    if (x >= VGA_WIDTH) {
        x = 0;
        y++;
        if (y >= VGA_HEIGHT) {
            y = VGA_HEIGHT - 1;
        }
    }

    set_cursor_xy(x, y);

    cursor_x = x;
    cursor_y = y;
}

void update_cursor() {
    outb(VGA_CTRL_REGISTER, 14);
    outb(VGA_DATA_REGISTER, cursor_pos >> 8);
    outb(VGA_CTRL_REGISTER, 15);
    outb(VGA_DATA_REGISTER, cursor_pos & 0xFF);
}

void enable_cursor(uint8_t cursor_start, uint8_t cursor_end) {
    outb(VGA_CTRL_REGISTER, 0x0A);
    outb(VGA_DATA_REGISTER, (inb(VGA_DATA_REGISTER) & 0xC0) | cursor_start);
    
    outb(VGA_CTRL_REGISTER, 0x0B);
    outb(VGA_DATA_REGISTER, (inb(VGA_DATA_REGISTER) & 0xE0) | cursor_end);
}

void disable_cursor() {
    outb(VGA_CTRL_REGISTER, 0x0A);
    outb(VGA_DATA_REGISTER, 0x20);
}

void clear_screen() {
    for (int i = 0; i < WIDTH * HEIGHT; i++) {
        video_memory[i] = (uint16_t)' ' | (uint16_t)current_color << 8;
    }
    cursor_pos = 0;
    update_cursor();
}

void set_color(uint8_t color) {
    current_color = color;
}

void puts(const char *str) {
    for (size_t i = 0; i < strlen(str); i++) {
        putchar(str[i]);
    }
}