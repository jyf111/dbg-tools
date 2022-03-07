#### build
```shell
mkdir build && cd build
cmake .. // 如果只有c++17，或者只需要单参数，cmake -DSINGLE=on ..
make
```
#### demo
```cpp
#include "debug.hpp"
struct data {
  int a, b, c;
  std::string d;
};
int main() {
  int a = 2;
  short b = 3;
  int& ref = a;
  long&& rref = 4l;
  data d{1, 2, 3, "hello"};
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
+ [x] [行号(函数名)]：表达式 = 值 (类型)
+ [x] 多参数时放在同一行，用逗号分隔
+ [x] 计时功能（timer 比如start、restart、show...）
+ [x] config the ostream
+ [x] config the output container length
+ [x] config expression output color
+ [x] POD easy output [magic-get]<https://www.youtube.com/watch?v=abdeAew3gmQ>(reflection)
+ [x] empty debug (split line)
+ [x] support for queue、stack...(string view、union、shared_ptr、uniqueptr、variant...)

#### TODO
1. 2进制、8进制、16进制输出整数
2. for_each field?(may be)
3. more test