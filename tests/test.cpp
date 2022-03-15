#include <cassert>

#include "../debug.hpp"
enum gb_enum { bad, ok };
union gb_union {
  int x;
  short y;
};
struct gb_struct {
  int a;
  long b;
  double c;
  float d;
};
class gb_class_private {
  int a = 2, b = 3, c = 1;
};
class gb_class {
public:
  int a;
};
struct none {};
struct single {
  long one = 1;
};
namespace {
struct A {
  int a = 128, b = 0;
};
}  // namespace
namespace nps {
struct B {
  short c = -1;
};
}  // namespace nps
int main() {
  {
    dbg("test for timer");
    dbg::timer timer;
    timer.start("start for loop");
    int sum = 0;
    for (int i = 1; i < 100000000; i++) {
      sum += rand() % i;
    }
    timer.stop();
    timer.log("for loop");
    timer.restart("start for loop");
    for (int i = 1; i < 10000000; i++) {
      sum += rand() % i;
    }
    timer.stop();
    timer.log("for loop");
    dbg::timer::show();
  }
  {
    dbg("test for const char*");
    dbg("this is a message");
    dbg("attention!");
    dbg("");
  }
  {
    dbg("test for primitive types");
    int a = 1;
    int& b = a;
    int* c = &a;
    int* d = nullptr;
    bool e = true;
    const int* ee = nullptr;
    const int* eee = &a;
    bool boolean = false;
    char ch = 'Y';
    int8_t byte = 202;
    const char* str = "hello";
    const char str_arr[] = "world";  // equal to "world"
    short st = 0;
    long long yyy = -2;
    volatile long long z = st;
    const volatile float bbb = 1.034f;
    long long& lf = yyy;
    long zz = 12312321;
    long double ldb = 0.000012313;
    char invisible = '\x00';  // static_cast<char>(130);
    char& lchar = invisible;
    const char* invisible_str = "\x80\x81\x82\x83\x90";  // TODO ?
    dbg(a);
    dbg(b);
    dbg(c);
    dbg(d);
    dbg(e);
    dbg(ee);
    dbg(eee);
    dbg(boolean);
    dbg(ch);
    dbg(byte);
    dbg(str);
    dbg(str_arr);
    dbg(st);
    dbg(yyy);
    dbg(z);
    dbg(bbb);
    dbg(lf);
    dbg(zz);
    dbg(ldb);
    dbg(nullptr);
    dbg(1);
    dbg(1.0);
    dbg(2.0f);
    dbg('O');
    dbg(100l);
    dbg(128ll);
    dbg(invisible);
    dbg(lchar);
    dbg(invisible_str);
    dbg(true);
    std::string rvalue = "hello world";
    dbg(std::move(rvalue));
    dbg(sizeof(int));
    dbg(3 + 1.0);
    std::string strstr = "str";
    const char* ccccc = "meme";
    dbg(strstr);
    dbg(ccccc);
  }
  {
    dbg("test for container");
    std::vector<int> vec{1, 2, 3};
    dbg(vec);
    const std::vector<int> cvec{1, 2, 3};
    dbg(cvec);
    std::vector<long> empty_vec{};
    dbg(empty_vec);
    std::vector<char> vec_chars{'h', 'e', 'l', 'l', 'o', '\x00', '\xFE'};
    dbg(vec_chars);
    dbg(std::move(std::vector<int>{0, 1, 0, 1}));
    const std::array<int, 2> dummy_array{{0, 4}};
    dbg(dummy_array);
    const std::list<int> dummy_list{1, 2, 3, 4, 5, 6, 7, 8, 9};
    dbg(dummy_list);
    std::vector<std::vector<int>> vec_of_vec_of_ints{{1, 2}, {3, 4, 5}};
    dbg(vec_of_vec_of_ints);
    std::vector<std::vector<std::vector<int>>> vec_of_vec_of_vec_of_ints{{{1, 2}, {3, 4, 5}},
                                                                         {{3}}};
    dbg(vec_of_vec_of_vec_of_ints);
    int dummy_int_array[] = {11, 22, 33};
    dbg(dummy_int_array);
    int longarr[] = {1, 0, -9, 5, 6, 8, 10, 12, 3, 4, 5, 11, 999};
    dbg(longarr);
    int multi_demension[4][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
    dbg(multi_demension);
  }
  {
#ifndef SINGLE
    int a = 1, b = 2;
    short c = 30;
    char d = 'O';
    double e = 10.f;
    dbg("test for multiple variables");
    dbg();  // empty -> split line
    dbg(a, (b), (c), d, e);
    int tmparr[] = {1, 2, 3};
    std::initializer_list tmplist = {0, 9, -1, 2};
    dbg(tmparr, tmplist);
    // dbg({1, 2, 3}, {0, 9, -1, 2}); // ! NOTICE
    dbg("first", a, "second", b, "third", c);
#endif
  }
  {
    dbg("test for special STL container");
    dbg(__cplusplus);
    std::pair<int, long> P{1, 2l};
    dbg(P);
    std::tuple<int, char, int8_t, char> tp4s{109, '\x23', 126, '\x0'};
    dbg(tp4s);
    std::queue<short> q;
    q.push(1);
    q.push(20);
    dbg(q);
    std::array<unsigned long long, 4> arr{1, 10, 99, 87};
    dbg(arr);
    std::string_view sv("123Yio");
    dbg(sv);
    std::deque<long> dq;
    dq.push_front(1);
    dq.push_back(-2);
    dbg(dq);
    std::stack<uint64_t> stk;
    stk.push(0);
    stk.push(999);
    dbg(stk);
    dbg(std::string_view{"test"});
    dbg(std::make_optional<bool>(false));
  }
  {
    dbg("test for union");
    gb_union gb;
    gb.x = 2;
    dbg(gb.x);
    union data {
      int n;
      char ch;
      double f;
    } ud;
    ud.f = 1.23;
    dbg(ud.f);
    // dbg(ud); // ! cann't know the value
    dbg(dbgtype(ud));
  }
  {
    dbg("test for enum");
    dbg(bad);
    dbg(ok);
    gb_enum g = bad;
    dbg(g);
    enum class color : char { RED = 1, BLUE = 2 };
    color c = color::RED;
    dbg(c);
    dbg(color::RED);
    dbg(color::BLUE);
  }
  {
    dbg("test for aggregate type");
    struct POD {
      int a, b;
    };
    POD pod{1, 2};
    dbg(pod.a);
    dbg(pod.b);
    dbg(pod);
    gb_struct gs{1, 2, 3, 4};
    dbg(gs);
    gb_class_private gcp;
    dbg(gcp);
    dbg(std::is_aggregate_v<gb_class_private>);
    gb_class gc{166};
    dbg(gc);
    dbg(std::is_aggregate_v<gb_class>);
    none no;
    dbg(no);
    single sg;
    dbg(sg);
    dbg(A());
    dbg(nps::B());
    struct complex_data {
      std::vector<int> vec;
      struct inner {
        int integer;
        double decimal;
      } in;
      int* b;
      const char* cs;
      char ch;
      std::string name;
    };
    int tmp = 6;
    complex_data cd = {{1, 2, 3}, {123, 2.0}, &tmp, "complex", 'P', "Alex"};
    dbg(cd);
  }
  {
    dbg("test for base output");
    char value = 110;
    short neg = -12;
    dbg(dbg::hex(value));
    dbg(dbg::hex(neg));
    std::string str = "123";
    dbg(dbg::hex(str));
    dbg(dbg::bin(value));
    dbg(dbg::bin(neg));
    dbg(dbg::oct(value));
    dbg(dbg::oct(neg));
  }
  {
    dbg("extra test");
    int x = 1;
    dbg(++x);
    assert(x == 2);
    std::unique_ptr<int> uq_ptr = std::make_unique<int>(123);
    dbg(uq_ptr);
    std::shared_ptr<long> sh_ptr = std::make_shared<long>(29);
    dbg(sh_ptr);
    std::variant<int, float> v;
    v = 12;
    dbg(v);
    struct sp {
      int a[2] = {0, 1};
    };
    std::cout << std::is_same_v<dbg::flatten::constructible_nfields_margs_t<sp, 0, 2>, std::size_t> << '\n';
    //dbg(sp());
    std::map<std::string, int> mp;
    dbg(mp);
    std::set<std::string> st;
    dbg(st);
    std::unordered_map<std::string, int> ump;
    dbg(ump);
    std::bitset<12> bit(1209);
    dbg(bit);
  }
  return 0;
}
