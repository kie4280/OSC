#include "shell.h"

#include "commands.h"
#include "lib/string.h"
#include "uart.h"

#define SHELL_PROMPT "> "
#define SHELL_CLEAR_SCREEN "\x1b[2J\x1b[H"
#define SHELL_CLEAR_TO_END "\x1b[K"
#define SHELL_CURSOR_LEFT "\x1b[D"
#define SHELL_CURSOR_RIGHT "\x1b[C"

static void shell_write(const char* text) {
  (void)uart_puts(text, strlen(text));
}

static inline void shell_write_prompt(void) { shell_write(SHELL_PROMPT); }

static void shell_redraw_line(const shell_state_t* shell) {
  shell_write("\r");
  shell_write(SHELL_PROMPT);
  shell_write(shell->input);
  shell_write(SHELL_CLEAR_TO_END);

  // Reposition the terminal cursor from the end of the displayed input.
  for (size_t i = shell->cursor; i < shell->input_length; ++i) {
    shell_write(SHELL_CURSOR_LEFT);
  }
}

static void shell_clear_screen(const shell_state_t* shell) {
  shell_write(SHELL_CLEAR_SCREEN);
  shell_redraw_line(shell);
}

static void shell_add_history(shell_state_t* shell) {
  if (shell->input_length == 0U) {
    return;
  }

  if (shell->history_count > 0U) {
    const size_t last =
        (shell->history_next + SHELL_HISTORY_SIZE - 1U) % SHELL_HISTORY_SIZE;
    if (strcmp(shell->history[last], shell->input) == 0) {
      return;
    }
  }

  memcpy(shell->history[shell->history_next], shell->input,
         shell->input_length + 1U);
  shell->history_next = (shell->history_next + 1U) % SHELL_HISTORY_SIZE;
  if (shell->history_count < SHELL_HISTORY_SIZE) {
    ++shell->history_count;
  }
}

static void shell_load_history(shell_state_t* shell, size_t offset) {
  const size_t index =
      (shell->history_next + SHELL_HISTORY_SIZE - offset) % SHELL_HISTORY_SIZE;

  size_t length = strlen(shell->history[index]);
  if (length >= SHELL_INPUT_MAX_LINE_SIZE) {
    length = SHELL_INPUT_MAX_LINE_SIZE - 1U;
  }
  memcpy(shell->input, shell->history[index], length);
  shell->input[length] = '\0';
  shell->input_length = length;
  shell->cursor = length;
  shell->history_offset = offset;
  shell_redraw_line(shell);
}

static void shell_history_up(shell_state_t* shell) {
  if (shell->history_count == 0U ||
      shell->history_offset >= shell->history_count) {
    return;
  }

  if (shell->history_offset == 0U) {
    memcpy(shell->history_draft, shell->input, shell->input_length + 1U);
  }
  shell_load_history(shell, shell->history_offset + 1U);
}

static void shell_history_down(shell_state_t* shell) {
  if (shell->history_offset == 0U) {
    return;
  }

  if (shell->history_offset == 1U) {
    size_t length = strlen(shell->history_draft);
    if (length >= SHELL_INPUT_MAX_LINE_SIZE) {
      length = SHELL_INPUT_MAX_LINE_SIZE - 1U;
    }
    memcpy(shell->input, shell->history_draft, length);
    shell->input[length] = '\0';
    shell->input_length = length;
    shell->cursor = length;
    shell->history_offset = 0U;
    shell_redraw_line(shell);
    return;
  }

  shell_load_history(shell, shell->history_offset - 1U);
}

static void shell_submit_line(shell_state_t* shell) {
  static const char newline[] = "\r\n";

  while (shell->cursor < shell->input_length) {
    shell_write(SHELL_CURSOR_RIGHT);
    ++shell->cursor;
  }
  shell_write(newline);
  shell_add_history(shell);
  shell_execute_command(shell->input);

  shell->input[0] = '\0';
  shell->input_length = 0U;
  shell->cursor = 0U;
  shell->history_offset = 0U;
  shell->history_draft[0] = '\0';
  shell_write_prompt();
}

