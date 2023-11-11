#include <catch2/catch_test_macros.hpp>

#include "dbg.h"

TEST_CASE("message") {
  SECTION("single message") {
    DBG("test for const char*");
    DBG("this is a message");
    DBG("attention!");
    DBG("");
  }

  SECTION("multi message") {
    DBG("1");
    DBG("2");
    DBG("3");
  }
}

TEST_CASE("primitive type", "[type]") {
  SECTION("literal") {
    DBG(1);
    DBG(1.0);
    DBG(2.0F);
    DBG('O');
    DBG(100L);
    DBG(128LL);
    DBG("str");
    DBG(1 + 2);
    DBG(1 * 2.0);
    DBG('a' + 3);
    DBG(0U);
    DBG(true);
    DBG(sizeof(int));
    DBG(0.0L);
    DBG(__cplusplus);
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
    DBG(i16);
    DBG(i32);
    DBG(i8);
    DBG(si8);
    DBG(ui16);
    DBG(ui8);
    DBG(li);
    DBG(lli);
    DBG(mx);
    DBG(boolean);
    DBG(f32);
    DBG(f64);
    DBG(lf64);
    long zz = 12312321;
    long double ldb = 0.000012313;
    DBG(zz);
    DBG(ldb);
  }
  SECTION("left reference") {
    short &ri16 = i16;
    int &ri32 = i32;
    char &ri8 = i8;
    signed char &rsi8 = si8;
    uint16_t &rui16 = ui16;
    uint8_t &rui8 = ui8;
    long &rli = li;
    long long &rlli = lli;
    uint64_t &rmx = mx;
    bool &rboolean = boolean;
    DBG(ri16);
    DBG(ri32);
    DBG(ri8);
    DBG(rsi8);
    DBG(rui16);
    DBG(rui8);
    DBG(rli);
    DBG(rlli);
    DBG(rmx);
    DBG(rboolean);
  }
  SECTION("pointer") {
    short *pi16 = &i16;
    int *pi32 = &i32;
    char *pi8 = &i8;
    signed char *psi8 = &si8;
    uint16_t *pui16 = &ui16;
    uint8_t *pui8 = &ui8;
    long *pli = &li;
    long long *plli = &lli;
    uint64_t *pmx = &mx;
    bool *pboolean = &boolean;
    DBG(pi16);
    DBG(pi32);
    DBG(pi8);
    DBG(psi8);
    DBG(pui16);
    DBG(pui8);
    DBG(pli);
    DBG(plli);
    DBG(pmx);
    DBG(pboolean);
    int *null = nullptr;
    DBG(null);
    DBG(nullptr);
  }
  SECTION("right reference") {
    int &&i = 123;
    std::string &&str = "nonesence";
    short &&j = 1;
    DBG(i);
    DBG(str);
    DBG(j);
    std::string rvalue = "hello world";
    DBG(std::move(rvalue));
  }
  SECTION("unvisible char") {
    int8_t byte = 22;
    char invisible = '\x00';  // static_cast<char>(130);
    char &lchar = invisible;
    const char *invisible_str = "\x80\x81\x82\x83\x90";
    DBG(byte);
    DBG(invisible);
    DBG(lchar);
    DBG(invisible_str);
  }
  SECTION("const/volatile") {
    const int *ee = nullptr;
    const int *eee = &i32;
    const char *str = "hello";
    const char str_arr[] = "world";  // equal to "world"
    DBG(ee);
    DBG(eee);
    DBG(str);
    DBG(str_arr);
  }
  SECTION("string") {
    std::string strstr = "str";
    const char *ccccc = "meme";
    DBG(strstr);
    DBG(ccccc);
    std::string rowstr = R"(\\...*\\)";
    DBG(rowstr);
  }
}

