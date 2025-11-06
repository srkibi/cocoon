#ifndef TERMINAL_H
#define TERMINAL_H

void terminal_initialize();
void terminal_run();
void terminal_prompt();
void terminal_handle_input(char c);
void terminal_execute_command();

// Comandos do sistema de arquivos
void cmd_ls(const char *args);  // Mostrar lista de arquivos
void cmd_cat(const char *args);  // Ler conteúdo de um arquivo
void cmd_mkdir(const char *args);  // Criar diretório
void cmd_touch(const char *args);  // Criar arquivo
void cmd_echo(const char *args); // Escrever em arquivo
void cmd_cd(const char *args); // Mudar diretório
void cmd_pwd(); // Mostrar diretório atual

#endif