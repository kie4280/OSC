#ifndef RPI_SHELL_H
#define RPI_SHELL_H

#include <stddef.h>
#include <stdint.h>

#define SHELL_HISTORY_SIZE 32U
#define SHELL_INPUT_MAX_LINE_SIZE 128U

typedef enum {
  SHELL_ESCAPE_NONE,
  SHELL_ESCAPE_STARTED,
  SHELL_ESCAPE_CSI,
  SHELL_ESCAPE_SS3,
} shell_escape_state_t;

typedef struct {
  char history[SHELL_HISTORY_SIZE][SHELL_INPUT_MAX_LINE_SIZE];
  char history_draft[SHELL_INPUT_MAX_LINE_SIZE];
  char input[SHELL_INPUT_MAX_LINE_SIZE];
  size_t input_length;
  size_t cursor;
  size_t history_count;
  size_t history_next;
  size_t history_offset;
  shell_escape_state_t escape_state;
  uint8_t last_input_was_cr;
} shell_state_t;

void shell_init(shell_state_t* shell);
void shell_loop(shell_state_t* shell);

#endif  // RPI_SHELL_H
