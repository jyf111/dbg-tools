#include "gdb.h"

void g() { BREAKPOINT("bt"); }

void f() {
  g();
  BREAKPOINT("bt");
}

int main() {
  f();
  return 0;
}
