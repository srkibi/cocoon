#ifndef PACKAGE_H
#define PACKAGE_H

#include <stdint.h>

#define MAX_PACKAGES 50
#define MAX_PACKAGE_NAME 32
#define PACKAGE_CONTENT_SIZE 4096

typedef struct {
    char name[MAX_PACKAGE_NAME];
    char description[128];
    char version[16];
    uint8_t installed;
    char content[PACKAGE_CONTENT_SIZE];
} package_t;

// Sistema de pacotes
void pkg_init();
int pkg_install(const char *pkg_name);
int pkg_remove(const char *pkg_name);
int pkg_list();
int pkg_search(const char *query);
package_t* pkg_find(const char *pkg_name);

// Pacotes disponíveis
extern package_t available_packages[MAX_PACKAGES];
extern int package_count;

#endif