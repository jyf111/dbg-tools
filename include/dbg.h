#pragma once

#include <any>
#include <bitset>
#include <concepts>
#include <cstring>
#include <forward_list>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <optional>
#include <queue>
#include <set>
#include <source_location>
#include <sstream>
#include <stack>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <valarray>
#include <variant>
#include <vector>

namespace dbg {
namespace printer {
inline bool color_print(const std::ostream &os) {
  int fd = 0;
  if (&os == &std::cout)
    fd = fileno(stdout);
  else if (&os == &std::cerr || &os == &std::clog)
    fd = fileno(stderr);
  else
    return false;
  char *term = getenv("TERM");
  if (isatty(fd) && !(term && !strcmp(term, "dumb"))) {
    return true;
  }
  return false;
}
}  // namespace printer

namespace config {
struct Option {
  std::ostream *os = nullptr;  // defer the init
  size_t CONTAINER_LENGTH = 20;
  std::string LOCATION_COLOR = "\033[02m";    // gray
  std::string EXPRESSION_COLOR = "\033[36m";  // cyan
  std::string VALUE_COLOR = "\033[37m";       // white
  std::string MESSAGE_COLOR = "\033[32m";     // green
  std::string ERROR_COLOR = "\033[1;31m";     // bold red
  std::string TYPE_COLOR = "\033[32m";        // magenta
  const std::string RESET_COLOR = "\033[0m";  // reset
  bool colorized_out = printer::color_print(std::cerr);
};

inline Option &getter() {
  static Option global_opt;
  return global_opt;
}

inline std::ostream *&os() { return getter().os; }
inline std::ostream &get_stream() {
  if (auto stream = os(); stream != nullptr)
    return *stream;
  else
    return std::cerr;
}
inline size_t &container_length() { return getter().CONTAINER_LENGTH; }
inline std::string &location_color() { return getter().LOCATION_COLOR; }
inline std::string &expression_color() { return getter().EXPRESSION_COLOR; }
inline std::string &value_color() { return getter().VALUE_COLOR; }
inline std::string &message_color() { return getter().MESSAGE_COLOR; }
inline std::string &error_color() { return getter().ERROR_COLOR; }
inline std::string &type_color() { return getter().TYPE_COLOR; }
inline const std::string &reset_color() { return getter().RESET_COLOR; }
inline const std::string empty_color() { return ""; }
inline bool &colorized_out() { return getter().colorized_out; }

inline void set_stream(std::ostream &redirect) {
  os() = &redirect;
  colorized_out() = printer::color_print(redirect);
}
inline void set_container_length(size_t length) { container_length() = length; }
inline void set_location_color(const std::string &color) { location_color() = color; }
inline void set_expression_color(const std::string &color) { expression_color() = color; }
inline void set_value_color(const std::string &color) { value_color() = color; }
inline void set_message_color(const std::string &color) { message_color() = color; }
inline void set_error_color(const std::string &color) { error_color() = color; }
inline void set_type_color(const std::string &color) { type_color() = color; }
}  // namespace config

namespace printer {
#define DBG_COLOR_PRINT(type)                                                                \
  inline std::string type##_print(const std::string &s) {                                    \
    return config::colorized_out() ? config::type##_color() + s + config::reset_color() : s; \
  }
DBG_COLOR_PRINT(location);
DBG_COLOR_PRINT(expression);
DBG_COLOR_PRINT(value);
DBG_COLOR_PRINT(message);
DBG_COLOR_PRINT(error);
DBG_COLOR_PRINT(type);
#undef DBG_COLOR_PRINT
}  // namespace printer

template <size_t N, std::integral T>
struct Base {
  Base(T val) : val_(val) {}
  T val_;
};
template <std::integral T>
Base<2, T> bin(T value) {
  return Base<2, T>{ value };
}
template <std::integral T>
Base<8, T> oct(T value) {
  return Base<8, T>{ value };
}
template <std::integral T>
Base<16, T> hex(T value) {
  return Base<16, T>{ value };
}

template <typename T>
concept is_aggregate = std::is_aggregate_v<T>;
template <typename T>
concept is_enum = std::is_enum_v<T>;
template <typename T>
concept is_union = std::is_union_v<T>;
template <typename T>
concept is_iteratable = requires(const T &t) {
  std::begin(t);
  std::end(t);
  std::size(t);
};
template <typename T>
concept has_ostream_operator = requires(std::ostream &os, const T &t) {
  os << t;
};
template <typename T>
concept is_container = is_iteratable<T> && !std::same_as<std::decay_t<T>, std::string>;

