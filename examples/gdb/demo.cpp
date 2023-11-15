#include "gdb.h"

void g() { BREAKPOINT(); }

void f() {
  g();
  int x = 1;
  BREAKPOINT(gdb::GdbCommand::bt(), gdb::GdbCommand::locals());
}

int main() {
  f();
  return 0;
}
