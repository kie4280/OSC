#include "kernel.h"
#include "shell/shell.h"
#include "uart.h"

void init_hardware() {
  // Initialize UART for serial communication
  uart_init();
  uart_puts("Bootloader: Hardware initialized.\n", 34);
}

void bootloader() {
  /* bootloader calls this function to load kernel */
  static shell_state_t shell;

  // Initialize hardware components
  init_hardware();
  shell_init(&shell);
  shell_loop(&shell);
}