// Ignore explicitly specified template arguments
template <int &...ExplicitArgumentBarrier, typename T>
requires(!std::is_enum_v<T> && !std::is_union_v<T>) std::string type_name(std::type_identity<T>) {
  std::string_view pretty_name(std::source_location::current().function_name());
  const auto L = pretty_name.find("T = ") + 4;
  const auto R = pretty_name.find_last_of(']');
  return std::string(pretty_name.substr(L, R - L));
}
template <is_enum Enum>
std::string type_name(std::type_identity<Enum>) {
  std::string_view pretty_name(std::source_location::current().function_name());
  const auto L = pretty_name.find("Enum = ") + 7;
  const auto R = pretty_name.find_last_of(']');
  return "enum " + std::string(pretty_name.substr(L, R - L)) + " : " +
         type_name(std::type_identity<std::underlying_type_t<Enum>>{});
}
template <is_union Union>
std::string type_name(std::type_identity<Union>) {
  std::string_view pretty_name(std::source_location::current().function_name());
  const auto L = pretty_name.find("Union = ") + 8;
  const auto R = pretty_name.find_last_of(']');
  return "union " + std::string(pretty_name.substr(L, R - L));
}

#define DBG_PRIMITIVE_TYPE_NAME(std_type, bit_type, bits)               \
  inline std::string type_name(std::type_identity<std_type>) {          \
    return sizeof(std_type) * CHAR_BIT == bits ? #bit_type : #std_type; \
  }
DBG_PRIMITIVE_TYPE_NAME(signed char, int8_t, 8);
DBG_PRIMITIVE_TYPE_NAME(unsigned char, uint8_t, 8);
DBG_PRIMITIVE_TYPE_NAME(short, int16_t, 16);
DBG_PRIMITIVE_TYPE_NAME(unsigned short, uint16_t, 16);
DBG_PRIMITIVE_TYPE_NAME(int, int32_t, 32);
DBG_PRIMITIVE_TYPE_NAME(unsigned int, uint32_t, 32);
DBG_PRIMITIVE_TYPE_NAME(long, int64_t, 64);
DBG_PRIMITIVE_TYPE_NAME(unsigned long, uint64_t, 64);
DBG_PRIMITIVE_TYPE_NAME(long long, int64_t, 64);
DBG_PRIMITIVE_TYPE_NAME(unsigned long long, uint64_t, 64);
#undef DBG_PRIMITIVE_TYPE_NAME

inline std::string type_name(std::type_identity<std::any>) { return "std::any"; }
inline std::string type_name(std::type_identity<std::string>) { return "std::string"; }
inline std::string type_name(std::type_identity<std::string_view>) { return "std::string_view"; }

template <typename T>
std::string get_type_name() {
  if (std::is_volatile_v<T>) {
    if (std::is_pointer_v<T>) {
      return get_type_name<std::remove_volatile_t<T>>() + " volatile";
    } else {
      return "volatile " + get_type_name<std::remove_volatile_t<T>>();
    }
  }
  if (std::is_const_v<T>) {
    if (std::is_pointer_v<T>) {
      return get_type_name<std::remove_const_t<T>>() + " const";
    } else {
      return "const " + get_type_name<std::remove_const_t<T>>();
    }
  }
  if (std::is_pointer_v<T>) {
    return get_type_name<std::remove_pointer_t<T>>() + " *";
  }
  if (std::is_lvalue_reference_v<T>) {
    return get_type_name<std::remove_reference_t<T>>() + " &";
  }
  if (std::is_rvalue_reference_v<T>) {
    return get_type_name<std::remove_reference_t<T>>() + " &&";
  }
  return type_name(std::type_identity<T>{});
}

#define DBG_TEMPLATE_TYPE_NAME_1(std_type, temp, get_temp_type_name) \
  template <temp>                                                    \
  inline std::string type_name(std::type_identity<std_type<T>>) {    \
    return #std_type "<" + get_temp_type_name + ">";                 \
  }
DBG_TEMPLATE_TYPE_NAME_1(std::vector, typename T, get_type_name<T>());
DBG_TEMPLATE_TYPE_NAME_1(std::valarray, typename T, get_type_name<T>());
DBG_TEMPLATE_TYPE_NAME_1(std::list, typename T, get_type_name<T>());
DBG_TEMPLATE_TYPE_NAME_1(std::forward_list, typename T, get_type_name<T>());
DBG_TEMPLATE_TYPE_NAME_1(std::initializer_list, typename T, get_type_name<T>());
DBG_TEMPLATE_TYPE_NAME_1(std::stack, typename T, get_type_name<T>());
DBG_TEMPLATE_TYPE_NAME_1(std::deque, typename T, get_type_name<T>());
DBG_TEMPLATE_TYPE_NAME_1(std::queue, typename T, get_type_name<T>());
DBG_TEMPLATE_TYPE_NAME_1(std::priority_queue, typename T, get_type_name<T>());
DBG_TEMPLATE_TYPE_NAME_1(std::set, typename T, get_type_name<T>());
DBG_TEMPLATE_TYPE_NAME_1(std::multiset, typename T, get_type_name<T>());
DBG_TEMPLATE_TYPE_NAME_1(std::unordered_set, typename T, get_type_name<T>());
DBG_TEMPLATE_TYPE_NAME_1(std::unordered_multiset, typename T, get_type_name<T>());
DBG_TEMPLATE_TYPE_NAME_1(std::optional, typename T, get_type_name<T>());
DBG_TEMPLATE_TYPE_NAME_1(std::bitset, size_t T, std::to_string(T));
#undef DBG_TEMPLATE_TYPE_NAME_1

