#ifndef RPI_UART_H
#define RPI_UART_H

#include <stddef.h>
#include <stdint.h>

int uart_init(void);
int uart_getc(char *c);
int uart_puts(const char* str, size_t len);

#endif  // RPI_UART_H