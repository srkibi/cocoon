#include "vga.h"
#include "terminal.h"
#include "keyboard.h"
#include "fs.h"
#include "string.h"
#include "package.h"
#include "wget.h"

#define COMMAND_BUFFER_SIZE 256
#define MAX_ARGS 10
#define HISTORY_SIZE 50

static char command_buffer[COMMAND_BUFFER_SIZE];
static int buffer_index = 0;
static int input_enabled = 1;
static char current_path[MAX_PATH_LEN] = "/";

static char command_history[HISTORY_SIZE][COMMAND_BUFFER_SIZE];
static int history_count = 0;
static int history_index = -1;
static int prompt_start_x = 0, prompt_start_y = 0;
static char cursor_offset = 0;
static int prompt_length = 0;

static void parse_command(char *cmd, char *argv[], int *argc) {
    *argc = 0;
    char *token = cmd;
    int in_quotes = 0;
    
    while (*token && *argc < MAX_ARGS) {
        while (*token == ' ') token++;
        if (!*token) break;
        
        if (*token == '"') {
            in_quotes = 1;
            token++;
            argv[(*argc)++] = token;
            
            while (*token && (*token != '"' || in_quotes)) {
                if (*token == '\\' && *(token+1) == '"') {
                    token++;
                }
                token++;
            }
        } else {
            argv[(*argc)++] = token;
            
            while (*token && *token != ' ') token++;
        }
        
        if (*token) {
            *token = '\0';
            token++;
        }
    }
}

void cmd_pkg(const char *args) {
    if (!args || strlen(args) == 0) {
        puts("\nCocoon OS Package Manager\n");
        puts("Usage: pkg <command> [package]\n");
        puts("Commands:\n");
        puts("  install <pkg>  - Install a package\n");
        puts("  remove <pkg>   - Remove a package\n");
        puts("  list           - List packages\n");
        puts("  search <query> - Search packages\n");
        return;
    }
    
    char command[32];
    char pkg_name[32];
    
    if (strncmp(args, "install ", 8) == 0) {
        strcpy(pkg_name, args + 8);
        pkg_install(pkg_name);
    } else if (strncmp(args, "remove ", 7) == 0) {
        strcpy(pkg_name, args + 7);
        pkg_remove(pkg_name);
    } else if (strcmp(args, "list") == 0) {
        pkg_list();
    } else if (strncmp(args, "search ", 7) == 0) {
        strcpy(pkg_name, args + 7);
        pkg_search(pkg_name);
    } else {
        puts("Unknown pkg command: ");
        puts(args);
        puts("\n");
    }
}

void cmd_wget(const char *args) {
    if (!args || strlen(args) == 0) {
        wget_show_help();
        return;
    }
    
    char *space = strchr(args, ' ');
    if (!space) {
        puts("Usage: wget <URL> <output file>\n");
        return;
    }
    
    *space = '\0';
    char *url = (char*)args;
    char *output_file = space + 1;
    
    wget_download(url, output_file);
}

static int get_cursor_text_position() {
    int x, y;
    get_cursor_position(&x, &y);
    return cursor_offset + (x - prompt_start_x);
}

static void set_cursor_text_position(int text_pos) {
    if (text_pos < 0) text_pos = 0;
    if (text_pos > buffer_index) text_pos = buffer_index;
    
    cursor_offset = text_pos;
    
    int x = prompt_start_x + prompt_length + text_pos;
    int y = prompt_start_y;
    
    while (x >= VGA_WIDTH) {
        x -= VGA_WIDTH;
        y++;
    }
    
    set_cursor_xy(x, y);
}

void clear_current_line() {
    int x, y;
    get_cursor_position(&x, &y);
    
    set_cursor_xy(prompt_start_x, prompt_start_y);
    
    for (int i = prompt_start_x; i < VGA_WIDTH; i++) {
        putchar(' ');
    }
    
    set_cursor_xy(prompt_start_x, prompt_start_y);
    cursor_offset = 0;
}

static void redraw_command_line() {
    int current_x, current_y;
    get_cursor_position(&current_x, &current_y);
    
    set_cursor_xy(prompt_start_x, prompt_start_y);
    
    for (int i = prompt_start_x; i < VGA_WIDTH; i++) {
        putchar(' ');
    }

    set_cursor_xy(prompt_start_x, prompt_start_y);
    
    set_color(vga_entry_color(VGA_LIGHT_GREEN, VGA_BLACK));
    puts("cocoon@root:");
    set_color(vga_entry_color(VGA_LIGHT_BLUE, VGA_BLACK));
    puts("~");
    puts(current_path);
    puts("$ ");
    set_color(vga_entry_color(VGA_WHITE, VGA_BLACK));
    
    puts(command_buffer);
    
    set_cursor_text_position(cursor_offset);
}