#define DBG_TEMPLATE_TYPE_NAME_2(std_type, temp1, get_temp1_type_name, temp2, get_temp2_type_name) \
  template <temp1, temp2>                                                                          \
  inline std::string type_name(std::type_identity<std_type<T1, T2>>) {                             \
    return #std_type "<" + get_temp1_type_name + ", " + get_temp2_type_name + ">";                 \
  }
DBG_TEMPLATE_TYPE_NAME_2(std::array, typename T1, get_type_name<T1>(), size_t T2, std::to_string(T2));
DBG_TEMPLATE_TYPE_NAME_2(std::pair, typename T1, get_type_name<T1>(), typename T2, get_type_name<T2>());
DBG_TEMPLATE_TYPE_NAME_2(std::map, typename T1, get_type_name<T1>(), typename T2, get_type_name<T2>());
DBG_TEMPLATE_TYPE_NAME_2(std::multimap, typename T1, get_type_name<T1>(), typename T2, get_type_name<T2>());
DBG_TEMPLATE_TYPE_NAME_2(std::unordered_map, typename T1, get_type_name<T1>(), typename T2, get_type_name<T2>());
DBG_TEMPLATE_TYPE_NAME_2(std::unordered_multimap, typename T1, get_type_name<T1>(), typename T2, get_type_name<T2>());
#undef DBG_TEMPLATE_TYPE_NAME_2

template <typename... T>
std::string type_list_to_string() {
  std::string result = (... + (get_type_name<T>() + ", "));
  if constexpr (sizeof...(T) > 0) {
    result.pop_back();
    result.pop_back();
  }
  return result;
}
template <typename... T>
std::string type_name(std::type_identity<std::tuple<T...>>) {
  return "std::tuple<" + type_list_to_string<T...>() + ">";
}
template <typename... T>
std::string type_name(std::type_identity<std::variant<T...>>) {
  return "std::variant<" + type_list_to_string<T...>() + ">";
}

template <typename T>
std::string type_name(std::type_identity<std::type_identity<T>>) {
  return get_type_name<T>();
}

template <size_t N, std::integral T>
std::string type_name(std::type_identity<Base<N, T>>) {
  return get_type_name<T>();
}

