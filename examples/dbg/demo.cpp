#include "dbg.h"

struct Data {
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

  struct dir {
    int d[4] = { 1, -1, 2, -2 };
  };
  dir direction;
};

struct ComplexData {
  ComplexData(int &x, double y) : x(x), y(y) {}
  int &x;
  double y;
};
DBG_REGISTER(ComplexData, x, y);

int main() {
  int a = 2;
  long long b = 7;
  int &ref = a;
  const long &&rref = 42l;
  DBG(a, b, ref, rref);
  DBG((std::vector<int>{ 1, 2, 3, 4, 5 }));
  Data d{ { 41.13, -73.70 }, { 480, 340, "https://foo/bar/baz.jpg", { Data::image::format::type::yuyv_422 } } };
  DBG(d);
  DBG(ComplexData(a, 2.));
  DBG("Notice here!");
  const int32_t A = 2;
  const int32_t B = DBG(3 * A) + 1;
  DBG(dbg::bin(A), dbg::hex(B));
  std::priority_queue<int, std::vector<int>, std::greater<int>> pq;
  pq.push(2);
  pq.push(1);
  DBG(pq);
  char c[2][3][4];
  DBG(std::type_identity<decltype(c)>());
  return 0;
}
