#ifndef __CX_CELL_H__
#define __CX_CELL_H__ 1

typedef unsigned long cell_t;

#define cell_tag ((long)0x8 << 60)
#define empty_cell_tag ((long)0x4 << 60)
#define has_tag ((long)0xf << 60)

#define S_TAG(x) ((long)x | cell_tag)
#define S_UNTAG(x) ((long)x & ~has_tag)

cell_t tag(cell_t s);
void* untag(cell_t s);
int is_tagged(cell_t s);

cell_t sexp(void);
int nullp(cell_t s);
cell_t set_car(cell_t s, void* e);
cell_t get_car(cell_t s);
cell_t set_cdr(cell_t s, void* e);
cell_t get_cdr(cell_t s);
cell_t push_item(cell_t s, void* e);

#endif // __CX_CELL_H__
