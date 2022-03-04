#ifndef DEBUG_HELPER_
#define DEBUG_HELPER_

#include <string>
#include <vector>
#include <tuple>
#include <iostream>
#include <sstream>

namespace dbg {
template<typename T>
struct type {};

template <int&... ExplicitArgumentBarrier, typename T>
std::string type_name(type<T>) {
  auto pretty_function = static_cast<std::string>(__PRETTY_FUNCTION__);
  const auto L = pretty_function.find("T = ") + 4;
  const auto R = pretty_function.find_last_of(";");
  return pretty_function.substr(L, R-L);
}

template <typename T>
std::string get_type_name() {
  if (std::is_volatile<T>::value) {
    if (std::is_pointer<T>::value) {
      return get_type_name<typename std::remove_volatile<T>::type>() + " volatile";
    } else {
      return "volatile " + get_type_name<typename std::remove_volatile<T>::type>();
    }
  }
  if (std::is_const<T>::value) {
    if (std::is_pointer<T>::value) {
      return get_type_name<typename std::remove_const<T>::type>() + " const";
    } else {
      return "const " + get_type_name<typename std::remove_const<T>::type>();
    }
  }
  if (std::is_pointer<T>::value) {
    return get_type_name<typename std::remove_pointer<T>::type>() + "*";
  }
  if (std::is_lvalue_reference<T>::value) {
    return get_type_name<typename std::remove_reference<T>::type>() + "&";
  }
  if (std::is_rvalue_reference<T>::value) {
    return get_type_name<typename std::remove_reference<T>::type>() + "&&";
  }
  return type_name(type<T>());
}

inline std::string type_name(type<short>) {
  return "short"; //short int
}

inline std::string type_name(type<unsigned short>) {
  return "unsigned short"; //short unsigned int
}

inline std::string type_name(type<long>) {
  return "long"; //long int
}

inline std::string type_name(type<unsigned long>) {
  return "unsigned long"; // unsigned long int
}

inline std::string type_name(type<long long>) {
  return "long long"; // long long int
}

inline std::string type_name(type<unsigned long long>) {
  return "unsigned long long"; // unsigned long long int
}

inline std::string type_name(type<std::string>) {
  return "std::string"; // std::__cxx11::basic_string<char>
}

template <typename T>
std::string type_name(type<std::vector<T, std::allocator<T>>>) {
  return "std::vector<" + get_type_name<T>() + ">";
}

template <typename T1, typename T2>
std::string type_name(type<std::pair<T1, T2>>) {
  return "std::pair<" + get_type_name<T1>() + ", " + get_type_name<T2>() + ">";
}

template <typename... T>
std::string type_list_to_string() {
  std::string result;
  auto unused = {(result += get_type_name<T>() + ", ", 0)..., 0};
  static_cast<void>(unused);

  if constexpr (sizeof...(T) > 0) {
    result.pop_back();
    result.pop_back();
  }
  return result;
}

template <typename... T>
std::string type_name(type<std::tuple<T...>>) {
  return "std::tuple<" + type_list_to_string<T...>() + ">";
}

namespace detail {
template <class Default, class AlwaysVoid,
          template<class...> class Op, class... Args>
struct detector {
  using value_t = std::false_type;
  using type = Default;
};

template <class Default, template<class...> class Op, class... Args>
struct detector<Default, std::void_t<Op<Args...>>, Op, Args...> {
  using value_t = std::true_type;
  using type = Op<Args...>;
};

} // namespace detail

struct nonesuch {};

template <template<class...> class Op, class... Args>
using is_detected = typename detail::detector<nonesuch, void, Op, Args...>::value_t;

template <template<class...> class Op, class... Args>
constexpr bool is_detected_v = is_detected<Op, Args...>::value;

template <typename T>
using detect_begin_t = decltype(begin(std::declval<T>()));

template <typename T>
using detect_end_t = decltype(end(std::declval<T>()));

template <typename T>
using detect_size_t = decltype(size(std::declval<T>()));

template <typename T>
using detect_ostream_operator_t = decltype(std::declval<std::ostream&>() << std::declval<T>());

template <typename T>
constexpr bool is_container = std::conjunction_v<is_detected<detect_begin_t, T>,
                                                is_detected<detect_end_t, T>,
                                                is_detected<detect_size_t, T>,
                                                std::negation<std::is_same<std::string, std::decay_t<T>>>>;
template <typename T>
constexpr bool has_ostream_operator = is_detected_v<detect_ostream_operator_t, T>;

namespace printer {
template <typename T>
std::enable_if_t<has_ostream_operator<T>, void>
basic_print(std::ostream& os, const T& value) {
  os << value;
}

template <typename T>
std::enable_if_t<!has_ostream_operator<T>, void>
basic_print(std::ostream& os, const T&) {
  // TODO: do nothing
  os << "os not support!\n";
}

template <typename T>
std::enable_if_t<!is_container<const T&> && !std::is_enum_v<T>, void>
print(std::ostream& os, const T& value) {
  basic_print(os, value);
}

template <typename Container>
std::enable_if_t<is_container<const Container&>, void>
print(std::ostream& os, const Container& value) {
  os << "{";
  const size_t size = std::size(value);
  const size_t n = std::min(size_t{10}, size); // TODO: config
  size_t i = 0;
  for (auto it = begin(value); it != end(value) && i < n; ++it, ++i) {
    print(os, *it);
    if (i != n - 1) {
      os << ", ";
    }
  }

  if (size > n) {
    os << ", ...";
    os << " size:" << size;
  }

  os << "}";
}

template <typename Enum>
std::enable_if_t<std::is_enum<Enum>::value, void>
print(std::ostream& os, Enum const& value) {
  os << static_cast<std::underlying_type_t<Enum>>(value);
}

void print(std::ostream& os, const std::string& value) {
  os << '"' << value << '"';
}

void print(std::ostream& os, const char* const& value) {
  os << '"' << value << '"';
}

void print(std::ostream& os, const bool& value) {
  os << std::boolalpha << value << std::noboolalpha;
}

void print(std::ostream& os, const char& value) {
  os << "'" << value << "'";
}

void print(std::ostream& os, const std::nullptr_t& value) {
  os << "nullptr";
}

template <typename T>
void print(std::ostream& os, T* const& value) {
  if (value==nullptr) {
    print(os, nullptr);
  } else {
    os << value;
  }
}


template <typename T1, typename T2>
void print(std::ostream& os, const std::pair<T1, T2>& value) {
  os << "{";
  print(os, value.first);
  os << ", ";
  print(os, value.second);
  os << "}";
}

template <size_t idx>
struct print_tuple {
  template<typename... Ts>
  void operator() (std::ostream& os, const std::tuple<Ts...>& tuple) {
    print_tuple<idx-1>()(os, tuple);
    os << ", ";
    print(os, std::get<idx>(tuple));
  }
};

template<>
struct print_tuple<0> {
  template <typename... Ts>
  void operator() (std::ostream& os, const std::tuple<Ts...>& tuple) {
    print(os, std::get<0>(tuple));
  }
};

template <typename... Ts>
void print(std::ostream& os, const std::tuple<Ts...>& value) {
  os << "{";
  print_tuple<sizeof...(Ts) - 1>()(os, value);
  os << "}";
}

template <>
void print(std::ostream& os, const std::tuple<>&) {
  os << "{}";
}
} // namespace printer

class debugHelper {
public:
  debugHelper(const char* function_name, int line) {
    location = "[" + static_cast<std::string>(function_name) + "(" + std::to_string(line) + ")]";
  }
  static void clear() {
    exprs.clear();
    types.clear();
  }
  static void push_expr(std::string expr) {
    exprs.push_back(expr);
  }
  static void push_type(std::string type) {
    types.push_back(type);
  }
  template <typename... Ts>
  void print(std::tuple<Ts...> values) {
    int num = std::tuple_size<decltype(values)>::value;
    std::cout << location << ' ' << num << '\n';
    for (int i=0; i<num; i++) {
      std::cout << exprs[i] << ' ' << types[i] << '\n';
    }
    std::stringstream stream_value;
    printer::print(stream_value, values);
    std::cout << stream_value.str() << '\n';
  }
private:
  static std::vector<std::string> exprs, types;
  std::string location;
};
std::vector<std::string> debugHelper::exprs, debugHelper::types;
} // namespace dbg


