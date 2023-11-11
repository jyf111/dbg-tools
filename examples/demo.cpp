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
struct Bad {
  int x, y;
};
DBG_REGISTER(Bad, x, y)
struct MyStruct {
  struct gps {
    double latitude;
    double longitude;
  };
  gps location;

  struct image {
    uint16_t width;
    uint16_t height;
    std::string url;

    struct format {
      enum class type { bayer_10bit, yuyv_422 };
      type type;
    };
    format format;
  };
  image thumbnail;
};

MyStruct s{ { 41.13, -73.70 }, { 480, 340, "https://foo/bar/baz.jpg", { MyStruct::image::format::type::yuyv_422 } } };
int main() {
  int a = 2;
  short b = 3;
  long long x = 7;
  int &ref = a;
  long &&rref = 4l;
  data d{ 1, 2, 3, "hello", { 6, 0, 8 } };
  DBG(a, b, x);
  DBG(ref, rref);
  DBG(d);
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
  DBG(dbg::type<char[2][3][4]>());
  DBG("This is a message");
  DBG(std::string("This is a string"));
  const char *msg = "MMMM";
  DBG(msg);
  const char *xxxx[] = { "111", "222", "333" };
  DBG(xxxx);
  int y = DBG(x) + 2;
  const int32_t A = 2;
  const int32_t B = DBG(3 * A) + 1;
  DBG(B);
  DBG(s);
  Bad bad = { 2, 5 };
  DBG(bad);
  DBG();
  return 0;
}
