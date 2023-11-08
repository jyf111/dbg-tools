#include <any>
#include <valarray>

#include "dbg.h"
struct data {
  int a, b, c;
  std::string d;
  int arr[3];
  struct dir {
    int d[4] = { 1, -1, 2, -2 };
  };
  dir dr;
};
int main() {
  int a = 2;
  short b = 3;
  long long x = 7;
  int &ref = a;
  long &&rref = 4l;
  data d{ 1, 2, 3, "hello", { 6, 0, 8 } };
  DBG(a, b, x);
  DBG(d);
  DBG(ref, rref);
  std::deque<int> dq{ 1, 2 };
  DBG(dq);
  DBG((std::vector<int>{ 1, 2 }));
  std::stack<int> stk;
  stk.push(1);
  DBG(stk);
  std::valarray<int> g = { 1, 2 };
  DBG(g);
  std::any aa = 1;
  DBG(aa);
  union gb_union {
    int x;
    short y;
  } gg;
  enum class color : char { RED = 1, BLUE = 2 };
  color c = color::RED;
  DBG(c);
  std::tuple<int, double> tp = { 1, 2. };
  DBG(tp);
  std::byte bb{ 1 };
  DBG(bb);
  uint8_t ttt = 1;
  DBG(ttt);
  DBG(true);
  return 0;
}
