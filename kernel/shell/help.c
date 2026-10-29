#include "commands.h"

#include "uart.h"

void shell_command_help(void) {
  static const char help_text[] =
      "Available commands:\r\n"
      "  help         Show this help\r\n"
      "  echo [text]  Print text\r\n";
  (void)uart_puts(help_text, sizeof(help_text) - 1U);
}
