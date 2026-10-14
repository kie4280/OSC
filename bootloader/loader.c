#include "kernel.h"
#include "uart.h"

void init_hardware() {
  // Initialize UART for serial communication
  uart_init();
}

void bootstrap_kernel() {
  /* bootloader calls this function to load kernel */

  // Initialize hardware components
  init_hardware();
  while (1) {
    uart_puts("Bootloader: Hardware initialized.\n", 34);
  }
}