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
  std::vector<int> vec{1, 2 ,3};
  enum E {
    bad,
    ok
  };
  enum class F : std::size_t {
    oo = 2,
    pp
  };
  dbg(); // empty
  dbg(a, b, c, d, e, ee, eee, f, bad, ok, F::oo);
  dbg(vec);
  std::queue<short> q;
  q.push(1); q.push(20);
  dbg(q);
  std::array<long long, 4> arr{1, 10, 99, 87};
  dbg(arr);
  std::string_view sv("123Yio");
  dbg(sv);
  std::deque<long> dq;
  dq.push_front(1); dq.push_back(-2);
  dbg(dq);
  std::stack<uint64_t> stk;
  stk.push(0); stk.push(999);
  dbg(stk);
  return 0;
}
