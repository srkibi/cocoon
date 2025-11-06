#include "vga.h"
#include "ports.h"

#include <stddef.h>

#define WIDTH VGA_WIDTH
#define HEIGHT VGA_HEIGHT
#define VIDEO_MEMORY VGA_MEMORY

// Portas do controlador VGA
#define VGA_CTRL_REGISTER 0x3D4
#define VGA_DATA_REGISTER 0x3D5

static uint16_t *video_memory = (uint16_t*)VIDEO_MEMORY;
static uint8_t current_color = 0x0F; // Branco sobre preto
static uint16_t cursor_pos = 0;
static int cursor_x = 0, cursor_y = 0;

// Função nossa para strlen
static size_t strlen(const char *str) {
    size_t len = 0;
    while (str[len] != '\0') {
        len++;
    }
    return len;
}

// Função para criar cor VGA
uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) {
    return fg | bg << 4;
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

// Atualizar a função putchar para manter controle da posição
void putchar(char c) {
    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
    } else if (c == '\r') {
        cursor_x = 0;
    } else if (c == '\b') { // Backspace
        if (cursor_x > 0) {
            cursor_x--;
        } else if (cursor_y > 0) {
            cursor_y--;
            cursor_x = WIDTH - 1;
        }
        video_memory[cursor_y * WIDTH + cursor_x] = (uint16_t)' ' | (uint16_t)current_color << 8;
    } else {
        video_memory[cursor_y * WIDTH + cursor_x] = (uint16_t)c | (uint16_t)current_color << 8;
        cursor_x++;
    }
    
    // Scroll se necessário
    if (cursor_y >= HEIGHT) {
        for (int i = 0; i < WIDTH * (HEIGHT - 1); i++) {
            video_memory[i] = video_memory[i + WIDTH];
        }
        for (int i = WIDTH * (HEIGHT - 1); i < WIDTH * HEIGHT; i++) {
            video_memory[i] = (uint16_t)' ' | (uint16_t)current_color << 8;
        }
        cursor_y = HEIGHT - 1;
    }
    
    // Quebra de linha automática
    if (cursor_x >= WIDTH) {
        cursor_x = 0;
        cursor_y++;
        
        // Scroll novamente se necessário após quebra de linha
        if (cursor_y >= HEIGHT) {
            for (int i = 0; i < WIDTH * (HEIGHT - 1); i++) {
                video_memory[i] = video_memory[i + WIDTH];
            }
            for (int i = WIDTH * (HEIGHT - 1); i < WIDTH * HEIGHT; i++) {
                video_memory[i] = (uint16_t)' ' | (uint16_t)current_color << 8;
            }
            cursor_y = HEIGHT - 1;
        }
    }
    
    cursor_pos = cursor_y * WIDTH + cursor_x;
    update_cursor();
}

// Atualizar a posição do cursor físico
void update_cursor() {
    // Registro de cursor alto
    outb(VGA_CTRL_REGISTER, 14);
    outb(VGA_DATA_REGISTER, cursor_pos >> 8);
    // Registro de cursor baixo
    outb(VGA_CTRL_REGISTER, 15);
    outb(VGA_DATA_REGISTER, cursor_pos & 0xFF);
}

// Habilitar cursor
void enable_cursor(uint8_t cursor_start, uint8_t cursor_end) {
    outb(VGA_CTRL_REGISTER, 0x0A);
    outb(VGA_DATA_REGISTER, (inb(VGA_DATA_REGISTER) & 0xC0) | cursor_start);
    
    outb(VGA_CTRL_REGISTER, 0x0B);
    outb(VGA_DATA_REGISTER, (inb(VGA_DATA_REGISTER) & 0xE0) | cursor_end);
}

// Desabilitar cursor
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