#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <limits.h>
#include <assert.h>

#include "cell.h"

void test_1(void) {
  printf("test_1\n");
  cell_t c = 0;
  c = tag(c);

  char* m = malloc(10);
  cell_t x = tag((cell_t)m);

  assert(is_tagged(c));

  printf("%p\n%p\n%ld, %ld, %ld\n",
         (void*)c,
         (void*)x,
         sizeof(void*),
         sizeof(unsigned long),
         sizeof(char));

  free((char*)untag(x));
}

void test_2(void) {
  printf("test_2\n");
  cell_t c = sexp();
  assert(is_tagged(c) && nullp(c));
}

void test_3(void) {
  printf("test_3\n");
  cell_t c = sexp();
  assert(is_tagged(c) && nullp(c));
  char* m = malloc(8);
  set_car(c, m);
  char* n = (char*)get_car(c);
  assert(m == n);
  free(m);
}

void test_4(void) {
  printf("test_4\n");
  cell_t c = sexp();
  assert(is_tagged(c) && nullp(c));
  char* m = malloc(8);
  set_cdr(c, m);
  char* n = (char*)get_cdr(c);
  assert(m == n);
  free(m);
}

void test_5(void) {
  printf("test_5\n");
  cell_t c = sexp();
  assert(is_tagged(c) && nullp(c));
  char* m = malloc(8);
  c = set_cdr(c, m);
  char* n = (char*)get_cdr(c);
  assert(m == n);
  free(m);
}

void test_6(void) {
  printf("test_6\n");
  cell_t c = sexp();
  assert(is_tagged(c) && nullp(c));
  char* m = malloc(8);
  c = set_car(c, m);
  assert(!nullp(c));
  char* n = (char*)get_car(c);
  assert(m == n);
  push_item(c, m);
  char* o = (char*)get_car(get_cdr(c));
  assert(m == o);
  free(m);
}

int main() {
  test_1();
  test_2();
  test_3();
  test_4();
  test_5();
  test_6();

  return 0;
}
