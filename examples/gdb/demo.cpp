#include "gdb.h"

void g() { BREAKPOINT(gdb::GdbCommand::bt()); }

void f() {
  g();
  BREAKPOINT(gdb::GdbCommand::bt(), gdb::GdbCommand::mappings());
}

int main() {
  f();
  return 0;
}
