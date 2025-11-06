#include "wget.h"
#include "vga.h"
#include "string.h"
#include "fs.h"

// URLs "fictícias" para demonstração
typedef struct {
    const char *url;
    const char *content;
} web_resource_t;

web_resource_t web_resources[] = {
    {"http://repo.cocoon-os.org/packages/list", 
     "Available packages:\n- nano (text editor)\n- snake (game)\n- sysutils (system tools)"},
     
    {"http://repo.cocoon-os.org/docs/welcome.txt",
     "Welcome to Cocoon OS!\n\nThis is a simple operating system\nfor educational purposes."},
     
    {"http://repo.cocoon-os.org/games/snake.txt",
     "Snake Game Instructions:\nUse arrow keys to move\nEat food to grow\nDon't hit walls!"},
     
    {"http://repo.cocoon-os.org/utils/help.txt",
     "System Utilities Help:\n- ls: list files\n- cat: view files\n- mkdir: create directories"},
     
    {NULL, NULL}
};

int wget_download(const char *url, const char *output_file) {
    puts("Downloading: ");
    puts(url);
    puts("\n");
    
    // Simular delay de download
    for (volatile long i = 0; i < 1000000; i++);
    
    // Encontrar recurso
    web_resource_t *resource = NULL;
    for (int i = 0; web_resources[i].url != NULL; i++) {
        if (strcmp(web_resources[i].url, url) == 0) {
            resource = &web_resources[i];
            break;
        }
    }
    
    if (!resource) {
        puts("❌ Error: Could not resolve host or URL not found\n");
        return -1;
    }
    
    // Criar arquivo de saída
    fs_entry_t *file = fs_find_entry(output_file);
    if (!file) {
        // Extrair nome do arquivo do path
        char *last_slash = strrchr(output_file, '/');
        char *filename = last_slash ? last_slash + 1 : (char*)output_file;
        char parent_path[256];
        
        if (last_slash) {
            strncpy(parent_path, output_file, last_slash - output_file);
            parent_path[last_slash - output_file] = '\0';
            if (parent_path[0] == '\0') strcpy(parent_path, "/");
        } else {
            strcpy(parent_path, "/");
        }
        
        file = fs_create_file(parent_path, filename);
        if (!file) {
            puts("❌ Error: Could not create output file\n");
            return -1;
        }
    }
    
    // Escrever conteúdo
    fs_write_file(file, resource->content);
    
    puts("✅ Saved to: ");
    puts(output_file);
    puts(" (");
    
    // Mostrar tamanho
    int size = strlen(resource->content);
    char size_str[10];
    char *ptr = size_str;
    
    if (size == 0) {
        *ptr++ = '0';
    } else {
        char temp[10];
        char *t = temp;
        int n = size;
        while (n > 0) {
            *t++ = '0' + (n % 10);
            n /= 10;
        }
        while (t > temp) {
            *ptr++ = *--t;
        }
    }
    *ptr++ = ' ';
    *ptr++ = 'b';
    *ptr++ = 'y';
    *ptr++ = 't';
    *ptr++ = 'e';
    *ptr++ = 's';
    *ptr = '\0';
    
    puts(size_str);
    puts(")\n");
    
    return 0;
}

void wget_show_help() {
    puts("\nCocoon OS wget - Web file downloader\n");
    puts("Usage: wget [URL] [output file]\n");
    puts("Examples:\n");
    puts("  wget http://repo.cocoon-os.org/docs/welcome.txt /home/user/welcome.txt\n");
    puts("  wget http://repo.cocoon-os.org/packages/list /tmp/packages.txt\n");
    puts("\nAvailable URLs:\n");
    
    for (int i = 0; web_resources[i].url != NULL; i++) {
        puts("  ");
        puts(web_resources[i].url);
        puts("\n");
    }
}