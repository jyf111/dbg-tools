[![Github Actions](https://github.com/jyf111/dbg-tools/actions/workflows/ci.yml/badge.svg)](https://github.com/jyf111/dbg-tools/actions/workflows/ci.yml)

# dbg-tools

dbg-tools is a header-only C++ library. It provides two macros,
`DBG(...)` and `GDB(...)`, to help quick and dirty debugging.

It was inspired by [dbg-macro](https://github.com/sharkdp/dbg-macro), [magic-get](https://github.com/boostorg/pfr) and
[debug-here](https://github.com/ethanpailes/debug-here).

## Examples

```cpp
// examples/dbg/demo.cpp
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
```

[![dbg-demo.png](https://z1.ax1x.com/2023/11/15/piYbPpD.png)](https://imgse.com/i/piYbPpD)

```cpp
// examples/gdb/demo.cpp
#include "gdb.h"

void g() { GDB(); }

void f() {
  g();
  int x = 1;
  GDB(gdb::GdbCommand::bt(), gdb::GdbCommand::locals());
}

int main() {
  f();
  return 0;
}
```

[![gdb-demo.png](https://z1.ax1x.com/2023/11/15/piYb24K.png)](https://imgse.com/i/piYb24K)

## Features

`DBG(...)` is a C++20 implementation of Rust's `dbg!` macro, and offers more.
It is intended to replace the cumbersome `printf("...", ...)` and `std::cout << ...`.

- It is a variadic macro, and also allows empty parameters
- In addition to printing the location, expression, and returning a reference to the value, it also prints the accurate type infomation of the value
- The output is colorized and should be easy to read
- It is specialized for various types, including containers, smart pointers, characters, strings, enums, unions, optionals, variants, monostate, thread::id, and hexadecimal, octal and binary format integers, etc
- It can directly print aggregate structs, and also provides anthor macro `DBG_REGISTER(...)` for non-aggregate structs
- Each line is squeezed into a single string and output using a `write` system call, ensuring atomicity for short outputs freely

`GDB(...)` is an C++ implementation of Rust's `debug_here!` macro and Python3's `breakpoint` function.
It enables programmatic breakpoints and automatically initiates a GDB instance attached to the relevant location.
It also wraps GDB’s batch mode, allowing direct execution of GDB commands such as `backtrace`, `info proc mappings`, etc.

## Quick start

```bash
$ git clone https://github.com/jyf111/dbg-tools --depth 1
$ sudo ln -s $(readlink -f include/) /usr/local/include/dbg_tools
$ cat main.cpp
#include <dbg_tools/dbg.h>
#include <dbg_tools/gdb.h>
int main() {
  int x = 1;
  GDB(gdb::GdbCommand::bt());
  return DBG(x) = 0;
}
$ g++ main.cpp -o main -std=c++20
$ ./main
...
#1  0x0000561e5b478936 in main ()
[Inferior 1 (process 12112) detached]
[main.cpp:6 (main)] x = 1 (int32_t)
```

## Build

```bash
cmake -B build -S .
cmake --build build
cmake --build build --target test
sudo cmake --build build --target install
```

## Known Issues

- To pass a expression with curly brackets to `DBG` macro (e.g., initializer_list), you need to wrap it with parentheses
- Frame #0 in the stacktrace output by `backtrace` in GDB is the `wait()` function introduced by this library. If you don't want to see it, you can load `utils/gdb_backtrace.py` (`-x utils/gdb_backtrace.py`), which provides a new command `rbt` that ignores frame #0