namespace flatten {
struct any_constructor {
  template <typename T>
  constexpr operator T &() const &noexcept;
  template <typename T>
  constexpr operator T &&() const &&noexcept;
};
template <size_t I>
using indexed_any_constructor = any_constructor;

template <typename T, typename... U>
concept aggregate_initializable = is_aggregate<T> && requires {
  T{ { std::declval<U>() }... };
};
template <is_aggregate Aggregate, typename Indices>
struct aggregate_initializable_from_indices;
template <is_aggregate Aggregate, size_t... Indices>
struct aggregate_initializable_from_indices<Aggregate, std::index_sequence<Indices...>>
    : std::bool_constant<aggregate_initializable<Aggregate, indexed_any_constructor<Indices>...>> {};
template <typename T, size_t N>
concept aggregate_initializable_with_n_args =
    is_aggregate<T> && aggregate_initializable_from_indices<T, std::make_index_sequence<N>>::value;

template <is_aggregate T, size_t N, bool CanInitialize>
struct aggregate_field_count : aggregate_field_count<T, N + 1, aggregate_initializable_with_n_args<T, N + 1>> {};
template <is_aggregate T, size_t N>
struct aggregate_field_count<T, N, false> : std::integral_constant<size_t, N - 1> {};

template <is_aggregate T>
struct num_aggregate_fields : aggregate_field_count<T, 0, true> {};
template <is_aggregate T>
constexpr size_t num_aggregate_fields_v = num_aggregate_fields<T>::value;

// Begin generated code by `python utils/gen_flatten.py 16`
template <is_aggregate Aggregate>
constexpr auto flatten_impl(const Aggregate &t, std::integral_constant<size_t, 0> N1) noexcept {
  return std::forward_as_tuple();
}
template <is_aggregate Aggregate>
constexpr auto flatten_impl(const Aggregate &t, std::integral_constant<size_t, 1> N1) noexcept {
  auto &[f1] = t;
  return std::forward_as_tuple(f1);
}
template <is_aggregate Aggregate>
constexpr auto flatten_impl(const Aggregate &t, std::integral_constant<size_t, 2> N1) noexcept {
  auto &[f1, f2] = t;
  return std::forward_as_tuple(f1, f2);
}
template <is_aggregate Aggregate>
constexpr auto flatten_impl(const Aggregate &t, std::integral_constant<size_t, 3> N1) noexcept {
  auto &[f1, f2, f3] = t;
  return std::forward_as_tuple(f1, f2, f3);
}
template <is_aggregate Aggregate>
constexpr auto flatten_impl(const Aggregate &t, std::integral_constant<size_t, 4> N1) noexcept {
  auto &[f1, f2, f3, f4] = t;
  return std::forward_as_tuple(f1, f2, f3, f4);
}
template <is_aggregate Aggregate>
constexpr auto flatten_impl(const Aggregate &t, std::integral_constant<size_t, 5> N1) noexcept {
  auto &[f1, f2, f3, f4, f5] = t;
  return std::forward_as_tuple(f1, f2, f3, f4, f5);
}
template <is_aggregate Aggregate>
constexpr auto flatten_impl(const Aggregate &t, std::integral_constant<size_t, 6> N1) noexcept {
  auto &[f1, f2, f3, f4, f5, f6] = t;
  return std::forward_as_tuple(f1, f2, f3, f4, f5, f6);
}
template <is_aggregate Aggregate>
constexpr auto flatten_impl(const Aggregate &t, std::integral_constant<size_t, 7> N1) noexcept {
  auto &[f1, f2, f3, f4, f5, f6, f7] = t;
  return std::forward_as_tuple(f1, f2, f3, f4, f5, f6, f7);
}
template <is_aggregate Aggregate>
constexpr auto flatten_impl(const Aggregate &t, std::integral_constant<size_t, 8> N1) noexcept {
  auto &[f1, f2, f3, f4, f5, f6, f7, f8] = t;
  return std::forward_as_tuple(f1, f2, f3, f4, f5, f6, f7, f8);
}
template <is_aggregate Aggregate>
constexpr auto flatten_impl(const Aggregate &t, std::integral_constant<size_t, 9> N1) noexcept {
  auto &[f1, f2, f3, f4, f5, f6, f7, f8, f9] = t;
  return std::forward_as_tuple(f1, f2, f3, f4, f5, f6, f7, f8, f9);
}
template <is_aggregate Aggregate>
constexpr auto flatten_impl(const Aggregate &t, std::integral_constant<size_t, 10> N1) noexcept {
  auto &[f1, f2, f3, f4, f5, f6, f7, f8, f9, f10] = t;
  return std::forward_as_tuple(f1, f2, f3, f4, f5, f6, f7, f8, f9, f10);
}
template <is_aggregate Aggregate>
constexpr auto flatten_impl(const Aggregate &t, std::integral_constant<size_t, 11> N1) noexcept {
  auto &[f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11] = t;
  return std::forward_as_tuple(f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11);
}
template <is_aggregate Aggregate>
constexpr auto flatten_impl(const Aggregate &t, std::integral_constant<size_t, 12> N1) noexcept {
  auto &[f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12] = t;
  return std::forward_as_tuple(f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12);
}
template <is_aggregate Aggregate>
constexpr auto flatten_impl(const Aggregate &t, std::integral_constant<size_t, 13> N1) noexcept {
  auto &[f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13] = t;
  return std::forward_as_tuple(f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13);
}
template <is_aggregate Aggregate>
constexpr auto flatten_impl(const Aggregate &t, std::integral_constant<size_t, 14> N1) noexcept {
  auto &[f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14] = t;
  return std::forward_as_tuple(f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14);
}
template <is_aggregate Aggregate>
constexpr auto flatten_impl(const Aggregate &t, std::integral_constant<size_t, 15> N1) noexcept {
  auto &[f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15] = t;
  return std::forward_as_tuple(f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15);
}
template <is_aggregate Aggregate>
constexpr auto flatten_impl(const Aggregate &t, std::integral_constant<size_t, 16> N1) noexcept {
  auto &[f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16] = t;
  return std::forward_as_tuple(f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16);
}
template <is_aggregate Aggregate, size_t N>
constexpr auto flatten_impl(const Aggregate &t, std::integral_constant<size_t, N> N1) noexcept {
  config::get_stream() << printer::error_print("Please rerun utils/gen_flatten.py to generate more binds!");
  return std::forward_as_tuple();
}
// End generated code

template <is_aggregate Aggregate>
constexpr auto flatten_to_tuple(const Aggregate &t) noexcept {
  return flatten_impl(t, std::integral_constant<size_t, num_aggregate_fields_v<Aggregate>>());
}
}  // namespace flatten

