#include "debug.h"

int main() {
  int a = 1;
  int& b = a;
  int* c = &a;
  int* d = nullptr;
  bool e = true;
  const int* ee = nullptr;
  const int* eee = &a;
  std::nullptr_t f = nullptr;
  std::cout << d << '\n';
  std::vector<int> vec{1, 2 ,3};
  enum E {
    bad,
    ok
  };
  dbg(a, b, c, d, e, ee, eee, f, bad, ok);
  dbg(vec);
  return 0;
}
