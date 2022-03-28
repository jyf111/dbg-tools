#define CATCH_CONFIG_MAIN
#include "../include/catch.hpp"
#include "../debug.hpp"

#include <any>

TEST_CASE("Timer", "[timer]") {
  dbg::Timer::show();

  dbg::Timer timer;
  timer.start("start for loop");
  int sum = 0;
  for (int i = 1; i < 100000000; i++) {
    sum += rand() % i;
  }
  timer.stop();
  timer.log("for loop");

  SECTION("restart") {
    timer.restart("restart for loop");
    for (int i = 1; i < 10000000; i++) {
      sum += rand() % i;
    }
    timer.stop();
    timer.log("for loop");
  }
  SECTION("continue") {
    timer.start("start for loop");
    for (int i = 1; i < 10000000; i++) {
      sum += rand() % i;
    }
    timer.stop();
    timer.log("for loop");
  }

  dbg::Timer::show();
}

TEST_CASE("message") {
  SECTION("single message") {
    LOG("test for const char*");
    LOG("this is a message");
    LOG("attention!");
    LOG("");
  }

  SECTION("multi message") {
    LOG("1", " 2", " 3");
  }
}

TEST_CASE("primitive type", "[type]") {
  SECTION("literal") {
    LOG(1); LOG(1.0);
    LOG(2.0F); LOG('O');
    LOG(100L); LOG(128LL);
    LOG("str"); LOG(1 + 2);
    LOG(1 * 2.0); LOG('a' + 3);
    LOG(0U); LOG(true);
    LOG(sizeof(int)); LOG(0.0L);
    LOG(__cplusplus);
  }
  short i16 = 2;
  int i32 = 1;
  char i8 = 'T';
  signed char si8 = 12;
  uint16_t ui16 = 17;
  uint8_t ui8 = 0x8;
  long li = 9009;
  long long lli = 100000ll;
  uint64_t mx = std::numeric_limits<uint64_t>::max();
  bool boolean = false;
  float f32 = 1.0F;
  double f64 = 1.2;
  long double lf64 = 1.23L;
  SECTION("primitive type") {
    LOG(i16); LOG(i32);
    LOG(i8); LOG(si8);
    LOG(ui16); LOG(ui8);
    LOG(li); LOG(lli);
    LOG(mx); LOG(boolean);
    LOG(f32); LOG(f64); LOG(lf64);
    long zz = 12312321;
    long double ldb = 0.000012313;
    LOG(zz); LOG(ldb);
  }
  SECTION("left reference") {
    short& ri16 = i16;
    int& ri32 = i32;
    char& ri8 = i8;
    signed char& rsi8 = si8;
    uint16_t& rui16 = ui16;
    uint8_t& rui8 = ui8;
    long& rli = li;
    long long& rlli = lli;
    uint64_t& rmx = mx;
    bool& rboolean = boolean;
    LOG(ri16); LOG(ri32);
    LOG(ri8); LOG(rsi8);
    LOG(rui16); LOG(rui8);
    LOG(rli); LOG(rlli);
    LOG(rmx); LOG(rboolean);
  }
  SECTION("pointer") {
    short* pi16 = &i16;
    int* pi32 = &i32;
    char* pi8 = &i8;
    signed char* psi8 = &si8;
    uint16_t* pui16 = &ui16;
    uint8_t* pui8 = &ui8;
    long* pli = &li;
    long long* plli = &lli;
    uint64_t* pmx = &mx;
    bool* pboolean = &boolean;
    LOG(pi16); LOG(pi32);
    LOG(pi8); LOG(psi8);
    LOG(pui16); LOG(pui8);
    LOG(pli); LOG(plli);
    LOG(pmx); LOG(pboolean);
    int* null = nullptr;
    LOG(null); LOG(nullptr);
  }
  SECTION("right reference") {
    int&& i = 123;
    std::string&& str = "nonesence";
    short&& j = 1;
    LOG(i); LOG(str); LOG(j);
    std::string rvalue = "hello world";
    LOG(std::move(rvalue));
  }
  SECTION("unvisible char") {
    int8_t byte = 202;
    char invisible = '\x00';  // static_cast<char>(130);
    char& lchar = invisible;
    const char* invisible_str = "\x80\x81\x82\x83\x90";
    LOG(byte); LOG(invisible);
    LOG(lchar); LOG(invisible_str);
  }
  SECTION("const/volatile") {
    const int* ee = nullptr;
    const int* eee = &i32;
    const char* str = "hello";
    const char str_arr[] = "world";  // equal to "world"
    volatile long long z = -23;
    const volatile float bbb = 1.034F;
    LOG(ee); LOG(eee);
    LOG(str); LOG(str_arr);
    LOG(z); LOG(bbb);
  }
  SECTION("string") {
    std::string strstr = "str";
    const char* ccccc = "meme";
    LOG(strstr);
    LOG(ccccc);
    std::string rowstr = R"(\\...*\\)";
    LOG(rowstr);
  }
}

