#include "package.h"
#include "vga.h"
#include "string.h"
#include "fs.h"

package_t available_packages[MAX_PACKAGES];
int package_count = 0;

void pkg_init() {
    package_count = 0;
    
    // Pacotes base do sistema
    package_t *pkg;
    
    // Editor de texto básico
    pkg = &available_packages[package_count++];
    strcpy(pkg->name, "nano");
    strcpy(pkg->description, "Simple text editor");
    strcpy(pkg->version, "1.0");
    pkg->installed = 1; // Já vem instalado
    strcpy(pkg->content, "Basic text editor for Cocoon OS");
    
    // Cliente HTTP
    pkg = &available_packages[package_count++];
    strcpy(pkg->name, "wget");
    strcpy(pkg->description, "Web file downloader");
    strcpy(pkg->version, "1.0");
    pkg->installed = 0;
    strcpy(pkg->content, "Download files from the web");
    
    // Jogo simples
    pkg = &available_packages[package_count++];
    strcpy(pkg->name, "snake");
    strcpy(pkg->description, "Snake game");
    strcpy(pkg->version, "1.0");
    pkg->installed = 0;
    strcpy(pkg->content, "Classic snake game");
    
    // Ferramentas de sistema
    pkg = &available_packages[package_count++];
    strcpy(pkg->name, "sysutils");
    strcpy(pkg->description, "System utilities");
    strcpy(pkg->version, "1.0");
    pkg->installed = 1;
    strcpy(pkg->content, "Basic system utilities");
    
    //puts("✅ Package system initialized\n");
}

int pkg_install(const char *pkg_name) {
    package_t *pkg = pkg_find(pkg_name);
    
    if (!pkg) {
        puts("Package not found: ");
        puts(pkg_name);
        puts("\n");
        return -1;
    }
    
    if (pkg->installed) {
        puts("Package already installed: ");
        puts(pkg_name);
        puts("\n");
        return 0;
    }
    
    pkg->installed = 1;
    
    // Criar arquivos/recursos do pacote
    if (strcmp(pkg_name, "wget") == 0) {
        // Criar arquivo binário do wget (simulado)
        fs_create_file("/bin", "wget");
        fs_entry_t *wget_file = fs_find_entry("/bin/wget");
        if (wget_file) {
            fs_write_file(wget_file, "Cocoon OS wget client - HTTP file downloader");
        }
    } else if (strcmp(pkg_name, "snake") == 0) {
        fs_create_file("/bin", "snake");
        fs_entry_t *snake_file = fs_find_entry("/bin/snake");
        if (snake_file) {
            fs_write_file(snake_file, "Snake game executable");
        }
    }
    
    puts("✅ Installed package: ");
    puts(pkg_name);
    puts("\n");
    
    return 0;
}

int pkg_remove(const char *pkg_name) {
    package_t *pkg = pkg_find(pkg_name);
    
    if (!pkg) {
        puts("Package not found: ");
        puts(pkg_name);
        puts("\n");
        return -1;
    }
    
    if (!pkg->installed) {
        puts("Package not installed: ");
        puts(pkg_name);
        puts("\n");
        return 0;
    }
    
    // Não permitir remover pacotes essenciais
    if (strcmp(pkg_name, "sysutils") == 0 || strcmp(pkg_name, "nano") == 0) {
        puts("Cannot remove essential package: ");
        puts(pkg_name);
        puts("\n");
        return -1;
    }
    
    pkg->installed = 0;
    
    // Remover arquivos do pacote
    if (strcmp(pkg_name, "wget") == 0) {
        fs_delete_entry("/bin/wget");
    } else if (strcmp(pkg_name, "snake") == 0) {
        fs_delete_entry("/bin/snake");
    }
    
    puts("✅ Removed package: ");
    puts(pkg_name);
    puts("\n");
    
    return 0;
}

int pkg_list() {
    puts("\nInstalled packages:\n");
    puts("===================\n");
    
    for (int i = 0; i < package_count; i++) {
        if (available_packages[i].installed) {
            puts("✓ ");
            puts(available_packages[i].name);
            puts(" - ");
            puts(available_packages[i].description);
            puts(" (v");
            puts(available_packages[i].version);
            puts(")\n");
        }
    }
    
    puts("\nAvailable packages:\n");
    puts("===================\n");
    
    for (int i = 0; i < package_count; i++) {
        if (!available_packages[i].installed) {
            puts("  ");
            puts(available_packages[i].name);
            puts(" - ");
            puts(available_packages[i].description);
            puts(" (v");
            puts(available_packages[i].version);
            puts(")\n");
        }
    }
    
    return 0;
}

int pkg_search(const char *query) {
    puts("\nSearch results for '");
    puts(query);
    puts("':\n");
    
    int found = 0;
    for (int i = 0; i < package_count; i++) {
        if (strstr(available_packages[i].name, query) || 
            strstr(available_packages[i].description, query)) {
            puts(available_packages[i].installed ? "✓ " : "  ");
            puts(available_packages[i].name);
            puts(" - ");
            puts(available_packages[i].description);
            puts(" (v");
            puts(available_packages[i].version);
            puts(")\n");
            found = 1;
        }
    }
    
    if (!found) {
        puts("No packages found.\n");
    }
    
    return found;
}

package_t* pkg_find(const char *pkg_name) {
    for (int i = 0; i < package_count; i++) {
        if (strcmp(available_packages[i].name, pkg_name) == 0) {
            return &available_packages[i];
        }
    }
    return NULL;
}