namespace printer {
template <has_ostream_operator T>
void print_impl(std::ostream &os, const T &value) {
  os << value;
}
inline void print_impl(std::ostream &os, const char &value) {
  if (std::isprint(value)) {
    os << value;
  } else {
    std::ostringstream oss;
    oss << "\\x" << std::setw(2) << std::setfill('0') << std::hex << std::uppercase << (0xFF & value);
    os << oss.str();
  }
}

template <typename T>
void print(std::ostream &os, const T &value) {
  print_impl(os, value);
}

inline void print(std::ostream &os, const std::string &value) {
  os << '"';
  for (const auto &c : value) print_impl(os, c);
  os << '"';
}
inline void print(std::ostream &os, const std::string_view &value) { print(os, std::string(value)); }
inline void print(std::ostream &os, const char *const &value) { print(os, std::string(value)); }

inline void print(std::ostream &os, const char &value) {
  os << "'";
  print_impl(os, value);
  os << "'";
}

inline void print(std::ostream &os, const signed char &value) { os << +value; }
inline void print(std::ostream &os, const unsigned char &value) { os << +value; }

inline void print(std::ostream &os, const bool &value) { os << std::boolalpha << value << std::noboolalpha; }

inline void print(std::ostream &os, const std::nullptr_t &value) { os << "nullptr"; }

inline void print(std::ostream &os, const std::nullopt_t &value) { os << "nullopt"; }

template <size_t N, std::integral T>
void print(std::ostream &os, const Base<N, T> &value) {
  if constexpr (N == 2) {
    os << "0b" << std::bitset<sizeof(T) * CHAR_BIT>(value.val_);
  } else {
    std::ostringstream oss;
    if constexpr (N == 8)
      oss << std::oct << "0o";
    else if constexpr (N == 16)
      oss << std::hex << "0x";
    oss << std::setw(sizeof(T)) << std::setfill('0');
    oss << std::uppercase << +value.val_;
    os << oss.str();
  }
}

// Begin forward declaration of print
template <is_enum Enum>
void print(std::ostream &os, const Enum &value);
template <is_container Container>
void print(std::ostream &os, const Container &value);
template <is_aggregate Aggregate>
requires(!is_container<Aggregate>) void print(std::ostream &os, const Aggregate &value);
template <typename... Ts>
void print(std::ostream &os, const std::tuple<Ts...> &value);
template <>
inline void print(std::ostream &os, const std::tuple<> &);
template <typename T1, typename T2>
void print(std::ostream &os, const std::pair<T1, T2> &value);
template <typename T>
void print(std::ostream &os, T *const &value);
template <typename T>
void print(std::ostream &os, const std::optional<T> &value);
template <typename T, typename Deleter>
void print(std::ostream &os, const std::unique_ptr<T, Deleter> &value);
template <typename T>
void print(std::ostream &os, const std::shared_ptr<T> &value);
template <typename... Ts>
void print(std::ostream &os, const std::variant<Ts...> &value);
template <typename T, typename C>
void print(std::ostream &os, const std::stack<T, C> &value);
template <typename T, typename C>
void print(std::ostream &os, const std::queue<T, C> &value);
template <typename T, typename C, typename Cmp>
void print(std::ostream &os, const std::priority_queue<T, C, Cmp> &value);
// End forward declaration of print

template <is_enum Enum>
void print(std::ostream &os, const Enum &value) {
  print(os, static_cast<std::underlying_type_t<Enum>>(value));
}

template <is_container Container>
void print(std::ostream &os, const Container &value) {
  os << "[";
  const size_t size = std::size(value);
  const size_t n = std::min(config::container_length(), size);
  size_t i = 0;
  for (auto it = std::begin(value); it != std::end(value) && i < n; ++it, ++i) {
    print(os, *it);
    if (i != n - 1) {
      os << ", ";
    }
  }
  if (size > n) {
    os << ", ...";
    os << " size: " << size;
  }
  os << "]";
}

#if __has_builtin(__builtin_dump_struct) && __clang_major__ >= 15
template <is_aggregate Aggregate>
struct ReflectField {
  static constexpr size_t n_field = flatten::num_aggregate_fields_v<Aggregate>;
  inline static std::string fields[n_field];
  inline static size_t i_field = 0;
  static void constexpr_printf(const char *format, auto... args) {
    if constexpr (sizeof...(args) > 1) {
      if (i_field < n_field) {
        auto tuple = std::tuple{ args... };
        if (strlen(std::get<0>(tuple)) == 2) {
          fields[i_field++] = std::get<2>(tuple);
        }
      }
    }
  }

