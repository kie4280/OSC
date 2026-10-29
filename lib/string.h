#ifndef RPI_STRING_H
#define RPI_STRING_H

#include <stddef.h>

void* memcpy(void* restrict destination, const void* restrict source,
             size_t length);
void* memmove(void* destination, const void* source, size_t length);
void* memset(void* destination, int value, size_t length);
int strcmp(const char* lhs, const char* rhs);
size_t strlen(const char* text);

#endif  // RPI_STRING_H
