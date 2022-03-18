// // /*
// //   Demonstration of type loophole.
// //   Part of the blog post:
// //   http://alexpolt.github.io/type-loophole.html
// // */

// // #include <string>
// // #include <type_traits>
// // #include <vector>
// // #include <iostream>
// // template <class T, std::size_t N> struct tag { // forward declaration of loophole(tag<T,N>) without a result type 
// // friend auto loophole(tag<T,N>);
// // };
// // template <class T, class FieldType, std::size_t N, bool B>
// // struct fn_def { // definition of loophole(tag<T,N>) with a result type
// // friend auto loophole(tag<T,N>) { return FieldType{}; }
// // };
// // template <class T, std::size_t N> struct loophole_ubiq { template<class U, std::size_t M> static std::size_t ins(...); template<class U, std::size_t M, std::size_t = sizeof(loophole(tag<T,M>{})) > static char ins(int); template<class U, std::size_t = sizeof(fn_def< T, U, N, sizeof(ins<U, N>(0)) == sizeof(char) >)> constexpr operator U&() const noexcept; };


// // template <typename T>
// // struct no{};

// // template <typename T, std::size_t N>
// // struct loophole_type
// //      // Instantiating loopholes:
// //     : no< decltype(T{ {}, loophole_ubiq<T, N>{} }, 0) >
// // {
// //     static constexpr bool val = std::is_class_v<decltype(loophole(tag<T, N>{}))>;
// // };
// // int main() {
// //   struct test { char c; int i; };
// //   std::cout << loophole_type<test, 0>::val;
// //   // sizeof(test{loophole_ubiq<test, 0>{} });
// // }
// #include <type_traits>
// #include <iostream>
// #include <utility>
// template<typename T, int N> struct tag{
//   friend auto loophole(tag<T,N>);
// };

// template<typename T, int N>
// struct loophole_t
// {
//     friend auto loophole(tag<T, N>) { return T{}; };
// };
// template <int N>
// struct detector
// {
//     template <typename T, int = sizeof(loophole_t<T, N>)>
//     operator T();
// };

// struct any_constructor {
//   std::size_t I;
//   // -Wreturn-type
//   template <typename T>
//   constexpr operator T&() const& noexcept {}
// };

// template <typename T, int N, std::size_t... I>
// constexpr auto get_field_type(std::index_sequence<I...>)
// {
//     sizeof(T{any_constructor{I}..., detector<sizeof...(I)>{}});
//     return loophole(tag<N>{});
// }

// struct test
// {
//   int a;
//   long long b;
//   double x;
//   int y;
// };
// struct newtest {
//   int x;
//   double xx;
// };
// int main(void)
// {
//   static_assert( std::is_same<int, decltype(get_field_type<test, 0>(std::make_index_sequence<0>{}))>::value, "xxx" );
//   static_assert( std::is_same<long long, decltype(get_field_type<test, 1>(std::make_index_sequence<1>{}))>::value, "xxx" );
//   static_assert( std::is_same<double, decltype(get_field_type<test, 2>(std::make_index_sequence<2>{}))>::value, "xxx" );
//   static_assert( std::is_same<int, decltype(get_field_type<test, 3>(std::make_index_sequence<3>{}))>::value, "xxx" );
//   static_assert( std::is_same<int, decltype(get_field_type<newtest, 0>(std::make_index_sequence<0>{}))>::value, "xxx" );
//   static_assert( std::is_same<double, decltype(get_field_type<newtest, 1>(std::make_index_sequence<1>{}))>::value, "xxx" );
//   return 0;
// }
#include <bits/stdc++.h>
using namespace std;
// template <class T, std::size_t N> struct tag { // forward declaration of loophole(tag<T,N>) without a result type
//   friend auto loophole(tag<T,N>);
// };
// template <class T, class FieldType, std::size_t N, bool B> struct fn_def { // definition of loophole(tag<T,N>) with a result type
//   friend auto loophole(tag<T,N>) { return FieldType{}; }
// };
// template <class T, std::size_t N> struct loophole_ubiq {
//   template<class U, std::size_t M> static std::size_t ins(...);
//   template<class U, std::size_t M, std::size_t = sizeof(loophole(tag<T,M>{})) >
//   static char ins(int);
//   template<class U, std::size_t = sizeof(fn_def< T, U, N, sizeof(ins<U, N>(0)) == sizeof(char) >)>
//   constexpr operator U&() const noexcept;
// };
struct noclass {
  template <typename T, typename = std::enable_if_t<!std::is_class_v<T>>>
  operator T() const {
    return T{};
  }
};
struct nothing { int data;};
struct tmp {
  int x;
  short y;
  nothing nott;
};
int main() {
  tmp{noclass{}, noclass{}, {}};
  // struct test { char c; int i; };
  // test t{loophole_ubiq<test, 0>{} };
  // static_assert( std::is_same< char, decltype( loophole(tag<test, 0>{}) ) >::value, "" );
}