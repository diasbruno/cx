#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "char.h"

#define nil NULL

typedef FILE file_t;

typedef void (*read_word_t)(file_t* file, char* buffer);

enum tokens { IDENTIFIER, NUMBER };

struct token_t {
  int type, col, lin;
  char* name;
};

void log_token(struct token_t* t) {
  switch (t->type) {
  case IDENTIFIER: {
    printf("- {token IDENTIFIER %s (%d, %d)}\n\n",
           t->name,
           t->lin,
           t->col);
  } break;
  case NUMBER: {
    printf("- {token NUMBER %s (%d, %d)}\n\n",
           t->name,
           t->lin,
           t->col);
  } break;
  }
}

void free_token(struct token_t* t) {
  free(t->name);
  free(t);
}

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
  // 09   - octal
  // 0x11 - hex

  int ch = 0;
  if (buffer[0] == '0') {
    ch = getc(file);
    *(++buffer) = ch;
    if (ch == 'x') {
      printf("hex\n");
      while ((ch = getc(file)) > EOF && is_hex_num(ch)) {
        *(++buffer) = ch;
      }
    } else if (ch == 'b') {
      printf("binary\n");
      while ((ch = getc(file)) > EOF && is_binary_num(ch)) {
        *(++buffer) = ch;
      }
    } else if (is_octal_num(ch)) {
      printf("octal\n");
      while ((ch = getc(file)) > EOF && is_octal_num(ch)) {
        *(++buffer) = ch;
      }
    }
  } else {
    printf("regular number\n");

    while ((ch = getc(file)) > EOF && is_number(ch)) {
      *(++buffer) = ch;
    }
  }

  ungetc(ch, file);
  *(++buffer) = 0;
}

struct token_t* create_token(int type, const char* name,
                             int col, int lin) {
  struct token_t* t = malloc(sizeof(struct token_t));
  t->type = type;
  t->col = col;
  t->lin = lin;
  t->name = (char*)name;
  return t;
}

int main(int count, char* args[]) {
  read_word_t reader[] = {read_identifier,
                          read_number};

  int lineno = 1, col = 1;

  char buff[1024] = {0};
  int ch = 0;
  int use = 0;
  file_t* f = fopen(args[1], "r");
  while ((ch = getc(f)) > EOF) {
    if (ch == '\n') {
      printf("[new line]\n\n");
      lineno++, col = 1;
      continue;
    } else if (is_space(ch)) {
      int s = 1;
      while ((ch = getc(f)) > EOF && is_space(ch)) { s++; }
      printf("[spaces %d]\n\n", s);
      col += s;
    }

    buff[0] = ch;

    if (is_identifier(ch)) {
      use = IDENTIFIER;
    }
    if (is_number(ch)) {
      use = NUMBER;
    }

    read_word_t* r = reader + use;
    (*r)(f, buff);
    struct token_t* t = create_token(use, strdup(buff), col, lineno);
    col += strlen(buff) - 1;
    log_token(t);
    free_token(t);
  }

  fclose(f);

  return 0;
}
