#include "../debug.h"

enum E {
  bad,
  ok
};
union gb {
  int x;
  short y;
};
struct element {
  int a;
  long b;
  double c;
  float d;
};
struct none {

};

struct single {
  long one = 1;
};
int main() {
  dbg("this is a message");
  int a = 1;
  int& b = a;
  int* c = &a;
  int* d = nullptr;
  bool e = true;
  const int* ee = nullptr;
  const int* eee = &a;
  std::nullptr_t f = nullptr;
  std::vector<int> vec{1, 2 ,3};
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
  std::array<unsigned long long, 4> arr{1, 10, 99, 87};
  dbg(arr);
  std::string_view sv("123Yio");
  dbg(sv);
  std::deque<long> dq;
  dq.push_front(1); dq.push_back(-2);
  dbg(dq);
  std::stack<uint64_t> stk;
  stk.push(0); stk.push(999);
  dbg(stk, "Hello !!", "Error.");
  dbg("Test", "test2");
  std::string str = "str";
  const char* ccc = "meme";
  dbg(str);
  dbg(ccc);
  struct POD {
    int a, b;
  };
  POD pod{1, 2};
  dbg(pod.a, pod.b);
  int longarr[] = {1, 0, -9, 5, 6, 8, 10, 12, 3, 4, 5, 11, 999};
  dbg(longarr);
  union data{
    int n;
    char ch;
    double f;
  } ud;
  ud.f = 1.23;
  //dbg(ud);
  dbg(ud.f);
  dbg(__cplusplus);
  dbg::timer::start();
  int sum = 0;
  for (int i=1; i<10000000; i++) {
    sum += rand()%i;
  }
  dbg::timer::stop();
  dbg::timer::log("for loop");
  dbg::timer::restart();
  for (int i=1; i<10000000; i++) {
    sum += rand()%i;
  }
  dbg::timer::stop();
  dbg::timer::log("for loop");
  dbg::timer::show();
  dbg(dbgtype(sum));
  short tmp = 0;
  volatile const short* place = &tmp;
  dbg(dbgtype(place));
  element ele{};
  gb gg;
  dbg("test dbgtype:", dbgtype(ele), dbgtype(ud), dbgtype(gg));
  dbg(pod);
  dbg(ele);
  single sg;
  none ne;
  dbg(sg, ne);
  std::cout << std::is_aggregate_v<const single&> << '\n';
  return 0;
}
