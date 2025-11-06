#ifndef STRING_H
#define STRING_H

#include <stddef.h>

// Nossas próprias funções de string
size_t strlen(const char *str);
char *strcpy(char *dest, const char *src);
char *strncpy(char *dest, const char *src, size_t n);
int strcmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, size_t n);
char *strcat(char *dest, const char *src);
char *strstr(const char *haystack, const char *needle);
char *strchr(const char *str, int c);
char *strrchr(const char *str, int c);

#endif