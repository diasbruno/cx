#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "cell.h"
#include "cx.h"
#include "token.h"

enum decl_t { INCLUDE_DECL };

struct ast_t {
  int type;
  void* object;
};

void log_token(struct token_t* t);

/*
#define a 1
(define
  {name}
  {content})

#define f(x) (x())
(define
   {name}
   {params}
   {content})
 */

static int expect_str(const char* str, const char* exp) {
  unsigned long l = strlen(exp);
  printf("expect(%s,%ld) = (%s,%ld) => %d \n",
         exp, l,
         str,
         strlen(str),
         strlen(str) == l &&
         strncmp(str, exp, l) == 0);
  return
    strlen(str) == l &&
    strncmp(str, exp, l) == 0;
}

/* static cell_t process_define_context(cell_t r, */
/*                                      struct token_t** ts) { */
/*   struct token_t* t = *ts; */

/*   switch (t->type) { */
/*   case IDENTIFIER: { */
/*     printf("is label\n"); */
/*     push_item(r, t->name); */
/*   } break; */
/*   case'(': { */
/*     printf("is function\n"); */
/*     push_item(r, t->name); */
/*   } break; */
/*   } */

/*   return r; */
/* } */

static struct ast_t* process_include_context(struct token_t** ts) {
/*
#include <stdlib.h>
(include {name})
#include a
(include {name})
 */

  struct token_t* t = *ts;

  static char filename[1024] = {0};
  int i = 0;
  int exp = t->type;
  assert(exp == IDENTIFIER ||
         exp == '"' || exp == '\'' || exp == '<');

  exp = exp == '<' ? '>' : exp;

  while ((t = token_next(ts)) &&
         t->type != exp) {
    if (t->type < '!') {
      size_t l = strlen(t->name);
      memcpy(filename + i, t->name, l);
      i += l;
    } else {
      filename[i] = t->type;
      i++;
    }

    printf("filename %c %c %s\n", exp, t->type, filename);
  }
  filename[i] = 0;

  struct ast_t* a = (struct ast_t*)malloc(sizeof(struct ast_t));
  a->type = INCLUDE_DECL;
  a->object = strdup(filename);

  return a;
}

static struct ast_t* processor_context(struct token_t** ts) {
  // #include <stdio.h>
  // #define a 000
  // #define f(x) ...
  struct token_t* t = *ts;
  struct ast_t* a = nil;
  (void)token_next(ts);
  if (expect_str(t->name, "define")) {
    // process_define_context(r, ts);
  } else if (expect_str(t->name, "include")) {
    a = process_include_context(ts);
  }

  return a;
}

static struct ast_t* create_ast(const struct token_t* t,
                                struct token_t** ts) {
  log_token((struct token_t*)t);

  printf("type: %c\n", (char)t->type);

  if (t->type == '#') {
    (void)token_next(ts);
    return processor_context(ts);
  } else {
    (void)token_next(ts);
  }
  return nil;
}

void log_ast(struct ast_t* a) {
  switch(a->type) {
  case INCLUDE_DECL: {
    printf("(include %s)\n", (char*)a->object);
  }
  }
}

void print_ast(cell_t root) {
    if (!is_tagged(root)) {
    printf("empty\n");
    return;
  }

  cell_t w = 0;
  for (w = root; // (a (b NIL))
       w && is_tagged(w) && !nullp(w) && nullp(get_cdr(w));
       w = get_cdr(w)) {
    log_ast((struct ast_t*)get_car(w));
  }
}

void parser(struct token_t* ts, const char* filename) {
  struct token_t* tl = ts;
  struct token_t* t = nil;
  cell_t root = sexp();
  
  root = set_car(root, strdup(filename));
  do {
    t = tl;
    printf("%p %p\n", t, tl);
    struct ast_t* ast = create_ast(t, &tl);
    push_item(root, (void*)ast);
  } while((t = token_next(&tl)) && t->type != 0);
  print_ast(root);
}
