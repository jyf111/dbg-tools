#include "gdb.h"

void g() { BREAKPOINT(); }

void f() {
  g();
  BREAKPOINT();
}

int main() {
  f();
  return 0;
}
