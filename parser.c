#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "cell.h"
#include "cx.h"
#include "token.h"

enum decl_t { EMPTY_DECL,
              INCLUDE_DECL,
              DEFINE_DECL,
              DEFINE_FUNCT_DECL,
              TYPEDEF_DECL,
              CONTINUE_DECL };

struct typedef_t {
  char* of;
  char* alias;
};

struct define_t {
  char* name;
  cell_t content;
};

struct ast_t {
  int type;
  void* object;
};

void log_token(struct token_t* t);

static int expect_str(const char* str, const char* exp) {
  unsigned long l = strlen(exp);
  return
    strlen(str) == l &&
    strncmp(str, exp, l) == 0;
}

static int expect_type(int type, int exp) {
  return type == exp;
}

#define alloc_ast() (struct ast_t*)malloc(sizeof(struct ast_t))

static struct ast_t* process_continue(struct token_t** ts) {
  struct ast_t* a = alloc_ast();
  a->type = CONTINUE_DECL;
  struct token_t* t = token_next(ts);
  log_token(t);
  assert(t->type == ';');
  return a;
}

static struct ast_t* process_typedef(struct token_t** ts) {
  struct ast_t* a = alloc_ast();
  struct typedef_t* d =
    (struct typedef_t*)malloc(sizeof(struct typedef_t));

  struct token_t* t= nil;
  t = token_next(ts);
  d->of = strdup(t->name);
  t = token_next(ts);
  d->alias = strdup(t->name);
  // end
  t = token_next(ts);
  log_token(t);
  assert(t->type == ';');

  a->type = TYPEDEF_DECL;
  a->object = d;

  return a;
}

static struct ast_t* process_define_context(struct token_t** ts) {
  struct token_t* t = *ts;

  struct ast_t* a = alloc_ast();
  struct define_t* d =
    (struct define_t*)malloc(sizeof(struct define_t));

  assert(t->type == IDENTIFIER);
  d->name = strdup(t->name);

  t = token_next(ts);
  assert(t->type == IDENTIFIER ||
         t->type == '\\');
  a->type = (t->type != '(') ? DEFINE_DECL : DEFINE_FUNCT_DECL;

  cell_t content = sexp();

  do {
    if (expect_type(t->type, '\\')) {
      t = token_next(ts);
      if (!expect_type(t->type, '\n')) {
        perror("invalid new line preprocessor.\n");
        break;
      }
      t = token_next(ts);
    }

    if (expect_type(t->type, '\n')) { break; }
    content = push_item(content, t);
  } while ((t = token_next(ts)));

  d->content = content;
  a->object = d;

  return a;
}

static struct ast_t* process_include_context(struct token_t** ts) {
  struct token_t* t = *ts;

  static char filename[1024] = {0};
  int i = 0;
  int exp = t->type;
  assert(exp == IDENTIFIER ||
         exp == '"' || exp == '\'' || exp == '<');

  exp = exp == '<' ? '>' : exp;

  while ((t = token_next(ts)) &&
         t->type != exp) {
    assert(!expect_type(t->type, '\n'));
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

  struct ast_t* a = alloc_ast();
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
    return process_define_context(ts);
  } else if (expect_str(t->name, "include")) {
    return process_include_context(ts);
  }
  return a;
}

static struct ast_t* create_ast(const struct token_t* t,
                                struct token_t** ts) {
  if (expect_type(t->type, '#')) {
    (void)token_next(ts);
    return processor_context(ts);
  } else {
    switch (t->type) {
    /* case IF_K: { */
    /*   void; */
    /* } break; */
    /* case ELSE_K: { */
    /*   void; */
    /* } break; */
    /* case BREAK_K: { */
    /*   void; */
    /* } break; */
    /* case SWITCH_K: { */
    /*   void; */
    /* } break; */
    /* case CASE_K: { */
    /*   void; */
    /* } break; */
    /* case FOR_K: { */
    /*   void; */
    /* } break; */
    /* case GOTO_K: { */
    /*   void; */
    /* } break; */
    /* case UNION_K: { */
    /*   void; */
    /* } break; */
    /* case STATIC_k: { */
    /*   void; */
    /* } break; */
    case CONTINUE_K: {
      return process_continue(ts);
    } break;
    /* case EXTERN_K: { */
    /*   void; */
    /* } break; */
    /* case CONST_K: { */
    /*   void; */
    /* } break; */
    /* case SIZEOF_K: { */
    /*   void; */
    /* } break; */
    /* case WHILE_K: { */
    /*   void; */
    /* } break; */
    /* case DO_K: { */
    /*   void; */
    /* } break; */
    /* case VOID_K: { */
    /*   void; */
    /* } break; */
    /* case DEFAULT_K: { */
    /*   void; */
    /* } break; */
    case TYPEDEF_K: {
      return process_typedef(ts);
    } break;
    /* case ENUM_K: { */
    /*   void; */
    /* } break; */
    /* case STRUCT_K: { */
    /*   void; */
    /* } break; */
    /* case REGISTER_K: { */
    /*   void; */
    /* } break; */
    /* case VOLATILE_K: { */
    /*   void; */
    /* } break; */
    /* case   RETURN_K: { */
    /*   void; */
    /* } break; */
    default: {} break;
    }
  }
  return nil;
}

void log_ast(struct ast_t* a) {
  switch(a->type) {
  case INCLUDE_DECL: {
    printf("(include %s)\n", (char*)a->object);
  } break;
  case DEFINE_DECL: {
    struct define_t* d = (struct define_t*)a->object;
    printf("(define %s)\n", d->name);
  } break;
  case DEFINE_FUNCT_DECL: {
    printf("(define_func %s)\n", (char*)a->object);
  } break;
  case TYPEDEF_DECL: {
    struct typedef_t* d = (struct typedef_t*)a->object;
    printf("(typedef %s alias %s)\n", d->of, d->alias);
  }
  case CONTINUE_DECL: {
    printf("(continue)\n");
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

  // root = set_car(root, strdup(filename));

  do {
    t = tl;
    if (expect_type(t->type, '\n')) { continue; }
    struct ast_t* ast = create_ast(t, &tl);
    root = push_item(root, (void*)ast);
  } while((t = token_next(&tl)) && t->type != 0);

  print_ast(root);
}