static void shell_insert_character(shell_state_t* shell, char character) {
  if (shell->input_length >= SHELL_INPUT_MAX_LINE_SIZE - 1U) {
    return;
  }

  const size_t old_length = shell->input_length;
  memmove(shell->input + shell->cursor + 1U, shell->input + shell->cursor,
          old_length - shell->cursor + 1U);
  shell->input[shell->cursor] = character;
  ++shell->input_length;
  ++shell->cursor;
  shell->input[shell->input_length] = '\0';
  shell->history_offset = 0U;
  shell_redraw_line(shell);
}

static void shell_backspace(shell_state_t* shell) {
  if (shell->cursor == 0U) {
    return;
  }

  const size_t old_length = shell->input_length;
  memmove(shell->input + shell->cursor - 1U, shell->input + shell->cursor,
          old_length - shell->cursor + 1U);
  --shell->input_length;
  --shell->cursor;
  shell->history_offset = 0U;
  shell_redraw_line(shell);
}

static void shell_move_cursor_left(shell_state_t* shell) {
  if (shell->cursor > 0U) {
    --shell->cursor;
    shell_write(SHELL_CURSOR_LEFT);
  }
}

static void shell_move_cursor_right(shell_state_t* shell) {
  if (shell->cursor < shell->input_length) {
    ++shell->cursor;
    shell_write(SHELL_CURSOR_RIGHT);
  }
}

static void shell_process_csi_final(shell_state_t* shell, char character) {
  switch (character) {
    case 'A':
      shell_history_up(shell);
      break;
    case 'B':
      shell_history_down(shell);
      break;
    case 'C':
      shell_move_cursor_right(shell);
      break;
    case 'D':
      shell_move_cursor_left(shell);
      break;
    default:
      break;
  }
}

static void shell_process_character(shell_state_t* shell, char character) {
  if (shell->last_input_was_cr != 0U) {
    shell->last_input_was_cr = 0U;
    if (character == '\n') {
      return;
    }
  }

  if (shell->escape_state == SHELL_ESCAPE_STARTED) {
    if (character == '[') {
      shell->escape_state = SHELL_ESCAPE_CSI;
    } else if (character == 'O') {
      shell->escape_state = SHELL_ESCAPE_SS3;
    } else {
      shell->escape_state = SHELL_ESCAPE_NONE;
      shell_process_character(shell, character);
    }
    return;
  }

  if (shell->escape_state == SHELL_ESCAPE_CSI) {
    if (character >= 0x40 && character <= 0x7e) {
      shell->escape_state = SHELL_ESCAPE_NONE;
      shell_process_csi_final(shell, character);
    } else if (character < 0x20 || character > 0x3f) {
      shell->escape_state = SHELL_ESCAPE_NONE;
    }
    return;
  }

  if (shell->escape_state == SHELL_ESCAPE_SS3) {
    shell->escape_state = SHELL_ESCAPE_NONE;
    shell_process_csi_final(shell, character);
    return;
  }

  if (character == '\x1b') {
    shell->escape_state = SHELL_ESCAPE_STARTED;
  } else if (character == '\r') {
    shell->last_input_was_cr = 1U;
    shell_submit_line(shell);
  } else if (character == '\n') {
    shell_submit_line(shell);
  } else if (character == '\b' || character == 0x7f) {
    shell_backspace(shell);
  } else if (character == '\x0c') {
    shell_clear_screen(shell);
  } else if (character >= 0x20 && character <= 0x7e) {
    shell_insert_character(shell, character);
  }
}

void shell_init(shell_state_t* shell) {
  if (shell == NULL) {
    return;
  }

  memset(shell, 0, sizeof(*shell));

  static const char initialized[] = "Shell: Initialized.\r\n";
  shell_write(initialized);
  shell_write_prompt();
}

void shell_loop(shell_state_t* shell) {
  if (shell == NULL) {
    return;
  }

  for (;;) {
    char character;
    if (uart_getc(&character) == 0) {
      shell_process_character(shell, character);
    }
  }
}
