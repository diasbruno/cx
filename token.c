#include <stdlib.h>

#include "cx.h"
#include "token.h"

static struct mem_t mem = {{0}};

struct token_t* tokens(void) {
  return (struct token_t*)(mem.buffer + sizeof(size_t));
}

static struct token_t* next_slot(int type) {
  size_t* cap = (size_t*)mem.buffer;
  struct token_t* t =
    (struct token_t*)((size_t)tokens() + *cap);
  *cap += 32;
  return t;
}

struct token_t* create_token(int type, const char* name,
                             int col, int lin) {
  struct token_t* t = next_slot(type);
  t->type = type;
  t->col = col;
  t->lin = lin;
  t->name = (char*)name;
  return t;
}

struct token_t* create_punct_token(const char name) {
  struct token_t* t = next_slot(name);
  t->type = name;
  return (struct token_t*)t;
}

struct token_t* token_next(struct token_t** t) {
  size_t base = (size_t)*t;
  struct token_t* c = (struct token_t*)base;
  if (c->type == 0) { return nil; }
  base += 32;
  *t = (struct token_t*)base;
  return (struct token_t*)base;
}

void log_token(struct token_t* t) {
  switch (t->type) {
  case IDENTIFIER: {
    printf("{token IDENTIFIER %s (%d, %d)}\n",
           t->name,
           t->lin,
           t->col);
  } break;
  case NUMBER: {
    printf("{token NUMBER %s (%d, %d)}\n",
           t->name,
           t->lin,
           t->col);
  } break;
  case PUNCT: {
    int ch = *t->name;
    printf("{token PUNCT %c (%d, %d)}\n",
           ch,
           t->lin,
           t->col);
  } break;
  case '\n': {
    printf("{token [new-line]}\n");
  } break;
  default: {
    printf("{token %c}\n", t->type);
  }
  }
}

void log_tokens(void) {
  int* mp = (int*)mem.buffer;
  printf("alloc'd %d\n", *mp);
  for (int i = 0;; i++) {
    struct token_t* t =
      (struct token_t*)((size_t)tokens() + (i * 32));
    if (t->type == NOTHING) break;
    log_token(t);
  }
}

void free_token(struct token_t* t) {
  free(t->name);
}
