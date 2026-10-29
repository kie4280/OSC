#include "commands.h"

#include "uart.h"
#include "lib/string.h"

static void shell_write(const char* text) {
  (void)uart_puts(text, strlen(text));
}

static int shell_is_space(char character) {
  return character == ' ' || character == '\t';
}

void shell_execute_command(char* input) {
  char* command = input;
  while (shell_is_space(*command)) {
    ++command;
  }
  if (*command == '\0') {
    return;
  }

  char* command_end = command;
  while (*command_end != '\0' && !shell_is_space(*command_end)) {
    ++command_end;
  }

  const char command_terminator = *command_end;
  *command_end = '\0';
  const int is_help = strcmp(command, "help") == 0;
  const int is_echo = strcmp(command, "echo") == 0;
  *command_end = command_terminator;

  char* arguments = command_end;
  while (shell_is_space(*arguments)) {
    ++arguments;
  }

  if (is_help != 0) {
    shell_command_help();
  } else if (is_echo != 0) {
    shell_command_echo(arguments);
  } else {
    shell_write("Unknown command: ");
    *command_end = '\0';
    shell_write(command);
    *command_end = command_terminator;
    shell_write("\r\n");
  }
}
