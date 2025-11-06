#ifndef FS_H
#define FS_H

#include <stdint.h>

#define MAX_FILENAME_LEN 32
#define MAX_PATH_LEN 256
#define MAX_FILES 128
#define MAX_DIRS 64
#define FILE_CONTENT_SIZE 1024

// Tipos de entrada
typedef enum {
    FS_FILE,
    FS_DIRECTORY
} fs_entry_type;

// Estrutura de arquivo
typedef struct {
    char name[MAX_FILENAME_LEN];
    char content[FILE_CONTENT_SIZE];
    uint32_t size;
    uint32_t permissions;
} file_t;

// Estrutura de diretório
typedef struct {
    char name[MAX_FILENAME_LEN];
    struct fs_entry *entries[MAX_FILES];
    int entry_count;
} directory_t;

// Entrada do sistema de arquivos
typedef struct fs_entry {
    fs_entry_type type;
    char path[MAX_PATH_LEN];
    
    union {
        file_t file;
        directory_t dir;
    } data;
    
    struct fs_entry *parent;
} fs_entry_t;

// Funções do sistema de arquivos
void fs_init();
fs_entry_t* fs_create_file(const char *path, const char *name);
fs_entry_t* fs_create_directory(const char *path, const char *name);
fs_entry_t* fs_find_entry(const char *path);
fs_entry_t* fs_find_entry_relative(const char *base_path, const char *relative_path);
int fs_write_file(fs_entry_t *file, const char *content);
char* fs_read_file(fs_entry_t *file);
int fs_list_directory(const char *path);
int fs_delete_entry(const char *path);

#endif