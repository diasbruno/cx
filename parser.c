#include <string.h>
#include <assert.h>
#include "cell.h"
#include "cx.h"
#include "token.h"


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

static cell_t process_define_context(cell_t r,
                                     struct token_t** ts) {
  struct token_t* t = *ts;

  switch (t->type) {
  case IDENTIFIER: {
    printf("is label\n");
    push_item(r, t->name);
  } break;
  case'(': {
    printf("is function\n");
    push_item(r, t->name);
  } break;
  }

  return r;
}

static cell_t process_include_context(cell_t r,
                                      struct token_t** ts) {
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
    log_token(t);
  }

  return r;
}

static cell_t processor_context(struct token_t** ts) {
  // #include <stdio.h>
  // #define a 000
  // #define f(x) ...
  printf("pound context\n");
  cell_t r = sexp();
  struct token_t* t = *ts;
  log_token(t);
  (void)token_next(ts);
  if (expect_str(t->name, "define")) {
    printf("define\n");
    push_item(r, t->name);
    process_define_context(r, ts);
  } else if (expect_str(t->name, "include")) {
    printf("include\n");
    push_item(r, t->name);
    process_include_context(r, ts);
    printf("end include\n");
  }

  return r;
}

static cell_t create_ast(cell_t root, const struct token_t* t,
                         struct token_t** ts) {
  log_token((struct token_t*)t);

  printf("type: %c\n", (char)t->type);

  if (t->type == '#') {
    (void)token_next(ts);
    cell_t a = processor_context(ts);
    push_item(root, (void*)a);
  } else {
    (void)token_next(ts);
  }

  return root;
}

void parser(struct token_t* ts) {
  struct token_t* tl = ts;
  struct token_t* t = nil;
  cell_t root = sexp();
  do {
    t = tl;
    cell_t ast = create_ast(root, t, &tl);
    push_item(root, (void*)ast);
  } while((t = token_next(&tl)));
}
