#include "../debug.hpp"
struct data {
  int a, b, c;
  std::string d;
};
int main() {
  int a = 2;
  short b = 3;
  int& ref = a;
  long&& rref = 4l;
  data d{1, 2, 3, "hello"};
#ifndef SINGLE
  dbg(a, b);
  dbg(d);
  dbg(ref, rref);
#else
  dbg(a); dbg(b);
  dbg(d);
  dbg(ref); dbg(rref);
#endif
  return 0;
}