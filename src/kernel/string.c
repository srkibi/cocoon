#include "string.h"

size_t strlen(const char *str) {
    size_t len = 0;
    while (str[len] != '\0') {
        len++;
    }
    return len;
}

char *strcpy(char *dest, const char *src) {
    char *ptr = dest;
    while (*src != '\0') {
        *dest++ = *src++;
    }
    *dest = '\0';
    return ptr;
}

char *strncpy(char *dest, const char *src, size_t n) {
    char *ptr = dest;
    size_t i;
    for (i = 0; i < n && src[i] != '\0'; i++) {
        dest[i] = src[i];
    }
    for (; i < n; i++) {
        dest[i] = '\0';
    }
    return ptr;
}

int strcmp(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

int strncmp(const char *s1, const char *s2, size_t n) {
    for (size_t i = 0; i < n; i++) {
        if (s1[i] != s2[i]) return s1[i] - s2[i];
        if (s1[i] == '\0') return 0;
    }
    return 0;
}

char *strcat(char *dest, const char *src) {
    char *ptr = dest;
    while (*dest) dest++;
    while (*src) *dest++ = *src++;
    *dest = '\0';
    return ptr;
}

char *strstr(const char *haystack, const char *needle) {
    if (!*needle) return (char*)haystack;
    
    for (const char *h = haystack; *h; h++) {
        const char *n = needle;
        const char *h2 = h;
        
        while (*h2 && *n && *h2 == *n) {
            h2++;
            n++;
        }
        
        if (!*n) return (char*)h;
    }
    
    return NULL;
}

char *strchr(const char *str, int c) {
    while (*str) {
        if (*str == (char)c) return (char*)str;
        str++;
    }
    return NULL;
}

char *strrchr(const char *str, int c) {
    const char *last = NULL;
    while (*str) {
        if (*str == (char)c) last = str;
        str++;
    }
    return (char*)last;
}