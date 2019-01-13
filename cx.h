#ifndef __CX_H__
#define __CX_H__ 1

#include <stdio.h>

#define nil NULL

struct mem_t {
  char buffer[4096];
};

struct mem_pos_t { int p; };

typedef FILE file_t;

typedef void (*read_word_t)(file_t* file, char* buffer);

enum tokens { NOTHING, IDENTIFIER, NUMBER, PUNCT };

struct token_t {
  char* name;
  int type, col, lin;
};

#define next_token(ts) \
  (struct token_t*)(ts + sizeof(struct token_t))

#endif // __CX_H__