  inline static bool initialized = false;
  static void initialize(const Aggregate &value) {
    if (!initialized) {
      __builtin_dump_struct(&value, constexpr_printf);
      initialized = true;
    }
  }
};

template <is_aggregate Aggregate, size_t idx>
struct print_named_tuple {
  template <typename... Ts>
  void operator()(std::ostream &os, const std::tuple<Ts...> &tuple) {
    print_named_tuple<Aggregate, idx - 1>()(os, tuple);
    os << ", " << ReflectField<Aggregate>::fields[idx] + ": ";
    print(os, std::get<idx>(tuple));
  }
};
template <is_aggregate Aggregate>
struct print_named_tuple<Aggregate, 0> {
  template <typename... Ts>
  void operator()(std::ostream &os, const std::tuple<Ts...> &tuple) {
    os << ReflectField<Aggregate>::fields[0] + ": ";
    print(os, std::get<0>(tuple));
  }
};
#endif

template <is_aggregate Aggregate>
requires(!is_container<Aggregate>) void print(std::ostream &os, const Aggregate &value) {
  const auto &tuple = flatten::flatten_to_tuple(value);
#if __has_builtin(__builtin_dump_struct) && __clang_major__ >= 15
  ReflectField<Aggregate>::initialize(value);
  os << "{";
  constexpr size_t tuple_size = std::tuple_size_v<std::remove_cvref_t<decltype(tuple)>>;
  if constexpr (tuple_size > 0) {
    print_named_tuple<Aggregate, tuple_size - 1>()(os, tuple);
  }
  os << "}";
#else
  print(os, tuple);
#endif
}

template <size_t idx>
struct print_tuple {
  template <typename... Ts>
  void operator()(std::ostream &os, const std::tuple<Ts...> &tuple) {
    print_tuple<idx - 1>()(os, tuple);
    os << ", ";
    print(os, std::get<idx>(tuple));
  }
};
template <>
struct print_tuple<0> {
  template <typename... Ts>
  void operator()(std::ostream &os, const std::tuple<Ts...> &tuple) {
    print(os, std::get<0>(tuple));
  }
};
template <typename... Ts>
void print(std::ostream &os, const std::tuple<Ts...> &value) {
  os << "{";
  print_tuple<sizeof...(Ts) - 1>()(os, value);
  os << "}";
}
template <>
void print(std::ostream &os, const std::tuple<> &) {
  os << "{}";
}

template <typename T>
void print(std::ostream &os, T *const &value) {
  if (value == nullptr) {
    print(os, nullptr);
  } else {
    os << value;
  }
}

inline void print(std::ostream &os, char *const &value) {
  if (value == nullptr) {
    print(os, nullptr);
  } else {
    os << reinterpret_cast<int *>(value);
  }
}
inline void print(std::ostream &os, signed char *const &value) {
  if (value == nullptr) {
    print(os, nullptr);
  } else {
    os << reinterpret_cast<int *>(value);
  }
}
inline void print(std::ostream &os, unsigned char *const &value) {
  if (value == nullptr) {
    print(os, nullptr);
  } else {
    os << reinterpret_cast<int *>(value);
  }
}

template <typename T1, typename T2>
void print(std::ostream &os, const std::pair<T1, T2> &value) {
  os << "{";
  print(os, value.first);
  os << ", ";
  print(os, value.second);
  os << "}";
}

template <typename T>
void print(std::ostream &os, const std::optional<T> &value) {
  if (value) {
    os << '{';
    print(os, *value);
    os << '}';
  } else {
    os << "nullopt";
  }
}

template <typename T, typename Deleter>
void print(std::ostream &os, const std::unique_ptr<T, Deleter> &value) {
  print(os, value.get());
}

template <typename T>
void print(std::ostream &os, const std::shared_ptr<T> &value) {
  print(os, value.get());
  os << " (use_count = " << value.use_count() << ")";
}

template <typename... Ts>
void print(std::ostream &os, const std::variant<Ts...> &value) {
  os << "{";
  std::visit([&os](auto &&arg) { print(os, arg); }, value);
  os << "}";
}

template <typename T, typename C>
void print(std::ostream &os, const std::stack<T, C> &value) {
  auto stk = value;

  os << "[";
  const size_t size = std::size(value);
  const size_t n = std::min(config::container_length(), size);
  std::vector<T> elements;
  for (auto i = 0; i < n; i++) {
    elements.push_back(stk.top());
    stk.pop();
  }
  if (size > n) {
    os << "size: " << size;
    os << " ..., ";
  }
  for (auto i = 0; i < n; i++) {
    os << elements[n - 1 - i];
    if (i != n - 1) {
      os << ", ";
    }
  }
  os << "]";
}
template <typename T, typename C>
void print(std::ostream &os, const std::queue<T, C> &value) {
  auto que = value;

  os << "[";
  const size_t size = std::size(value);
  const size_t n = std::min(config::container_length(), size);
  for (auto i = 0; i < n; i++) {
    print(os, que.front());
    que.pop();
    if (i != n - 1) {
      os << ", ";
    }
  }
  if (size > n) {
    os << ", ...";
    os << " size: " << size;
  }
  os << "]";
}
template <typename T, typename C, typename Cmp>
void print(std::ostream &os, const std::priority_queue<T, C, Cmp> &value) {
  auto pq = value;

  os << "[";
  const size_t size = std::size(value);
  const size_t n = std::min(config::container_length(), size);
  for (auto i = 0; i < n; i++) {
    print(os, pq.top());
    pq.pop();
    if (i != n - 1) {
      os << ", ";
    }
  }
  if (size > n) {
    os << ", ...";
    os << " size: " << size;
  }
  os << "]";
}
}  // namespace printer

template <typename... Ts>
struct last {
  using type = typename decltype((std::type_identity<Ts>{}, ...))::type;
};
template <typename... Ts>
using last_t = typename last<Ts...>::type;

