#include <stdlib.h>

#include "cell.h"
#include "cx.h"

static struct mem_t cell_mem = {{0}};

typedef unsigned long cell_t;

struct _cell_t {
  cell_t car;
  cell_t cdr;
};

cell_t tag(cell_t s) {
  return S_TAG(s);
}

void* untag(cell_t s) {
  return (void*)S_UNTAG(s);
}

int is_tagged(cell_t s) {
  return (s & has_tag) > 0;
}

cell_t sexp(void) {
  struct mem_pos_t* m = (struct mem_pos_t*)cell_mem.buffer;
  struct _cell_t* c =
    (struct _cell_t*)(cell_mem.buffer +
                      sizeof(int) +
                      sizeof(struct _cell_t));
  m->p += sizeof(struct _cell_t);
  return tag((cell_t)c) | empty_cell_tag;
};

int nullp(cell_t s) {
  return (s & empty_cell_tag) == empty_cell_tag || s == 0;
}

cell_t set_car(cell_t s, void* e) {
  if (!is_tagged(s)) { return 0; }
  ((struct _cell_t*)untag(s))->car = (cell_t)e;
  return s & ~empty_cell_tag;
}

cell_t get_car(cell_t s) {
  if (!is_tagged(s)) { return 0; }
  struct _cell_t* cl = (struct _cell_t*)untag(s);
  return cl->car;
}

cell_t set_cdr(cell_t s, void* e) {
  if (!is_tagged(s)) { return 0; }
  ((struct _cell_t*)untag(s))->cdr = (cell_t)e;
  return s & ~empty_cell_tag;
}

cell_t get_cdr(cell_t s) {
  if (!is_tagged(s)) { return 0; }
  struct _cell_t* cl = (struct _cell_t*)untag(s);
  return cl->cdr;
}

cell_t push_item(cell_t s, void* e) {
  if (!is_tagged(s)) { return 0; }

  cell_t w = 0;
  for (w = s; // (a (b NIL))
       w && is_tagged(w) && !nullp(w) && nullp(get_cdr(w));
       w = get_cdr(w)) {
    printf("1 %p\n", (char*)w);
  }

  cell_t c = sexp();
  c = set_car(c, e);
  w = set_cdr(w, (void*)c);

  return w;
}