TEST_CASE("special STL container", "[STL][container]") {
  SECTION("pair") {
    std::pair<int, long> P{1, 2l};
    LOG(P);
  }
  SECTION("tuple") {
    std::tuple<int, char, int8_t, char> tp4s{109, '\x23', 126, '\x0'};
    LOG(tp4s);
  }
  SECTION("queue") {
    std::queue<short> q;
    q.push(1);
    q.push(20);
    LOG(q);
  }
  SECTION("array") {
    std::array<unsigned long long, 4> arr{1, 10, 99, 87};
    LOG(arr);
  }
  SECTION("string view") {
    std::string_view sv("123Yio");
    LOG(sv);
    LOG(std::string_view{"test"});
    LOG(std::string_view("tmp", 3));
  }
  SECTION("deque") {
    std::deque<long> dq;
    dq.push_front(1);
    dq.push_back(-2);
    LOG(dq);
  }
  SECTION("stack") {
    std::stack<uint64_t> stk;
    stk.push(0);
    stk.push(999);
    LOG(stk);
  }
  SECTION("optional") {
    LOG(std::make_optional<bool>(false));
    LOG(std::make_optional<std::string>("tmporal"));
    LOG(std::nullopt);
    std::optional<int> opt = std::nullopt;
    LOG(opt);
  }
  SECTION("variant") {
    std::variant<int, std::string> state;
    state = 12;
    LOG(state);
    state = "hello";
    LOG(state);
    std::variant<int, float> v;
    v = 12;
    LOG(v);
  }
  SECTION("any") {
    std::any wild = 10.10L;
    LOG(std::any_cast<long double>(wild));
  }
}

#ifndef SINGLE
TEST_CASE("variadic argument", "[variadic]") {
  int a = 1, b = 2;
  short c = 30;
  char d = 'O';
  double e = 10.f;
  LOG();  // empty -> split line
  LOG(a, (b), (c), d, e);
  int tmparr[] = {1, 2, 3};
  std::initializer_list tmplist = {0, 9, -1, 2};
  LOG(tmparr, tmplist);
  //LOG({1, 2, 3}, {0, 9, -1, 2}); // ! NOTICE
  LOG((std::initializer_list{1, 2, 3}), (std::initializer_list{0, 9, -1, 2}));
  LOG("first:", a, "second:", b, "third:", c);
}
#endif

TEST_CASE("base output", "[base]") {
  char value = 110;
  short neg = -12;
  SECTION("hex") {
    LOG(dbg::hex(value));
    LOG(dbg::hex(neg));
    LOG(dbg::hex(-neg));
  }
  SECTION("bin") {
    LOG(dbg::bin(value));
    LOG(dbg::bin(neg));
    LOG(dbg::bin(-neg));
  }
  SECTION("oct") {
    LOG(dbg::oct(value));
    LOG(dbg::oct(neg));
  }
  SECTION("none integral") {
    std::string str = "123";
    LOG(dbg::hex(str));
    LOG(dbg::bin("12"));
    LOG(dbg::oct('3'));
  }
}

