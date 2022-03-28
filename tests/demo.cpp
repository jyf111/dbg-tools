#include "../debug.hpp"
struct data {
  int a, b, c;
  std::string d;
  int arr[3];
  struct dir {
    int d[4] = {1, -1, 2, -2};
  };
  dir dr;
};
int main() {
  int a = 2;
  short b = 3;
  int& ref = a;
  long&& rref = 4l;
  data d{1, 2, 3, "hello", {6, 0, 8}};
#ifndef SINGLE
  LOG(a, b);
  LOG(d);
  LOG(ref, rref);
#else
  LOG(a); LOG(b);
  LOG(d);
  LOG(ref); LOG(rref);
#endif
  return 0;
}