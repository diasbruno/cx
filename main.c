#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "char.h"

#define nil NULL

struct mem_t {
  char buffer[4096];
} mem = {{0}};

struct mem_pos_t { int p; };

typedef FILE file_t;

typedef void (*read_word_t)(file_t* file, char* buffer);

enum tokens { NOTHING, IDENTIFIER, NUMBER, PUNCT };

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
  case PUNCT: {
    int ch = *t->name;
    printf("- {token PUNCT %c (%d, %d)}\n\n",
           ch,
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

struct token_t* create_token(int type, const char* name,
                             int col, int lin) {
  struct mem_pos_t* m = (struct mem_pos_t*)mem.buffer;
  struct token_t* t =
    (struct token_t*)(mem.buffer + sizeof(int) + m->p);
  t->type = type;
  t->col = col;
  t->lin = lin;
  t->name = (char*)name;
  m->p += sizeof(struct token_t);
  return t;
}

void log_tokens(void) {
  struct mem_pos_t* m = (struct mem_pos_t*)mem.buffer;
  if (m->p == 0) return;
  int mp = m->p;
  struct token_t* t;
  for (int i = 0; i < mp;i++) {
    t = (struct token_t*)(mem.buffer +
                          sizeof(int) +
                          (i * sizeof(struct token_t)));
    if (t->type == NOTHING) break;
    log_token(t);
  }
}

static char* table =
  "          "
  "          "
  "          "
  "   !\"#$%&'"
  "()*+,-./01"
  "23456789:;"
  "<=>?@ABCDE"
  "FGHIJKLMNO"
  "PQRSTUVWXY"
  "Z[\\]^_`abc"
  "defghijklm"
  "nopqrstuvw"
  "xyz{|}~   ";

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
      printf("is punct %c %d %c\n", ch, ch, *(table + ch));
      create_token(PUNCT, table + ch, col, lineno);
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
      create_token(use, strdup(buff), col, lineno);
      col += strlen(buff) - 1;
    }
  }
  log_tokens();

  fclose(f);

  return 0;
}
