#include "keyboard.h"
#include "vga.h"
#include "ports.h"

#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64

// Mapa de teclado US QWERTY (scancode para ASCII)
static const char keyboard_map[] = {
    0,  0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 
    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '-',
    0, 0, 0, '+', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static const char keyboard_map_shift[] = {
    0,  0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~', 
    0, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,
    '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '-',
    0, 0, 0, '+', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static int shift_pressed = 0;
static int caps_lock = 0;
static int ctrl_pressed = 0;
static int alt_pressed = 0;

#define KEY_UP     0x48
#define KEY_DOWN   0x50
#define KEY_LEFT   0x4B
#define KEY_RIGHT  0x4D
#define KEY_HOME   0x47
#define KEY_END    0x4F
#define KEY_DEL    0x53     

void keyboard_init() {
    // Habilita o teclado
    outb(0x21, 0xFD); // Mascara de IRQ - habilita apenas teclado (IRQ1)
}

int keyboard_has_input() {
    return inb(KEYBOARD_STATUS_PORT) & 1;
}

char keyboard_getchar() {
    if (!keyboard_has_input()) {
        return 0;
    }

    uint8_t scancode = inb(KEYBOARD_DATA_PORT);
    
    // Tecla pressionada (bit 7 não setado)
    if (scancode < 0x80) {
        switch (scancode) {
            case 0x2A: // Left shift pressed
            case 0x36: // Right shift pressed
                shift_pressed = 1;
                return 0;
            case 0x1D: // Ctrl pressed
                ctrl_pressed = 1;
                return 0;
            case 0x38: // Alt pressed
                alt_pressed = 1;
                return 0;
            case 0x3A: // Caps lock
                caps_lock = !caps_lock;
                return 0;
            case 0xE0: // Prefixo para teclas estendidas
                // A próxima scancode será a tecla estendida
                scancode = inb(KEYBOARD_DATA_PORT);
                if (scancode == KEY_HOME) return 0x01;  // HOME
                if (scancode == KEY_END) return 0x05;   // END
                if (scancode == KEY_DEL) return 0x04;   // DELETE
                return 0;
        }
        
        // Combinções Ctrl+...
        if (ctrl_pressed) {
            switch (scancode) {
                case 0x1E: // Ctrl+A (Home)
                    return 0x01;
                case 0x12: // Ctrl+E (End)
                    return 0x05;
                case 0x20: // Ctrl+D (Delete)
                    return 0x04;
            }
        }
        
        // Verificar se é uma tecla estendida que queremos tratar
        if (scancode == KEY_UP) return 0x11;    // UP
        if (scancode == KEY_DOWN) return 0x12;  // DOWN
        if (scancode == KEY_LEFT) return 0x13;  // LEFT  
        if (scancode == KEY_RIGHT) return 0x14; // RIGHT
        
        // Converter scancode para ASCII
        char c;
        if (shift_pressed || caps_lock) {
            c = keyboard_map_shift[scancode];
        } else {
            c = keyboard_map[scancode];
        }
        
        return c;
    } 
    // Tecla liberada (bit 7 setado)
    else {
        scancode -= 0x80;
        switch (scancode) {
            case 0x2A: // Left shift released
            case 0x36: // Right shift released
                shift_pressed = 0;
                break;
            case 0x1D: // Ctrl released
                ctrl_pressed = 0;
                break;
            case 0x38: // Alt released
                alt_pressed = 0;
                break;
        }
    }
    
    return 0;
}