TEST_CASE("special STL container", "[STL][container]") {
  SECTION("pair") {
    std::pair<int, long> P{ 1, 2l };
    DBG(P);
  }
  SECTION("tuple") {
    std::tuple<int, char, int8_t, char> tp4s{ 109, '\x23', 126, '\x0' };
    DBG(tp4s);
  }
  SECTION("queue") {
    std::queue<short> q;
    q.push(1);
    q.push(20);
    DBG(q);
  }
  SECTION("long queue") {
    std::queue<short> q;
    for (short i = 0; i < 25; i++) q.push(i);
    DBG(q);
  }
  SECTION("array") {
    std::array<unsigned long long, 4> arr{ 1, 10, 99, 87 };
    DBG(arr);
  }
  SECTION("string view") {
    std::string_view sv("123Yio");
    DBG(sv);
    DBG(std::string_view{ "test" });
    DBG(std::string_view("tmp", 3));
  }
  SECTION("deque") {
    std::deque<long> dq;
    dq.push_front(1);
    dq.push_back(-2);
    DBG(dq);
  }
  SECTION("stack") {
    std::stack<uint64_t> stk;
    stk.push(0);
    stk.push(999);
    DBG(stk);
  }
  SECTION("long stack") {
    std::stack<uint64_t> stk;
    for (uint64_t i = 0; i < 20; i++) stk.push(i);
    DBG(stk);
  }
  SECTION("priority queue") {
    std::priority_queue<int> pq;
    pq.push(30);
    pq.push(10);
    DBG(pq);
  }
  SECTION("optional") {
    DBG(std::make_optional<bool>(false));
    DBG(std::make_optional<std::string>("tmporal"));
    DBG(std::nullopt);
    std::optional<int> opt = std::nullopt;
    DBG(opt);
  }
  SECTION("variant") {
    std::variant<int, std::string> state;
    state = 12;
    DBG(state);
    state = "hello";
    DBG(state);
    std::variant<int, float> v;
    v = 12;
    DBG(v);
  }
  SECTION("any") {
    std::any wild = 10.10L;
    DBG(std::any_cast<long double>(wild));
  }
}

TEST_CASE("variadic argument", "[variadic]") {
  int a = 1, b = 2;
  short c = 30;
  char d = 'O';
  double e = 10.f;
  // DBG();
  DBG(a);
  DBG(b);
  DBG(c);
  DBG(d);
  DBG(e);
  int tmparr[] = { 1, 2, 3 };
  std::initializer_list<int> tmplist = { 0, 9, -1, 2 };
  DBG(tmparr);
  DBG(tmplist);
  // DBG({1, 2, 3}, {0, 9, -1, 2}); // ! NOTICE
  DBG((std::initializer_list<int>{ 1, 2, 3 }));
  DBG((std::initializer_list<int>{ 0, 9, -1, 2 }));
  DBG("first:");
  DBG(a);
  DBG("second:");
  DBG(b);
  DBG("third:");
  DBG(c);
}

TEST_CASE("base output", "[base]") {
  char value = 110;
  short neg = -12;
  SECTION("hex") {
    DBG(dbg::hex(value));
    DBG(dbg::hex(neg));
    DBG(dbg::hex(-neg));
  }
  SECTION("bin") {
    DBG(dbg::bin(value));
    DBG(dbg::bin(neg));
    DBG(dbg::bin(-neg));
  }
  SECTION("oct") {
    DBG(dbg::oct(value));
    DBG(dbg::oct(neg));
  }
}

