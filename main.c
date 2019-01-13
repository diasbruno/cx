#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "cx.h"
#include "char.h"
#include "token.h"

void parser(struct token_t* ts);

void read_identifier(file_t* file, char* buffer) {
  int ch = 0;

  while ((ch = getc(file)) > EOF && is_valid_identifier(ch)) {
    *(++buffer) = ch;
  }

  ungetc(ch, file);
  *(++buffer) = 0;
}

void read_number(file_t* file, char* buffer) {
  // 0b11 - binary
  // 09   - octaln
  // 0x11 - hex

  int ch = 0;
  if (buffer[0] == '0') {
    ch = getc(file);
    *(++buffer) = ch;
    if (ch == 'x') {
      while ((ch = getc(file)) > EOF && is_hex_num(ch)) {
        *(++buffer) = ch;
      }
    } else if (ch == 'b') {
      while ((ch = getc(file)) > EOF && is_binary_num(ch)) {
        *(++buffer) = ch;
      }
    } else if (is_octal_num(ch)) {
      while ((ch = getc(file)) > EOF && is_octal_num(ch)) {
        *(++buffer) = ch;
      }
    }
  } else {
    while ((ch = getc(file)) > EOF && is_number(ch)) {
      *(++buffer) = ch;
    }
  }

  ungetc(ch, file);
  *(++buffer) = 0;
}

int main(int count, char* args[]) {
  read_word_t reader[] = {nil,
                          read_identifier,
                          read_number,
                          nil};
  char buff[1024] = {0};
  int lineno = 1, col = 1, ch = 0, use = 0;
  file_t* f = fopen(args[1], "r");

  while ((ch = getc(f)) > EOF) {
    if (ch == '\n') {
      lineno++, col = 1;
      continue;
    } else if (is_space(ch)) {
      int s = 1;
      while ((ch = getc(f)) > EOF && is_space(ch)) { s++; }
      col += s;
    }

    if (ispunct(ch)) {
      create_punct_token(ch);
      col += 1;
    } else {

      buff[0] = ch;

      if (is_identifier(ch)) {
        use = IDENTIFIER;
      }
      if (isdigit(ch)) {
        use = NUMBER;
      }

      read_word_t* r = reader + use;
      (*r)(f, buff);
      (void)create_token(use, strdup(buff), col, lineno);
      col += strlen(buff) - 1;
    }
  }

  parser(tokens());

  fclose(f);

  return 0;
}
