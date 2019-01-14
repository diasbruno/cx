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

enum tokens { NOTHING, // 26
              IDENTIFIER,
              NUMBER,
              PUNCT,
              IF_K, // 4 1
              ELSE_K, // 1
              BREAK_K,
              SWITCH_K,
              CASE_K,
              FOR_K,
              GOTO_K,
              UNION_K,
              STATIC_k,
              CONTINUE_K,
              EXTERN_K,
              CONST_K,
              SIZEOF_K,
              WHILE_K,
              DO_K,
              VOID_K,
              DEFAULT_K,
              TYPEDEF_K,
              ENUM_K,
              STRUCT_K,
              REGISTER_K,
              VOLATILE_K,
              RETURN_K };

struct token_t {
  char* name;
  int type, col, lin;
};

#define next_token(ts) \
  (struct token_t*)(ts + sizeof(struct token_t))

#endif // __CX_H__