#define PARENS ()
#define EXPAND(arg) EXPAND1(EXPAND1(EXPAND1(EXPAND1(arg))))
#define EXPAND1(arg) EXPAND2(EXPAND2(EXPAND2(EXPAND2(arg))))
#define EXPAND2(arg) EXPAND3(EXPAND3(EXPAND3(EXPAND3(arg))))
#define EXPAND3(arg) EXPAND4(EXPAND4(EXPAND4(EXPAND4(arg))))
#define EXPAND4(arg) arg
#define FOR_EACH(func, ...)                                    \
  __VA_OPT__(EXPAND(FOR_EACH_HELPER(func, __VA_ARGS__)))
#define FOR_EACH_HELPER(func, a1, ...)                         \
  func(a1)                                                     \
  __VA_OPT__(FOR_EACH_AGAIN PARENS (func, __VA_ARGS__))
#define FOR_EACH_AGAIN() FOR_EACH_HELPER

#define DBG_SAVE_EXPR(x) dbg::debugHelper::push_expr(static_cast<std::string>(#x));
#define DBG_SAVE_TYPE(x) dbg::debugHelper::push_type(dbg::get_type_name<decltype(x)>());
#define dbg(...) \
  dbg::debugHelper::clear(); \
  FOR_EACH(DBG_SAVE_EXPR, __VA_ARGS__) \
  FOR_EACH(DBG_SAVE_TYPE, __VA_ARGS__) \
  dbg::debugHelper(__func__, __LINE__).print(std::forward_as_tuple(__VA_ARGS__)) \

#endif