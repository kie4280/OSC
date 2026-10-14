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
  uart_puts("Bootloader: Hardware initialized.\n", 34);
  // while (1) {
  //   for (int a=0; a<1000000; a++) {
  //     // Busy wait to simulate some delay
  //   }
  //   uart_puts("Bootloader: Hardware initialized.\n", 34);
  // }
}