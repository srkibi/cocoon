#ifndef PSF_H
#define PSF_H

#include <stdint.h>

// Cabeçalho PSF1
typedef struct {
    uint8_t magic[2];    // 0x36, 0x04
    uint8_t mode;        // 0 = 256 chars, 1 = 512 chars
    uint8_t height;      // Altura em pixels
} psf1_header_t;

// Cabeçalho PSF2
typedef struct {
    uint8_t magic[4];    // 0x72, 0xb5, 0x4a, 0x86
    uint32_t version;    // 0
    uint32_t headersize; // 32
    uint32_t flags;      // 0 = no unicode table
    uint32_t length;     // Número de glyphs
    uint32_t charsize;   // Bytes per glyph
    uint32_t height;     // Altura em pixels
    uint32_t width;      // Largura em pixels
} psf2_header_t;

#endif