TEST_CASE("container", "[container]") {
  SECTION("vector") {
    std::vector<int> vec{1, 2, 3};
    LOG(vec);
    const std::vector<int> cvec{1, 2, 3};
    LOG(cvec);
    std::vector<long> empty_vec{};
    LOG(empty_vec);
    std::vector<char> vec_chars{'h', 'e', 'l', 'l', 'o', '\x00', '\xFE'};
    LOG(vec_chars);
    LOG(std::move(std::vector<int>{0, 1, 0, 1}));
    std::vector<std::vector<int>> vec_of_vec_of_ints{{1, 2}, {3, 4, 5}};
    LOG(vec_of_vec_of_ints);
    std::vector<std::vector<std::vector<int>>> vec_of_vec_of_vec_of_ints{{{1, 2}, {3, 4, 5}},
                                                                          {{3}}};
    LOG(vec_of_vec_of_vec_of_ints);
  }
  SECTION("array") {
    const std::array<int, 2> dummy_array{{0, 4}};
    LOG(dummy_array);
    int dummy_int_array[] = {11, 22, 33};
    LOG(dummy_int_array);
    int longarr[] = {1, 0, -9, 5, 6, 8, 10, 12, 3, 4, 5, 11, 999};
    LOG(longarr);
    int multi_demension[4][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
    LOG(multi_demension);
  }
  SECTION("list") {
    const std::list<int> dummy_list{1, 2, 3, 4, 5, 6, 7, 8, 9};
    LOG(dummy_list);
  }
  SECTION("initializer_list") {
    LOG((std::initializer_list{"10", "20", "66"}));
    std::string a = "12", b = "22";
    LOG((std::initializer_list<std::string>{a, b, "32"}));
  }
  SECTION("set") {
    std::set<int> st{0, 1, 2, 3, 4, 5, 6};
    LOG(st);
    std::set<std::string> st2;
    LOG(st2);
  }
  SECTION("multiset") {
    std::multiset<int64_t> mst{0L, 1L, -1L, -1L, 0L, 5L, 5L};
    LOG(mst);
  }
  SECTION("unordered_set") {
    std::unordered_set<int> ust{0, 1, 2, 3, 4, 5, 6};
    LOG(ust);
    std::unordered_set<std::string> ust2;
    LOG(ust2);
  }
  SECTION("map") {
    std::map<int, std::string> index{{0, "00"}, {1, "11"}, {2, "22"}};
    LOG(index);
    std::map<std::string, int> mp;
    LOG(mp);
  }
  SECTION("multimap") {
    std::multimap<int, std::string> multi_index{{0, "00"}, {0, "0"}, {1, "1"}, {1, "11"}, {2, "22"}};
    LOG(multi_index);
  }
  SECTION("unordered_map") {
    std::unordered_map<std::string, int> ump;
    LOG(ump);
  }
}

union gb_union {
  int x;
  short y;
};
TEST_CASE("union") {
  gb_union gb;
  gb.x = 2;
  LOG(TYPE(gb));
  LOG(gb.x);
  union data {
    int n;
    char ch;
    double f;
  } ud;
  ud.f = 1.23;
  LOG(ud.f);
  // dbg(ud); // ! cann't know the value
  LOG(TYPE(ud));
}

enum gb_enum { bad, ok };
TEST_CASE("enum") {
  LOG(bad);
  LOG(ok);
  gb_enum g = bad;
  LOG(g);
  enum class color : char { RED = 1, BLUE = 2 };
  color c = color::RED;
  LOG(c);
  LOG(color::RED);
  LOG(color::BLUE);
}


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
TEST_CASE("aggregate") {
  struct POD {
    int a, b;
  };
  POD pod{1, 2};
  LOG(pod.a); LOG(pod.b);
  LOG(pod);
  gb_struct gs{1, 2, 3, 4};
  LOG(gs);
  gb_class_private gcp;
  LOG(gcp);
  LOG(std::is_aggregate_v<gb_class_private>);
  gb_class gc{166};
  LOG(gc);
  LOG(std::is_aggregate_v<gb_class>);
  none no;
  LOG(no);
  single sg;
  LOG(sg);
  LOG(A());
  LOG(nps::B());
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
  LOG(dbg::flatten::unique_counter_impl<complex_data>());
  int tmp = 6;
  complex_data cd = {{1, 2, 3}, {6, 6.6}, &tmp, "complex", 'P', "Alex"};
  LOG(cd);
}

TEST_CASE("miscellaneous") {
  int x = 1;
  LOG(++x);
  REQUIRE(x == 2);

  std::unique_ptr<int> uq_ptr = std::make_unique<int>(123);
  LOG(uq_ptr);
  std::shared_ptr<long> sh_ptr = std::make_shared<long>(29);
  LOG(sh_ptr);
  struct sp {
    int a[2] = {0, 1};
    int b = 6;
    int c[4] = {-1, 1, 2, -2};
  };
  sp sp_obj;
  LOG(sp_obj);
  int arr[2] = {-1, 1};
  int (&brr)[2] = arr;
  LOG(brr);
  std::bitset<12> bit(1209);
  LOG(bit);
  struct nest {
    struct inner {
      int a, b;
    };
    inner in;
    int c;
    struct tao {
      struct tao2 {
        char d = 'A';
        int data[4] = {0, 9, 8, 9};
      };
      tao2 tmp;
      int tag = -1;
    };
    tao t;
  };
  nest nst{{1, 2}, 5};
  LOG(nst);
  struct big {
    int a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q;
  };
  LOG(big());
  struct outer {
    struct inner {
      int a, b;
    } in;
    int c;
  };
  outer o = {{1, 2}, 3};
  LOG(o);
}