TEST_CASE("container", "[container]") {
  SECTION("vector") {
    std::vector<int> vec{ 1, 2, 3 };
    DBG(vec);
    const std::vector<int> cvec{ 1, 2, 3 };
    DBG(cvec);
    std::vector<long> empty_vec{};
    DBG(empty_vec);
    std::vector<char> vec_chars{ 'h', 'e', 'l', 'l', 'o', '\x00', '\xFE' };
    DBG(vec_chars);
    DBG(std::move(std::vector<int>{ 0, 1, 0, 1 }));
    std::vector<std::vector<int>> vec_of_vec_of_ints{ { 1, 2 }, { 3, 4, 5 } };
    DBG(vec_of_vec_of_ints);
    std::vector<std::vector<std::vector<int>>> vec_of_vec_of_vec_of_ints{ { { 1, 2 }, { 3, 4, 5 } }, { { 3 } } };
    DBG(vec_of_vec_of_vec_of_ints);
  }
  SECTION("array") {
    const std::array<int, 2> dummy_array{ { 0, 4 } };
    DBG(dummy_array);
    int dummy_int_array[] = { 11, 22, 33 };
    DBG(dummy_int_array);
    int longarr[] = { 1, 0, -9, 5, 6, 8, 10, 12, 3, 4, 5, 11, 999 };
    DBG(longarr);
    int multi_demension[4][2] = { { 1, 0 }, { -1, 0 }, { 0, 1 }, { 0, -1 } };
    DBG(multi_demension);
  }
  SECTION("list") {
    const std::list<int> dummy_list{ 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    DBG(dummy_list);
  }
  SECTION("initializer_list") {
    DBG((std::initializer_list<std::string>{ "10", "20", "66" }));
    std::string a = "12", b = "22";
    DBG((std::initializer_list<std::string>{ a, b, "32" }));
  }
  SECTION("set") {
    std::set<int> st{ 0, 1, 2, 3, 4, 5, 6 };
    DBG(st);
    std::set<std::string> st2;
    DBG(st2);
  }
  SECTION("multiset") {
    std::multiset<int64_t> mst{ 0L, 1L, -1L, -1L, 0L, 5L, 5L };
    DBG(mst);
  }
  SECTION("unordered_set") {
    std::unordered_set<int> ust{ 0, 1, 2, 3, 4, 5, 6 };
    DBG(ust);
    std::unordered_set<std::string> ust2;
    DBG(ust2);
  }
  SECTION("map") {
    std::map<int, std::string> index{ { 0, "00" }, { 1, "11" }, { 2, "22" } };
    DBG(index);
    std::map<std::string, int> mp;
    DBG(mp);
  }
  SECTION("multimap") {
    std::multimap<int, std::string> multi_index{ { 0, "00" }, { 0, "0" }, { 1, "1" }, { 1, "11" }, { 2, "22" } };
    DBG(multi_index);
  }
  SECTION("unordered_map") {
    std::unordered_map<std::string, int> ump;
    DBG(ump);
  }
}

union gb_union {
  int x;
  short y;
};
TEST_CASE("union") {
  gb_union gb;
  gb.x = 2;
  DBG(std::type_identity<decltype(gb)>());
  DBG(gb.x);
  union data {
    int n;
    char ch;
    double f;
  } ud;
  ud.f = 1.23;
  DBG(ud.f);
  // dbg(ud); // ! cann't know the value
  DBG(std::type_identity<decltype(ud)>());
}

enum gb_enum { bad, ok };
TEST_CASE("enum") {
  DBG(bad);
  DBG(ok);
  gb_enum g = bad;
  DBG(g);
  enum class color : char { RED = 1, BLUE = 2 };
  color c = color::RED;
  DBG(c);
  DBG(color::RED);
  DBG(color::BLUE);
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
  POD pod{ 1, 2 };
  DBG(pod.a);
  DBG(pod.b);
  DBG(pod);
  gb_struct gs{ 1, 2, 3, 4 };
  DBG(gs);
  gb_class_private gcp;
  DBG(std::is_aggregate_v<gb_class_private>);
  gb_class gc{ 166 };
  DBG(gc);
  DBG(std::is_aggregate_v<gb_class>);
  none no;
  DBG(no);
  single sg;
  DBG(sg);
  DBG(A());
  DBG(nps::B());
  struct complex_data {
    std::vector<int> vec;
    struct inner {
      int integer;
      double decimal;
    } in;
    int *b;
    const char *cs;
    char ch;
    std::string name;
  };
  DBG(dbg::flatten::num_aggregate_fields_v<complex_data>);
  int tmp = 6;
  complex_data cd = { { 1, 2, 3 }, { 6, 6.6 }, &tmp, "complex", 'P', "Alex" };
  DBG(cd);
}

TEST_CASE("miscellaneous") {
  int x = 1;
  DBG(++x);
  REQUIRE(x == 2);

  std::unique_ptr<int> uq_ptr = std::make_unique<int>(123);
  DBG(uq_ptr);
  std::shared_ptr<long> sh_ptr = std::make_shared<long>(29);
  DBG(sh_ptr);
  struct sp {
    int a[2] = { 0, 1 };
    int b = 6;
    int c[4] = { -1, 1, 2, -2 };
  };
  sp sp_obj;
  DBG(sp_obj);
  int arr[2] = { -1, 1 };
  int(&brr)[2] = arr;
  DBG(brr);
  std::bitset<12> bit(1209);
  DBG(bit);
  struct nest {
    struct inner {
      int a, b;
    };
    inner in;
    int c;
    struct tao {
      struct tao2 {
        char d = 'A';
        int data[4] = { 0, 9, 8, 9 };
      };
      tao2 tmp;
      int tag = -1;
    };
    tao t;
  };
  nest nst{ { 1, 2 }, 5 };
  DBG(nst);
  struct big {
    int a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q;
  };
  DBG(big());
  struct outer {
    struct inner {
      int a, b;
    } in;
    int c;
  };
  outer o = { { 1, 2 }, 3 };
  DBG(o);
  struct O {
    struct i {
      int a, b;
    };
    int x;
  };
  O oo = { 1 };
  DBG(oo);
  struct Empty {};
  Empty E;
  DBG(E);
}
