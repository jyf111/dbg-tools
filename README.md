#### build
```shell
mkdir build && cd build
cmake .. # 如果只有c++17，或者只需要单参数，cmake -DSINGLE=on ..
make -j4
./test
# or
cmake -B build
cmake --build build --parallel 4
build/test
```
#### demo
```cpp
#include "debug.hpp"
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
  dbg(a, b);
  dbg(d);
  dbg(ref, rref);
#else
  dbg(a); dbg(b);
  dbg(d);
  dbg(ref); dbg(rref);
#endif
  return 0;
}
```
![demo](img/demo.png)
### features
+ [x] 完整且正确的类型名称（包括const、volatile、左值引用、右值引用、数组、指针...）
+ [x] 支持可变参数
+ [x] 计时功能（timer，支持start、restart、stop、log、show）
+ [x] 默认输出到std::cerr，可以重定向cerr到文件，或者通过dbg::config::set_stream(os)修改输出流
+ [x] 可以对输出的颜色进行配置
+ [x] 在多参数模式下，支持空参数(dbg())，依靠__VA_OPT__
+ [x] 支持各种容器的输出(STL、原生数组...)
+ [x] 聚合类型(is_aggregate_v\<T\>)可以直接输出各个成员(无需自定义operator<<)。原理就是借助SFINAE、聚合类型初始化以及结构化绑定。
+ [x] 2进制、8进制、16进制输出整数
+ [x] 对于不可打印字符(<0x20||>0x7f)，用\x的十六进制转移+蓝色背景颜色显示（包括字符串中的不可见字符）
#### TODO
1. use catch2 for unit test
2. for_each field?(may be)
3. more tests
4. may be combine with json

#### weakness
1. 为了显示传入的语句，并且获得未退化的类型，只能使用宏，而多参数需要借助FOR_EACH_MACRO，容易导致报错信息过多（与宏展开有关）。多参数宏为了解决空参数问题，使用了__VA_OPT__，所以至少需要-std=c++2a。可以通过-DSINGLE指明使用单参数，此时需要-std=c++17，但是不支持空参数。
2. 由于使用了宏，不支持直接传入初始化列表形式
```cpp
// not support!
dbg({1, 3, 4}); dbg(data{1, 2});
```
3. 只支持unix。windows的colorize接口不一样
4. 测试
5. 在自动序列化聚合类型时，会生成大量模板（正比于sizeof(T)），所以如果聚合类型的大小很大，可能会导致编译速度极慢
#### thanks for
+ [magic-get](https://www.youtube.com/watch?v=abdeAew3gmQ) <https://github.com/boostorg/pfr>
+ dbg-macro <https://github.com/sharkdp/dbg-macro>
+ <https://towardsdev.com/counting-the-number-of-fields-in-an-aggregate-in-c-20-c81aecfd725c>
#### test
![test](img/test.png)