class Debugger {
 public:
  Debugger(const char *file, int line, const char *func) : os_(config::get_stream()) {
    std::string file_name(file);
    if (file_name.length() > MAX_PATH) {
      file_name = ".." + file_name.substr(file_name.size() - MAX_PATH, MAX_PATH);
    }
    location_ = '[' + file_name + ':' + std::to_string(line) + " (" + func + ")]";
  }

  Debugger(const Debugger &) = delete;
  const Debugger &operator=(const Debugger &) = delete;

  void print() { os_ << (printer::location_print(location_) + '\n'); }
  template <typename... T>
  auto print(const std::initializer_list<std::string> &exprs, const std::initializer_list<std::string> &type_names,
             T &&...values) -> last_t<T...> {
    return print_impl(exprs.begin(), type_names.begin(), std::forward<T>(values)...);
  }

 private:
  template <typename T>
  T &&print_impl(const std::string *expr_iter, const std::string *type_name_iter, T &&value) {
    std::stringstream ss;
    ss << printer::location_print(location_) << ' ';
    ss << printer::expression_print(*expr_iter) << " = ";
    std::stringstream ss2;
    printer::print(ss2, value);
    ss << printer::value_print(ss2.str());
    ss << " (" << printer::type_print(*type_name_iter) << ")\n";
    os_ << ss.str();
    return std::forward<T>(value);
  }
  template <size_t N>
  auto print_impl(const std::string *, const std::string *, const char (&value)[N]) -> decltype(value) {
    os_ << (printer::location_print(location_) + ' ' + printer::message_print(value) + '\n');
    return value;
  }
  template <typename T>
  std::type_identity<T> &&print_impl(const std::string *, const std::string *type_name_iter,
                                     std::type_identity<T> &&value) {
    os_ << (printer::location_print(location_) + ' ' +
            printer::type_print(*type_name_iter + " [sizeof " + std::to_string(sizeof(T)) + "]") + '\n');
    return std::forward<std::type_identity<T>>(value);
  }
  template <typename T, typename... U>
  auto print_impl(const std::string *expr_iter, const std::string *type_name_iter, T &&value, U &&...rest)
      -> last_t<T, U...> {
    print_impl(expr_iter, type_name_iter, std::forward<T>(value));
    return print_impl(expr_iter + 1, type_name_iter + 1, std::forward<U>(rest)...);
  }

  std::ostream &os_;
  std::string location_;

  static constexpr size_t MAX_PATH = 20;
};
}  // namespace dbg

