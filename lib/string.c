#include "string.h"

#include <stdint.h>

void* memcpy(void* restrict destination, const void* restrict source,
             size_t length) {
  unsigned char* dst = destination;
  const unsigned char* src = source;

  for (size_t i = 0U; i < length; ++i) {
    dst[i] = src[i];
  }
  return destination;
}

void* memmove(void* destination, const void* source, size_t length) {
  unsigned char* dst = destination;
  const unsigned char* src = source;

  if (length == 0U || destination == source) {
    return destination;
  }

  if ((uintptr_t)dst < (uintptr_t)src) {
    for (size_t i = 0U; i < length; ++i) {
      dst[i] = src[i];
    }
  } else {
    for (size_t i = length; i > 0U; --i) {
      dst[i - 1U] = src[i - 1U];
    }
  }
  return destination;
}

void* memset(void* destination, int value, size_t length) {
  unsigned char* dst = destination;
  const unsigned char byte = (unsigned char)value;

  for (size_t i = 0U; i < length; ++i) {
    dst[i] = byte;
  }
  return destination;
}

int strcmp(const char* lhs, const char* rhs) {
  const unsigned char* left = (const unsigned char*)lhs;
  const unsigned char* right = (const unsigned char*)rhs;

  while (*left != '\0' && *left == *right) {
    ++left;
    ++right;
  }
  return (int)*left - (int)*right;
}

size_t strlen(const char* text) {
  size_t length = 0U;
  while (text[length] != '\0') {
    ++length;
  }
  return length;
}
