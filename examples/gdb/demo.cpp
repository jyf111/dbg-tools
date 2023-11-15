#include "gdb.h"

void g() { GDB(); }

void f() {
  g();
  int x = 1;
  GDB(gdb::GdbCommand::bt(), gdb::GdbCommand::locals());
}

int main() {
  f();
  return 0;
}