void terminal_prompt() {
    set_color(vga_entry_color(VGA_LIGHT_GREEN, VGA_BLACK));

    get_cursor_position(&prompt_start_x, &prompt_start_y);

    puts("cocoon@root:");
    set_color(vga_entry_color(VGA_LIGHT_BLUE, VGA_BLACK));
    puts("~");
    puts(current_path);
    puts("$ ");

    prompt_length = strlen("cocoon@root:~") + strlen(current_path) + 2;

    set_color(vga_entry_color(VGA_WHITE, VGA_BLACK));
    input_enabled = 1;
    buffer_index = 0;
    command_buffer[0] = '\0';
    history_index = -1;
    cursor_offset = 0;
}

void terminal_initialize() {
    clear_screen();

    fs_init();
    pkg_init();

    set_color(vga_entry_color(VGA_WHITE, VGA_BLACK));
    puts("|                                        |\n");
    puts("|           Cocoon OS Terminal           |\n");
    puts("|   Type 'help' for available commands.  |\n");
    puts("|                                        |\n\n");

    terminal_prompt();
}

void cmd_ls(const char *args) {
    char path_to_list[MAX_PATH_LEN];
    
    if (!args || strlen(args) == 0) {
        strcpy(path_to_list, current_path);
    } else if (args[0] == '/') {
        strcpy(path_to_list, args);
    } else {
        strcpy(path_to_list, current_path);
        if (strcmp(current_path, "/") != 0) {
            strcat(path_to_list, "/");
        }
        strcat(path_to_list, args);
    }
    
    if (fs_list_directory(path_to_list) == -1) {
        puts("Directory not found: ");
        puts(path_to_list);
        puts("\n");
    }
}

void cmd_cat(const char *args) {
    if (!args || strlen(args) == 0) {
        puts("Use: cat <file>\n");
        return;
    }
    
    fs_entry_t *file;
    
    if (args[0] == '/') {
        file = fs_find_entry(args);
    } else {
        char full_path[MAX_PATH_LEN];
        strcpy(full_path, current_path);
        if (strcmp(current_path, "/") != 0) {
            strcat(full_path, "/");
        }
        strcat(full_path, args);
        file = fs_find_entry(full_path);
    }
    
    if (!file || file->type != FS_FILE) {
        puts("File not found: ");
        puts(args);
        puts("\n");
        
        puts("Available files ");
        puts(current_path);
        puts(":\n");
        fs_list_directory(current_path);
        return;
    }
    
    char *content = fs_read_file(file);
    puts("\n");
    puts(content);
    puts("\n");
}

void cmd_mkdir(const char *args) {
    if (!args || strlen(args) == 0) {
        puts("Use: mkdir <directory>\n");
        return;
    }
    
    char full_path[MAX_PATH_LEN];
    strcpy(full_path, current_path);
    if (strcmp(current_path, "/") != 0) {
        strcat(full_path, "/");
    }
    strcat(full_path, args);
    
    if (fs_create_directory(current_path, args)) {
        puts("Directory created: ");
        puts(args);
        puts("\n");
    } else {
        puts("Failed to create directory\n");
    }
}

void cmd_touch(const char *args) {
    if (!args || strlen(args) == 0) {
        puts("Use: touch <file>\n");
        return;
    }
    
    if (fs_create_file(current_path, args)) {
        puts("File created: ");
        puts(args);
        puts("\n");
    } else {
        puts("Failed to create file\n");
    }
}

void cmd_echo(const char *args) {
    if (!args || strlen(args) == 0) {
        puts("\n");
        return;
    }

    char *redirect = strstr(args, ">");
    if (redirect) {
        *redirect = '\0';
        char *filename = redirect + 1;
        
        while (*filename == ' ') filename++;
        
        char full_path[MAX_PATH_LEN];
        if (filename[0] == '/') {
            strcpy(full_path, filename);
        } else {
            strcpy(full_path, current_path);
            if (strcmp(current_path, "/") != 0) {
                strcat(full_path, "/");
            }
            strcat(full_path, filename);
        }
        
        fs_entry_t *file = fs_find_entry(full_path);
        if (!file) {
            char *last_slash = strrchr(full_path, '/');
            char *file_name;
            char parent_path[MAX_PATH_LEN];
            
            if (last_slash) {
                file_name = last_slash + 1;
                strncpy(parent_path, full_path, last_slash - full_path);
                parent_path[last_slash - full_path] = '\0';
                
                if (parent_path[0] == '\0') {
                    strcpy(parent_path, "/");
                }
            } else {
                file_name = filename;
                strcpy(parent_path, current_path);
            }
            
            file = fs_create_file(parent_path, file_name);
            if (!file) {
                puts("Failed to create file: ");
                puts(filename);
                puts("\n");
                return;
            }
        }
        
        if (file && file->type == FS_FILE) {
            char *text = (char*)args;
            while (*text == ' ') text++;
            
            fs_write_file(file, text);
            puts("Text written: ");
            puts(filename);
            puts("\n");
        } else {
            puts("Error: ");
            puts(filename);
            puts(" is not a valid file\n");
        }
    } else {
        puts("\n");
        puts(args);
        puts("\n");
    }
}