// Begin generated code by `python utils/gen_macro.py 16`
#define _DBG_CONCAT(x, y) x##y
#define _DBG_CONCAT_HELPER(x, y) _DBG_CONCAT(x, y)
#define _DBG_GET_NTH_ARG(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, N, ...) N
#define _DBG_COUNT_ARGS(...) _DBG_GET_NTH_ARG(__VA_ARGS__, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)
#define _DBG_APPLY_F0(f)
#define _DBG_APPLY_WITH_COMMA_F0(f)
#define _DBG_APPLY_F1(f, _1) f(_1)
#define _DBG_APPLY_WITH_COMMA_F1(f, _1) f(_1)
#define _DBG_APPLY_F2(f, _1, ...) f(_1) _DBG_APPLY_F1(f, __VA_ARGS__)
#define _DBG_APPLY_WITH_COMMA_F2(f, _1, ...) f(_1), _DBG_APPLY_WITH_COMMA_F1(f, __VA_ARGS__)
#define _DBG_APPLY_F3(f, _1, ...) f(_1) _DBG_APPLY_F2(f, __VA_ARGS__)
#define _DBG_APPLY_WITH_COMMA_F3(f, _1, ...) f(_1), _DBG_APPLY_WITH_COMMA_F2(f, __VA_ARGS__)
#define _DBG_APPLY_F4(f, _1, ...) f(_1) _DBG_APPLY_F3(f, __VA_ARGS__)
#define _DBG_APPLY_WITH_COMMA_F4(f, _1, ...) f(_1), _DBG_APPLY_WITH_COMMA_F3(f, __VA_ARGS__)
#define _DBG_APPLY_F5(f, _1, ...) f(_1) _DBG_APPLY_F4(f, __VA_ARGS__)
#define _DBG_APPLY_WITH_COMMA_F5(f, _1, ...) f(_1), _DBG_APPLY_WITH_COMMA_F4(f, __VA_ARGS__)
#define _DBG_APPLY_F6(f, _1, ...) f(_1) _DBG_APPLY_F5(f, __VA_ARGS__)
#define _DBG_APPLY_WITH_COMMA_F6(f, _1, ...) f(_1), _DBG_APPLY_WITH_COMMA_F5(f, __VA_ARGS__)
#define _DBG_APPLY_F7(f, _1, ...) f(_1) _DBG_APPLY_F6(f, __VA_ARGS__)
#define _DBG_APPLY_WITH_COMMA_F7(f, _1, ...) f(_1), _DBG_APPLY_WITH_COMMA_F6(f, __VA_ARGS__)
#define _DBG_APPLY_F8(f, _1, ...) f(_1) _DBG_APPLY_F7(f, __VA_ARGS__)
#define _DBG_APPLY_WITH_COMMA_F8(f, _1, ...) f(_1), _DBG_APPLY_WITH_COMMA_F7(f, __VA_ARGS__)
#define _DBG_APPLY_F9(f, _1, ...) f(_1) _DBG_APPLY_F8(f, __VA_ARGS__)
#define _DBG_APPLY_WITH_COMMA_F9(f, _1, ...) f(_1), _DBG_APPLY_WITH_COMMA_F8(f, __VA_ARGS__)
#define _DBG_APPLY_F10(f, _1, ...) f(_1) _DBG_APPLY_F9(f, __VA_ARGS__)
#define _DBG_APPLY_WITH_COMMA_F10(f, _1, ...) f(_1), _DBG_APPLY_WITH_COMMA_F9(f, __VA_ARGS__)
#define _DBG_APPLY_F11(f, _1, ...) f(_1) _DBG_APPLY_F10(f, __VA_ARGS__)
#define _DBG_APPLY_WITH_COMMA_F11(f, _1, ...) f(_1), _DBG_APPLY_WITH_COMMA_F10(f, __VA_ARGS__)
#define _DBG_APPLY_F12(f, _1, ...) f(_1) _DBG_APPLY_F11(f, __VA_ARGS__)
#define _DBG_APPLY_WITH_COMMA_F12(f, _1, ...) f(_1), _DBG_APPLY_WITH_COMMA_F11(f, __VA_ARGS__)
#define _DBG_APPLY_F13(f, _1, ...) f(_1) _DBG_APPLY_F12(f, __VA_ARGS__)
#define _DBG_APPLY_WITH_COMMA_F13(f, _1, ...) f(_1), _DBG_APPLY_WITH_COMMA_F12(f, __VA_ARGS__)
#define _DBG_APPLY_F14(f, _1, ...) f(_1) _DBG_APPLY_F13(f, __VA_ARGS__)
#define _DBG_APPLY_WITH_COMMA_F14(f, _1, ...) f(_1), _DBG_APPLY_WITH_COMMA_F13(f, __VA_ARGS__)
#define _DBG_APPLY_F15(f, _1, ...) f(_1) _DBG_APPLY_F14(f, __VA_ARGS__)
#define _DBG_APPLY_WITH_COMMA_F15(f, _1, ...) f(_1), _DBG_APPLY_WITH_COMMA_F14(f, __VA_ARGS__)
#define _DBG_APPLY_F16(f, _1, ...) f(_1) _DBG_APPLY_F15(f, __VA_ARGS__)
#define _DBG_APPLY_WITH_COMMA_F16(f, _1, ...) f(_1), _DBG_APPLY_WITH_COMMA_F15(f, __VA_ARGS__)
#define _DBG_APPLY_HELPER(f, ...) f(__VA_ARGS__)
#define _DBG_FOR_EACH(f, ...) \
  _DBG_APPLY_HELPER(_DBG_CONCAT_HELPER(_DBG_APPLY_F, _DBG_COUNT_ARGS(__VA_ARGS__)), f, __VA_ARGS__)
#define _DBG_FOR_EACH_WITH_COMMA(f, ...) \
  _DBG_APPLY_HELPER(_DBG_CONCAT_HELPER(_DBG_APPLY_WITH_COMMA_F, _DBG_COUNT_ARGS(__VA_ARGS__)), f, __VA_ARGS__)
// End generated code

#define _DBG_GET_EXPR(x) std::string(#x)
#define _DBG_GET_TYPE(x) dbg::get_type_name<decltype(x)>()

#define DBG(...)                                                                  \
  dbg::Debugger(__FILE__, __LINE__, __func__)                                     \
      .print(__VA_OPT__({ _DBG_FOR_EACH_WITH_COMMA(_DBG_GET_EXPR, __VA_ARGS__) }, \
                        { _DBG_FOR_EACH_WITH_COMMA(_DBG_GET_TYPE, __VA_ARGS__) }, __VA_ARGS__))

#define _DBG_PRINT_STRUCT_FIELD(field)   \
  {                                      \
    os << #field << ": ";                \
    print(os, value.field);              \
    if (++i_field < n_field) os << ", "; \
  }

#define DBG_REGISTER(struct_type, ...)                              \
  namespace dbg {                                                   \
  namespace printer {                                               \
  template <>                                                       \
  void print(std::ostream &os, const struct_type &value) {          \
    size_t i_field = 0, n_field = _DBG_COUNT_ARGS(__VA_ARGS__);     \
    os << "{";                                                      \
    __VA_OPT__(_DBG_FOR_EACH(_DBG_PRINT_STRUCT_FIELD, __VA_ARGS__)) \
    os << "}";                                                      \
  }                                                                 \
  }                                                                 \
  }
