#include "fs.h"
#include "vga.h"
#include "string.h"

static fs_entry_t filesystem[MAX_FILES + MAX_DIRS];
static int fs_entry_count = 0;
static fs_entry_t *root_dir = NULL;

void fs_init() {
    // Inicializar sistema de arquivos
    fs_entry_count = 0;

    // Criar diretório raiz
    root_dir = fs_create_directory("", "/");

    // Criar alguns arquivos e diretórios padrão
    fs_create_directory("/", "home");
    fs_create_directory("/", "etc");
    fs_create_directory("/", "bin");
    fs_create_directory("/home", "user");
    fs_create_directory("/home", "documents");

    fs_create_file("/etc", "motd");
    fs_write_file(fs_find_entry("/etc/motd"), "Welcome to Cocoon OS!\n");

    fs_create_file("/", "readme.txt");
    fs_write_file(fs_find_entry("/readme.txt"),
                "Cocoon OS v0.1\n"
                "Operational System Developed by KIBI");

    fs_create_file("/home/user", "documento.txt");
    fs_write_file(fs_find_entry("/home/user/documento.txt"),
                 "This is an example archive in the directory home/user.\n");

    fs_create_file("/home/user", "config.txt");
    fs_write_file(fs_find_entry("/home/user/config.txt"),
                 "User Configurations:\n"
                 "Theme: dark\n"
                 "Language: english\n"
                 "Shell: Cocoon Terminal\n");
}

fs_entry_t* fs_create_file(const char *path, const char *name) {
    if (fs_entry_count >= MAX_FILES + MAX_DIRS) return NULL;
    
    // Verificar se já existe um arquivo com esse nome no mesmo diretório
    char full_path[MAX_PATH_LEN];
    strcpy(full_path, path);
    if (strlen(path) > 0 && path[strlen(path)-1] != '/') {
        strcat(full_path, "/");
    }
    strcat(full_path, name);
    
    // Se já existe, retornar o existente
    fs_entry_t *existing = fs_find_entry(full_path);
    if (existing) {
        return existing;
    }
    
    // Criar novo arquivo
    fs_entry_t *entry = &filesystem[fs_entry_count++];
    entry->type = FS_FILE;
    
    strcpy(entry->path, full_path);
    strcpy(entry->data.file.name, name);
    entry->data.file.size = 0;
    entry->data.file.content[0] = '\0';
    entry->data.file.permissions = 0644; // rw-r--r--
    
    // Adicionar ao diretório pai
    fs_entry_t *parent = fs_find_entry(path);
    if (parent && parent->type == FS_DIRECTORY) {
        parent->data.dir.entries[parent->data.dir.entry_count++] = entry;
        entry->parent = parent;
    }
    
    return entry;
}

fs_entry_t* fs_create_directory(const char *path, const char *name) {
    if (fs_entry_count >= MAX_FILES + MAX_DIRS) return NULL;
    
    fs_entry_t *entry = &filesystem[fs_entry_count++];
    entry->type = FS_DIRECTORY;
    
    // Construir path completo
    strcpy(entry->path, path);
    if (strlen(path) > 0 && path[strlen(path)-1] != '/') {
        strcat(entry->path, "/");
    }
    strcat(entry->path, name);
    
    strcpy(entry->data.dir.name, name);
    entry->data.dir.entry_count = 0;
    
    // Adicionar ao diretório pai (exceto para root)
    if (strlen(path) > 0) {
        fs_entry_t *parent = fs_find_entry(path);
        if (parent && parent->type == FS_DIRECTORY) {
            parent->data.dir.entries[parent->data.dir.entry_count++] = entry;
            entry->parent = parent;
        }
    }
    
    return entry;
}

fs_entry_t* fs_find_entry(const char *path) {
    if (strcmp(path, "/") == 0) return root_dir;
    
    for (int i = 0; i < fs_entry_count; i++) {
        if (strcmp(filesystem[i].path, path) == 0) {
            return &filesystem[i];
        }
    }
    return NULL;
}

int fs_write_file(fs_entry_t *file, const char *content) {
    if (!file || file->type != FS_FILE) return -1;
    
    size_t len = strlen(content);
    if (len >= FILE_CONTENT_SIZE) len = FILE_CONTENT_SIZE - 1;
    
    strncpy(file->data.file.content, content, len);
    file->data.file.content[len] = '\0';
    file->data.file.size = len;
    
    return 0;
}

char* fs_read_file(fs_entry_t *file) {
    if (!file || file->type != FS_FILE) return NULL;
    return file->data.file.content;
}

int fs_list_directory(const char *path) {
    fs_entry_t *dir = fs_find_entry(path);
    if (!dir || dir->type != FS_DIRECTORY) return -1;
    
    puts("\n");
    
    if (dir->data.dir.entry_count == 0) {
        puts("Empty directory\n");
        return 0;
    }
    
    for (int i = 0; i < dir->data.dir.entry_count; i++) {
        fs_entry_t *entry = dir->data.dir.entries[i];
        if (entry->type == FS_DIRECTORY) {
            set_color(vga_entry_color(VGA_LIGHT_BLUE, VGA_BLACK));
            puts(entry->data.dir.name);
            puts("/\n");
        } else {
            set_color(vga_entry_color(VGA_WHITE, VGA_BLACK));
            puts(entry->data.file.name);
            // Mostrar tamanho do arquivo
            puts(" (");
            // Função simples para mostrar tamanho
            char size_str[10];
            int size = entry->data.file.size;
            if (size < 1024) {
                // Convert number to string manually
                char *ptr = size_str;
                int n = size;
                if (n == 0) {
                    *ptr++ = '0';
                } else {
                    char temp[10];
                    char *t = temp;
                    while (n > 0) {
                        *t++ = '0' + (n % 10);
                        n /= 10;
                    }
                    while (t > temp) {
                        *ptr++ = *--t;
                    }
                }
                *ptr++ = 'B';
                *ptr = '\0';
                puts(size_str);
            } else {
                puts("1KB");
            }
            puts(")\n");
        }
    }
    set_color(vga_entry_color(VGA_WHITE, VGA_BLACK));
    return 0;
}

int fs_delete_entry(const char *path) {
    fs_entry_t *entry = fs_find_entry(path);
    if (!entry) return -1;
    
    // Remover do diretório pai
    if (entry->parent && entry->parent->type == FS_DIRECTORY) {
        directory_t *parent_dir = &entry->parent->data.dir;
        for (int i = 0; i < parent_dir->entry_count; i++) {
            if (parent_dir->entries[i] == entry) {
                // Mover os elementos restantes
                for (int j = i; j < parent_dir->entry_count - 1; j++) {
                    parent_dir->entries[j] = parent_dir->entries[j + 1];
                }
                parent_dir->entry_count--;
                break;
            }
        }
    }
    
    // Marcar entrada como livre
    entry->path[0] = '\0';
    return 0;
}

fs_entry_t* fs_find_entry_relative(const char *base_path, const char *relative_path) {
    char full_path[MAX_PATH_LEN];
    
    // Se o path for absoluto, use diretamente
    if (relative_path[0] == '/') {
        return fs_find_entry(relative_path);
    }
    
    // Construir path completo
    strcpy(full_path, base_path);
    
    // Adicionar barra se necessário
    if (strcmp(base_path, "/") != 0 && relative_path[0] != '\0') {
        strcat(full_path, "/");
    }
    
    strcat(full_path, relative_path);
    
    return fs_find_entry(full_path);
}