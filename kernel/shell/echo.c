#include "commands.h"

#include "lib/string.h"
#include "uart.h"

void shell_command_echo(const char* arguments) {
  (void)uart_puts(arguments, strlen(arguments));
  (void)uart_puts("\r\n", 2U);
}