void cmd_echo_append(const char *args) {
    if (!args || strlen(args) == 0) {
        puts("\n");
        return;
    }

    char *redirect = strstr(args, ">>");
    if (redirect) {
        *redirect = '\0';
        char *filename = redirect + 2;
        
        while (*filename == ' ') filename++;
        
        char full_path[MAX_PATH_LEN];
        if (filename[0] == '/') {
            strcpy(full_path, filename);
        } else {
            strcpy(full_path, current_path);
            if (strcmp(current_path, "/") != 0) {
                strcat(full_path, "/");
            }
            strcat(full_path, filename);
        }
        
        fs_entry_t *file = fs_find_entry(full_path);
        if (!file) {
            puts("File not found: ");
            puts(filename);
            puts("\n");
            return;
        }
        
        if (file && file->type == FS_FILE) {
            char *current_content = fs_read_file(file);
            char new_content[FILE_CONTENT_SIZE];
            
            strcpy(new_content, current_content);
            strcat(new_content, args);
            
            fs_write_file(file, new_content);
            puts("Text written in: ");
            puts(filename);
            puts("\n");
        }
    }
}

void cmd_cd(const char *args) {
    if (!args || strlen(args) == 0) {
        fs_entry_t *home = fs_find_entry("/home");
        if (home) {
            strcpy(current_path, "/home");
        } else {
            strcpy(current_path, "/");
        }
        return;
    }
    
    if (strcmp(args, "..") == 0) {
        if (strcmp(current_path, "/") != 0) {
            char *last_slash = strrchr(current_path, '/');
            if (last_slash && last_slash != current_path) {
                *last_slash = '\0';
            } else {
                strcpy(current_path, "/");
            }
        }
    } else if (strcmp(args, ".") == 0) {
        return;
    } else if (strcmp(args, "/") == 0) {
        strcpy(current_path, "/");
    } else {
        fs_entry_t *dir = fs_find_entry_relative(current_path, args);
        
        if (dir && dir->type == FS_DIRECTORY) {
            strcpy(current_path, dir->path);
        } else {
            dir = fs_find_entry(args);
            if (dir && dir->type == FS_DIRECTORY) {
                strcpy(current_path, dir->path);
            } else {
                puts("Directorie not found: ");
                puts(args);
                puts("\n");
                
                puts("Available directories ");
                puts(current_path);
                puts(":\n");
                fs_list_directory(current_path);
            }
        }
    }
}

void cmd_rm(const char *args) {
    if (!args || strlen(args) == 0) {
        puts("Use: rm <file>\n");
        return;
    }
    
    char full_path[MAX_PATH_LEN];
    if (args[0] == '/') {
        strcpy(full_path, args);
    } else {
        strcpy(full_path, current_path);
        if (strcmp(current_path, "/") != 0) {
            strcat(full_path, "/");
        }
        strcat(full_path, args);
    }
    
    if (fs_delete_entry(full_path) == 0) {
        puts("Removed file: ");
        puts(args);
        puts("\n");
    } else {
        puts("Failed to remove file: ");
        puts(args);
        puts("\n");
    }
}

void cmd_pwd() {
    puts("\n");
    puts(current_path);
    puts("\n");
}

static void move_cursor_left() {
    if (cursor_offset > 0) {
        cursor_offset--;
        set_cursor_text_position(cursor_offset);
    }
}

static void move_cursor_right() {
    if (cursor_offset < buffer_index) {
        cursor_offset++;
        set_cursor_text_position(cursor_offset);
    }
}

static void move_cursor_home() {
    cursor_offset = 0;
    set_cursor_text_position(0);
}

static void move_cursor_end() {
    cursor_offset = buffer_index;
    set_cursor_text_position(buffer_index);
}

