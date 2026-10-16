#include "kernel.h"
#include "uart.h"

void init_hardware() {
  // Initialize UART for serial communication
  uart_init();
}

void bootloader() {
  /* bootloader calls this function to load kernel */

  // Initialize hardware components
  init_hardware();
  shell_init();
  uart_puts("Bootloader: Hardware initialized.\n", 34);
  for (int i = 0; i < 5; i++) {
    uart_puts("Bootloader: Loading kernel...\n", 31);
  }
}