static void insert_char_at_cursor(char c) {
    if (buffer_index >= COMMAND_BUFFER_SIZE - 1) return;
    
    if (history_index != -1) {
        history_index = -1;
    }
    
    for (int i = buffer_index; i > cursor_offset; i--) {
        command_buffer[i] = command_buffer[i - 1];
    }
    
    command_buffer[cursor_offset] = c;
    buffer_index++;
    command_buffer[buffer_index] = '\0';
    
    redraw_command_line();
    
    cursor_offset++;
    set_cursor_text_position(cursor_offset);
}

static void delete_char_at_cursor() {
    if (cursor_offset == 0 || buffer_index == 0) return;
    
    for (int i = cursor_offset - 1; i < buffer_index - 1; i++) {
        command_buffer[i] = command_buffer[i + 1];
    }
    
    buffer_index--;
    command_buffer[buffer_index] = '\0';
    cursor_offset--;
    
    redraw_command_line();
}

void terminal_execute_command() {
    input_enabled = 0;

    if (command_buffer[0] == '\0') {
        terminal_prompt();
        return;
    }
    
    char *argv[MAX_ARGS];
    int argc = 0;
    parse_command(command_buffer, argv, &argc);
    
    if (argc == 0) {
        terminal_prompt();
        return;
    }
    
    char *cmd = argv[0];
    char *args = (argc > 1) ? argv[1] : NULL;

    if(strcmp(command_buffer, "help") == 0) {
        puts("                   Available commands                   \n");
        puts("   help                       - Show this help message\n");
        puts("   clear                      - Clear the terminal screen\n");
        puts("   echo <msg>                 - Print message to terminal\n");
        puts("   ls [path]                  - List files in directory\n");
        puts("   cat <file>                 - Display file contents\n");
        puts("   mkdir <directory>          - Create a new directory\n");
        puts("   touch <file>               - Create a new file\n");
        puts("   cd <directory>             - Change current directory\n");
        puts("   pwd                        - Show current directory\n");
        puts("   rm <file>                  - Remove a file\n");
        puts("   pkg <command>              - Package manager\n");
        puts("   wget <url> <file>          - Download files from web\n");
        puts("   reboot                     - Reboot the system\n");
        puts("   version                    - Show OS version\n");
    } else if(strcmp(command_buffer, "clear") == 0) {
        clear_screen();
    } else if(strcmp(command_buffer, "version") == 0) {
        puts("Cocoon OS Version 1.0\n");
    } else if (strcmp(cmd, "ls") == 0) {
        cmd_ls(args);
    } else if (strcmp(cmd, "cat") == 0) {
        cmd_cat(args);
    } else if (strcmp(cmd, "mkdir") == 0) {
        cmd_mkdir(args);
    } else if (strcmp(cmd, "touch") == 0) {
        cmd_touch(args);
    } else if (strcmp(cmd, "echo") == 0) {
        cmd_echo(args);
    } else if (strncmp(command_buffer, "echo ", 5) == 0 && strstr(command_buffer, ">>")) {
        cmd_echo_append(args);
    } else if (strcmp(cmd, "cd") == 0) {
        cmd_cd(args);
    } else if (strcmp(cmd, "pwd") == 0) {
        cmd_pwd();
    } else if (strcmp(cmd, "rm") == 0) {
        cmd_rm(args);
    } else if (strcmp(cmd, "pkg") == 0) {
        cmd_pkg(args);
    } else if (strcmp(cmd, "wget") == 0) {
        cmd_wget(args);
    } else if(strcmp(command_buffer, "reboot") == 0) {
        puts("Rebooting system...\n");
    } else if(strcmp(command_buffer, "echo")) {
        cmd_echo(args);
    } else {
        puts("\nCommand not found: ");
        puts(cmd);
        puts("\nType 'help' for a list of commands.\n");
    }

    terminal_prompt();
}

void terminal_handle_input(char c) {
    if (!input_enabled) return;
    
    if (c == '\n' || c == '\r') {
        putchar('\n');
        terminal_execute_command();
    } else if (c == '\b') {
        delete_char_at_cursor();
    } else if (c == 0x13) {
        move_cursor_left();
    } else if (c == 0x14) {
        move_cursor_right();
    } else if (c == 0x01) {
        move_cursor_home();
    } else if (c == 0x05) {
        move_cursor_end();
    } else if (c == 0x04) {
        if (cursor_offset < buffer_index) {
            cursor_offset++;
            delete_char_at_cursor();
        }
    } else if (c >= 32 && c <= 126) {
        if (history_index != -1) {
            history_index = -1;
        }
        
        insert_char_at_cursor(c);
    }
}

void terminal_run() {
    terminal_initialize();
    keyboard_init();
    
    while(1) {
        char c = keyboard_getchar();
        if (c != 0) {
            terminal_handle_input(c);
        }
    }
}