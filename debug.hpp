#ifndef DEBUG_HELPER_DEBUG_HPP_
#define DEBUG_HELPER_DEBUG_HPP_

#ifdef SINGLE
static_assert(__cplusplus >= 201703L, "Sry, -std=c++17 should be supported");
#else
static_assert(__cplusplus > 201703L,
              "Sry, because of __VA_OPT__, -std=c++2a should be supported");
#endif

#if defined(WIN32)  // TODO
static_assert(false, "Sry, only support for unix");
#endif

#include <unistd.h>

#include <algorithm>
#include <bitset>
#include <chrono>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <optional>
#include <queue>
#include <set>
#include <sstream>
#include <stack>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

namespace dbg {
namespace helper {
inline FILE* get_standard_stream(const std::ostream& os) {
  if (&os == &std::cout)
    return stdout;
  else if ((&os == &std::cerr) || (&os == &std::clog))
    return stderr;
  return nullptr;
}

inline bool is_atty(const std::ostream& os) {
  FILE* std_stream = get_standard_stream(os);
  if (std_stream == nullptr)
    return false;
  else
    return ::isatty(fileno(std_stream));
}

inline bool should_color(const std::ostream& os) { return is_atty(os); }

inline std::string to_string(const int num, int width) {
  std::ostringstream os;
  os << std::setw(width) << std::setfill('0') << std::to_string(num);
  return os.str();
}
}  // namespace helper
namespace config {
struct option {
  std::ostream* os = nullptr;  // defer the init
  std::size_t CONTAINER_LENGTH{10};
  std::string LOCATION_COLOR = "\033[32m";      // bold green
  std::string EXPRESSION_COLOR = "\033[0;36m";  // cyan
  std::string VALUE_COLOR = "\033[37m";         // white
  std::string MESSAGE_COLOR = "\033[1;32m";     // bold green
  std::string ERROR_COLOR = "\033[1;31m";       // bold red
  std::string TYPE_COLOR = "\033[1;34m";        // bold blue
  std::string BACK_COLOR = "\033[44m";          // blue background
  const std::string RESET_COLOR = "\033[0m";    // reset
  const std::string EMPTY_COLOR = "";
  bool colorized_out = isatty(2);  // std::cerr
};

inline option& getter() {
  static option global_opt;
  return global_opt;
}

inline std::ostream*& get_os() { return getter().os; }
inline std::size_t& get_container_length() { return getter().CONTAINER_LENGTH; }
inline std::string& get_location_color() { return getter().LOCATION_COLOR; }
inline std::string& get_expression_color() { return getter().EXPRESSION_COLOR; }
inline std::string& get_value_color() { return getter().VALUE_COLOR; }
inline std::string& get_message_color() { return getter().MESSAGE_COLOR; }
inline std::string& get_error_color() { return getter().ERROR_COLOR; }
inline std::string& get_type_color() { return getter().TYPE_COLOR; }
inline std::string& get_back_color() { return getter().BACK_COLOR; }
inline const std::string get_reset_color() { return getter().RESET_COLOR; }
inline const std::string get_empty_color() { return getter().EMPTY_COLOR; }
inline bool& get_colorized_out() { return getter().colorized_out; }

inline void set_stream(std::ostream& redirect) {
  get_os() = &(redirect);
  get_colorized_out() = helper::is_atty(redirect);
}
inline void set_container_length(std::size_t length) {
  get_container_length() = length;
}
inline void set_location_color(const std::string& color) {
  get_location_color() = color;
}
inline void set_expression_color(const std::string& color) {
  get_expression_color() = color;
}
inline void set_value_color(const std::string& color) {
  get_value_color() = color;
}
inline void set_message_color(const std::string& color) {
  get_message_color() = color;
}
inline void set_error_color(const std::string& color) {
  get_error_color() = color;
}
inline void set_type_color(const std::string& color) {
  get_type_color() = color;
}
inline void set_back_color(const std::string& color) {
  get_back_color() = color;
}
}  // namespace config

namespace helper {
inline std::ostream& get_stream() {
  return (config::get_os() == nullptr ? std::cerr : *config::get_os());
}
}  // namespace helper

namespace printer {
inline std::string location_print(const std::string& s) {
  if (config::get_colorized_out())
    return config::get_location_color() + s + config::get_reset_color();
  else
    return s;
}
inline std::string expression_print(const std::string& s) {
  if (config::get_colorized_out())
    return config::get_expression_color() + s + config::get_reset_color();
  else
    return s;
}
inline std::string value_print(const std::string& s) {
  if (config::get_colorized_out()) {
    const auto pos = s.find(config::get_reset_color());
    if (pos == std::string::npos)
      return config::get_value_color() + s + config::get_reset_color();
    else
      return config::get_value_color() + s.substr(0, pos) +
             config::get_reset_color() + config::get_value_color() +
             value_print(s.substr(pos + config::get_reset_color().size()));
  } else {
    return s;
  }
}
inline std::string type_print(const std::string& s) {
  if (config::get_colorized_out())
    return config::get_type_color() + s + config::get_reset_color();
  else
    return s;
}
inline std::string message_print(const std::string& s) {
  if (config::get_colorized_out())
    return config::get_message_color() + s + config::get_reset_color();
  else
    return s;
}
inline std::string error_print(const std::string& s) {
  if (config::get_colorized_out())
    return config::get_error_color() + s + config::get_reset_color();
  else
    return s;
}
inline std::string invisible_print(const std::string& s) {
  if (config::get_colorized_out())
    return config::get_back_color() + s + config::get_reset_color();
  else
    return s;
}
} // namespace printer

template <typename T>
struct hex {
  hex(T _val) : val(_val) {}
  T val;
};

template <typename T>
struct oct {
  oct(T _val) : val(_val) {}
  T val;
};

template <typename T>
struct bin {
  bin(T _val) : val(_val) {}
  T val;
};

template <typename T>
struct type {};

template <int&... ExplicitArgumentBarrier, typename T>
std::enable_if_t<!std::is_enum_v<T> && !std::is_union_v<T>, std::string>
type_name(type<T>) {
  std::string_view pretty_function(__PRETTY_FUNCTION__);
  const auto L = pretty_function.find("T = ") + 4;
  const auto R = pretty_function.find_last_of(';');
  return std::string(pretty_function.substr(L, R - L));
}

inline std::string type_name(type<short>) {
  return "short";  // short int
}

inline std::string type_name(type<unsigned short>) {
  return "unsigned short";  // short unsigned int
}

inline std::string type_name(type<long>) {
  return "long";  // long int
}

inline std::string type_name(type<unsigned long>) {
  return "unsigned long";  // unsigned long int
}

inline std::string type_name(type<long long>) {
  return "long long";  // long long int
}

inline std::string type_name(type<unsigned long long>) {
  return "unsigned long long";  // unsigned long long int
}

inline std::string type_name(type<std::string>) {
  return "std::string";  // std::__cxx11::basic_string<char>
}

template <typename T>
std::string get_type_name() {
  if (std::is_volatile<T>::value) {
    if (std::is_pointer<T>::value) {
      return get_type_name<typename std::remove_volatile<T>::type>() +
             " volatile";
    } else {
      return "volatile " +
             get_type_name<typename std::remove_volatile<T>::type>();
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
  return type_name(type<T>());  // thanks for ADL
}

template <typename Enum>
std::enable_if_t<std::is_enum_v<Enum>, std::string> type_name(type<Enum>) {
  std::string_view pretty_function(__PRETTY_FUNCTION__);
  const auto L = pretty_function.find("Enum = ") + 7;
  const auto R = pretty_function.find_last_of(';');
  std::string name(pretty_function.substr(L, R - L));
  return "enum " + name + ": " + get_type_name<std::underlying_type_t<Enum>>();
}

template <typename Union>
std::enable_if_t<std::is_union_v<Union>, std::string> type_name(type<Union>) {
  std::string_view pretty_function(__PRETTY_FUNCTION__);
  const auto L = pretty_function.find("Union = ") + 8;
  const auto R = pretty_function.find_last_of(';');
  std::string name(pretty_function.substr(L, R - L));
  return "union " + name;
}

// template specialize
template <typename T>
std::string type_name(type<type<T>>) {
  return get_type_name<T>();
}

template <typename T>
std::string type_name(type<std::vector<T>>) {
  return "std::vector<" + get_type_name<T>() + ">";
}

template <typename T>
inline std::string type_name(type<std::list<T>>) {
  return "std::list<" + get_type_name<T>() + ">";  // std::__cxx11::list<T>
}

template <typename T>
std::string type_name(type<std::deque<T>>) {
  return "std::deque<" + get_type_name<T>() + ">";
}

template <typename T>
std::string type_name(type<std::stack<T>>) {
  return "std::stack<" + get_type_name<T>() + ">";
}

template <typename T>
std::string type_name(type<std::queue<T>>) {
  return "std::queue<" + get_type_name<T>() + ">";
}

template <typename T, std::size_t N>
std::string type_name(type<std::array<T, N>>) {
  return "std::array<" + get_type_name<T>() + ", " + std::to_string(N) + ">";
}

template <typename T1, typename T2>
std::string type_name(type<std::pair<T1, T2>>) {
  return "std::pair<" + get_type_name<T1>() + ", " + get_type_name<T2>() + ">";
}

template <typename Key>
std::string type_name(type<std::set<Key>>) {
  return "std::set<" + get_type_name<Key>() + ">";
}

template <typename Key>
std::string type_name(type<std::multiset<Key>>) {
  return "std::multiset<" + get_type_name<Key>() + ">";
}

template <typename Key, typename T>
std::string type_name(type<std::map<Key, T>>) {
  return "std::map<" + get_type_name<Key>() + ", " + get_type_name<T>() + ">";
}

template <typename Key, typename T>
std::string type_name(type<std::multimap<Key, T>>) {
  return "std::multimap<" + get_type_name<Key>() + ", " + get_type_name<T>() + ">";
}

template <typename Key, typename T>
std::string type_name(type<std::unordered_map<Key, T>>) {
  return "std::unordered_map<" + get_type_name<Key>() + ", " +
         get_type_name<T>() + ">";
}

template <typename Key>
std::string type_name(type<std::unordered_set<Key>>) {
  return "std::unordered_set<" + get_type_name<Key>() + ">";
}

template <typename T>
std::string type_name(type<std::optional<T>>) {
  return "std::optional<" + get_type_name<T>() + ">";
}

template <typename T>
std::string type_name(type<std::initializer_list<T>>) {
  return "std::initializer_list<" + get_type_name<T>() + ">";
}

template <typename... T>
std::string type_list_to_string() {
  std::string result;
  [[maybe_unused]] auto unused = {(result += get_type_name<T>() + ", ", 0)...,
                                  0};

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

template <typename... T>
std::string type_name(type<std::variant<T...>>) {
  return "std::variant<" + type_list_to_string<T...>() + ">";
}

template <typename T>
std::string type_name(type<hex<T>>) {
  return get_type_name<T>();
}

template <typename T>
std::string type_name(type<oct<T>>) {
  return get_type_name<T>();
}

template <typename T>
std::string type_name(type<bin<T>>) {
  return get_type_name<T>();
}

namespace helper {
template <class Default, class AlwaysVoid, template <class...> class Op,
          class... Args>
struct detector {
  using value_t = std::false_type;
  using type = Default;
};

template <class Default, template <class...> class Op, class... Args>
struct detector<Default, std::void_t<Op<Args...>>, Op, Args...> {
  using value_t = std::true_type;
  using type = Op<Args...>;
};

}  // namespace helper

struct nonesuch {};

template <template <class...> class Op, class... Args>
using is_detected =
    typename helper::detector<nonesuch, void, Op, Args...>::value_t;

template <template <class...> class Op, class... Args>
constexpr bool is_detected_v = is_detected<Op, Args...>::value;

template <typename T>
using detect_begin_t = decltype(std::begin(std::declval<T>()));

template <typename T>
using detect_end_t = decltype(std::end(std::declval<T>()));

template <typename T>
using detect_size_t = decltype(std::size(std::declval<T>()));

template <typename T>
using detect_ostream_operator_t =
    decltype(std::declval<std::ostream&>() << std::declval<T>());

template <typename T>
constexpr bool is_container_v = std::conjunction_v<
    is_detected<detect_begin_t, const T>, is_detected<detect_end_t, const T>,
    is_detected<detect_size_t, const T>,
    std::negation<std::is_same<std::string, std::decay_t<T>>>>;

template <typename T>
constexpr bool has_ostream_operator =
    is_detected_v<detect_ostream_operator_t, T>;

namespace flatten {
struct any_constructor {
  std::size_t I;
  // -Wreturn-type
  template <typename T,
            typename = std::enable_if_t<!std::is_lvalue_reference<T>::value>>
  operator T&&() const&&;

  template <typename T,
            typename = std::enable_if_t<std::is_copy_constructible<T>::value>>
  operator T&() const&;
};

struct class_detect {
  template <typename T, typename = std::enable_if_t<std::is_class_v<T>>>
  operator T&() const&;
};

template <typename T, std::size_t... I>
constexpr auto constructible_nfields(std::index_sequence<I...>) noexcept
    -> decltype(T{any_constructor{I}...});

template <typename T, std::size_t N,
          typename =
              decltype(constructible_nfields<T>(std::make_index_sequence<N>()))>
using constructible_nfields_t = std::size_t;

template <typename T, std::size_t... I1, std::size_t... I2>
constexpr auto constructible_nfields_margs(std::index_sequence<I1...>,
                                           std::index_sequence<I2...>) noexcept
    -> decltype(T{any_constructor{I1}..., {any_constructor{I2}...}});

template <typename T, std::size_t N, std::size_t M,
          typename = decltype(constructible_nfields_margs<T>(
              std::make_index_sequence<N>(), std::make_index_sequence<M>()))>
using constructible_nfields_margs_t = std::size_t;

template <typename T, std::size_t... I>
constexpr auto constructible_nfields_class(std::index_sequence<I...>) noexcept
    -> decltype(T{any_constructor{I}..., class_detect{}});

template <typename T, std::size_t N,
          typename = decltype(
              constructible_nfields_class<T>(std::make_index_sequence<N>()))>
using constructible_nfields_class_t = bool;

template <typename T, std::size_t I0, std::size_t... I>
constexpr auto specific_fields_class_detect(
    std::index_sequence<I0, I...>) noexcept
    -> constructible_nfields_class_t<T, sizeof...(I)> {
  return true;
}

template <typename T, std::size_t... I>
constexpr bool specific_fields_class_detect(
    std::index_sequence<I...>) noexcept {
  return false;
}

template <typename T, std::size_t I0, std::size_t... I>
constexpr auto fields_count(std::index_sequence<I0, I...>) noexcept
    -> constructible_nfields_t<T, sizeof...(I) + 1> {
  return sizeof...(I) + 1;  // I0 + ...I
}

template <typename T, std::size_t... I>
constexpr std::size_t fields_count(std::index_sequence<I...>) noexcept {
  if constexpr (sizeof...(I) > 0) {
    return fields_count<T>(std::make_index_sequence<sizeof...(I) - 1>{});
  } else {
    return 0;
  }
}

template <typename T, std::size_t N, std::size_t I0, std::size_t... I>
constexpr auto specific_fields_count(std::index_sequence<I0, I...>) noexcept
    -> constructible_nfields_margs_t<T, N, sizeof...(I) + 1> {
  return sizeof...(I) + 1;  // I0 + ...I
}

template <typename T, std::size_t N, std::size_t... I>
constexpr std::size_t specific_fields_count(
    std::index_sequence<I...>) noexcept {
  return specific_fields_count<T, N>(
      std::make_index_sequence<sizeof...(I) - 1>{});
}

template <typename T>
constexpr std::size_t counter_impl() noexcept {
  return fields_count<T>(std::make_index_sequence<sizeof(T)>{});
}

template <typename T, std::size_t N>
constexpr std::size_t specific_counter_impl() noexcept {
  return specific_fields_count<T, N>(
      std::make_index_sequence<sizeof(T) + 1>{});  // ! notice + 1
}

template <typename T, std::size_t cur_field, std::size_t total_fields>
constexpr std::size_t unique_fields_count(std::size_t unique_fields) noexcept {
  if constexpr (cur_field == total_fields) {
    return unique_fields;
  } else if constexpr ((specific_fields_class_detect<T>(
                           std::make_index_sequence<cur_field + 1>{}))) {
    return unique_fields_count<T, cur_field + 1, total_fields>(unique_fields +
                                                               1);
  } else {
    return unique_fields_count<
        T, cur_field + specific_counter_impl<T, cur_field>(), total_fields>(
        unique_fields + 1);
  }
}

template <typename T>
constexpr std::size_t unique_counter_impl() noexcept {
  return unique_fields_count<T, 0, counter_impl<T>()>(0);
}

// begin auto generate code
template <typename T>
constexpr auto flatten_impl(
    const T& t, std::integral_constant<std::size_t, 0> N1) noexcept {
  return std::forward_as_tuple();
}

template <typename T>
constexpr auto flatten_impl(
    const T& t, std::integral_constant<std::size_t, 1> N1) noexcept {
  auto& [f1] = t;
  return std::forward_as_tuple(f1);
}
template <typename T>
constexpr auto flatten_impl(
    const T& t, std::integral_constant<std::size_t, 2> N1) noexcept {
  auto& [f1, f2] = t;
  return std::forward_as_tuple(f1, f2);
}
template <typename T>
constexpr auto flatten_impl(
    const T& t, std::integral_constant<std::size_t, 3> N1) noexcept {
  auto& [f1, f2, f3] = t;
  return std::forward_as_tuple(f1, f2, f3);
}
template <typename T>
constexpr auto flatten_impl(
    const T& t, std::integral_constant<std::size_t, 4> N1) noexcept {
  auto& [f1, f2, f3, f4] = t;
  return std::forward_as_tuple(f1, f2, f3, f4);
}
template <typename T>
constexpr auto flatten_impl(
    const T& t, std::integral_constant<std::size_t, 5> N1) noexcept {
  auto& [f1, f2, f3, f4, f5] = t;
  return std::forward_as_tuple(f1, f2, f3, f4, f5);
}
template <typename T>
constexpr auto flatten_impl(
    const T& t, std::integral_constant<std::size_t, 6> N1) noexcept {
  auto& [f1, f2, f3, f4, f5, f6] = t;
  return std::forward_as_tuple(f1, f2, f3, f4, f5, f6);
}
template <typename T>
constexpr auto flatten_impl(
    const T& t, std::integral_constant<std::size_t, 7> N1) noexcept {
  auto& [f1, f2, f3, f4, f5, f6, f7] = t;
  return std::forward_as_tuple(f1, f2, f3, f4, f5, f6, f7);
}
template <typename T>
constexpr auto flatten_impl(
    const T& t, std::integral_constant<std::size_t, 8> N1) noexcept {
  auto& [f1, f2, f3, f4, f5, f6, f7, f8] = t;
  return std::forward_as_tuple(f1, f2, f3, f4, f5, f6, f7, f8);
}
template <typename T>
constexpr auto flatten_impl(
    const T& t, std::integral_constant<std::size_t, 9> N1) noexcept {
  auto& [f1, f2, f3, f4, f5, f6, f7, f8, f9] = t;
  return std::forward_as_tuple(f1, f2, f3, f4, f5, f6, f7, f8, f9);
}
template <typename T>
constexpr auto flatten_impl(
    const T& t, std::integral_constant<std::size_t, 10> N1) noexcept {
  auto& [f1, f2, f3, f4, f5, f6, f7, f8, f9, f10] = t;
  return std::forward_as_tuple(f1, f2, f3, f4, f5, f6, f7, f8, f9, f10);
}
template <typename T>
constexpr auto flatten_impl(
    const T& t, std::integral_constant<std::size_t, 11> N1) noexcept {
  auto& [f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11] = t;
  return std::forward_as_tuple(f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11);
}
template <typename T>
constexpr auto flatten_impl(
    const T& t, std::integral_constant<std::size_t, 12> N1) noexcept {
  auto& [f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12] = t;
  return std::forward_as_tuple(f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11,
                               f12);
}
template <typename T>
constexpr auto flatten_impl(
    const T& t, std::integral_constant<std::size_t, 13> N1) noexcept {
  auto& [f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13] = t;
  return std::forward_as_tuple(f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11,
                               f12, f13);
}
template <typename T>
constexpr auto flatten_impl(
    const T& t, std::integral_constant<std::size_t, 14> N1) noexcept {
  auto& [f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14] = t;
  return std::forward_as_tuple(f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11,
                               f12, f13, f14);
}
template <typename T>
constexpr auto flatten_impl(
    const T& t, std::integral_constant<std::size_t, 15> N1) noexcept {
  auto& [f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15] = t;
  return std::forward_as_tuple(f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11,
                               f12, f13, f14, f15);
}
template <typename T>
constexpr auto flatten_impl(
    const T& t, std::integral_constant<std::size_t, 16> N1) noexcept {
  auto& [f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15,
         f16] = t;
  return std::forward_as_tuple(f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11,
                               f12, f13, f14, f15, f16);
}
// end auto generate code
template <typename T, std::size_t N>
constexpr auto flatten_impl(const T& t, std::integral_constant<std::size_t, N> N1) noexcept {
  helper::get_stream() << printer::error_print("please rerun generate.py to gen more binds! ");
  return std::forward_as_tuple();
}

template <typename T>
constexpr auto flatten_to_tuple(const T& t) noexcept {
  return flatten_impl(
      t, std::integral_constant<std::size_t, unique_counter_impl<T>()>());
}
}  // namespace flatten

namespace printer {
inline void print_char(std::ostream& os, const char& value) {
  if (value >= 0x20 && value <= 0x7E) {
    os << value;
  } else {
    std::ostringstream tmp;
    tmp << "\\x" << std::setw(2) << std::setfill('0') << std::hex
        << std::uppercase << (0xFF & value);
    os << invisible_print(tmp.str());
  }
}

template <typename T>
std::enable_if_t<has_ostream_operator<T>, void> basic_print(std::ostream& os,
                                                            const T& value) {
  os << value;
}

template <typename T>
std::enable_if_t<!has_ostream_operator<T>, void> basic_print(std::ostream& os,
                                                             const T&) {
  os << printer::error_print("ostream operator << not support!");
}

template <typename T>
std::enable_if_t<
    !is_container_v<T> && !std::is_enum_v<T> && !std::is_aggregate_v<T>, void>
print(std::ostream& os, const T& value) {
  basic_print(os, value);
}

// forward declaration for print
template <typename Enum>
std::enable_if_t<std::is_enum_v<Enum>, void> print(std::ostream& os,
                                                   const Enum& value);

template <typename Container>
std::enable_if_t<is_container_v<Container>, void> print(std::ostream& os,
                                                        const Container& value);

template <typename Aggregate>
std::enable_if_t<std::is_aggregate_v<Aggregate> && !is_container_v<Aggregate>,
                 void>
print(std::ostream& os, const Aggregate& value);

template <typename... Ts>
void print(std::ostream& os, const std::tuple<Ts...>& value);

template <>
inline void print(std::ostream& os, const std::tuple<>&);

template <typename T>
void print(std::ostream& os, const std::stack<T>& value);

template <typename T>
void print(std::ostream& os, const std::queue<T>& value);

inline void print(std::ostream& os, const std::string& value);

inline void print(std::ostream& os, const std::string_view& value);

inline void print(std::ostream& os, const char* const& value);

inline void print(std::ostream& os, const char& value);

inline void print(std::ostream& os, const signed char& value);

inline void print(std::ostream& os, const unsigned char& value);

inline void print(std::ostream& os, const std::nullptr_t& value);

inline void print(std::ostream& os, const std::nullopt_t& value);

template <typename T1, typename T2>
void print(std::ostream& os, const std::pair<T1, T2>& value);

template <typename T>
void print(std::ostream& os, T* const& value);

template <>
inline void print(std::ostream& os, char* const& value);

template <>
inline void print(std::ostream& os, signed char* const& value);

template <>
inline void print(std::ostream& os, unsigned char* const& value);

template <typename T>
void print(std::ostream& os, const std::optional<T>& value);

template <typename T, typename Deleter>
void print(std::ostream& os, std::unique_ptr<T, Deleter>& value);

template <typename T>
void print(std::ostream& os, std::shared_ptr<T>& value);

template <typename... Ts>
void print(std::ostream& os, const std::variant<Ts...>& value);
// end of forward declaration for print

template <typename Enum>
std::enable_if_t<std::is_enum_v<Enum>, void> print(std::ostream& os,
                                                   const Enum& value) {
  print(os, static_cast<std::underlying_type_t<Enum>>(value));
}

template <typename Container>
std::enable_if_t<is_container_v<Container>, void> print(
    std::ostream& os, const Container& value) {
  os << "[";
  const size_t size = std::size(value);
  const size_t n = std::min(config::get_container_length(), size);
  size_t i = 0;
  for (auto it = std::begin(value); it != std::end(value) && i < n; ++it, ++i) {
    print(os, *it);
    if (i != n - 1) {
      os << ", ";
    }
  }

  if (size > n) {
    os << ", ...";
    os << " size:" << size;
  }

  os << "]";
}

template <typename Aggregate>
std::enable_if_t<std::is_aggregate_v<Aggregate> && !is_container_v<Aggregate>,
                 void>
print(std::ostream& os, const Aggregate& value) {
  print(os, flatten::flatten_to_tuple(value));
}

template <size_t idx>
struct print_tuple {
  template <typename... Ts>
  void operator()(std::ostream& os, const std::tuple<Ts...>& tuple) {
    print_tuple<idx - 1>()(os, tuple);
    os << ", ";
    print(os, std::get<idx>(tuple));
  }
};

template <>
struct print_tuple<0> {
  template <typename... Ts>
  void operator()(std::ostream& os, const std::tuple<Ts...>& tuple) {
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
inline void print(std::ostream& os, const std::tuple<>&) {
  os << "{}";
}

template <typename T>
void print(std::ostream& os, const std::stack<T>& value) {
  auto stk = value;
  os << "{";
  std::stringstream tmp;
  if (!stk.empty()) {
    print(tmp, stk.top());
    stk.pop();

    while (!stk.empty()) {
      tmp << " ,";  // ! notice
      print(tmp, stk.top());
      stk.pop();
    }
  }
  auto str = tmp.str();
  std::reverse(str.begin(), str.end());
  os << str << '}';
}

template <typename T>
void print(std::ostream& os, const std::queue<T>& value) {
  auto q = value;
  os << "{";
  if (!q.empty()) {
    print(os, q.front());
    q.pop();

    while (!q.empty()) {
      os << ", ";
      print(os, q.front());
      q.pop();
    }
  }
  os << '}';
}

inline void print(std::ostream& os, const std::string& value) {
  os << '"';
  for (const auto& c : value) print_char(os, c);
  os << '"';
}

inline void print(std::ostream& os, const std::string_view& value) {
  print(os, std::string(value));
}

inline void print(std::ostream& os, const char* const& value) {
  print(os, std::string(value));
}

inline void print(std::ostream& os, const bool& value) {
  os << std::boolalpha << value << std::noboolalpha;
}

inline void print(std::ostream& os, const char& value) {
  os << "'";
  print_char(os, value);
  os << "'";
}

inline void print(std::ostream& os, const signed char& value) {
  os << static_cast<int>(value);
}

inline void print(std::ostream& os, const unsigned char& value) {
  os << static_cast<int>(value);
}

inline void print(std::ostream& os, const std::nullptr_t& value) {
  os << "nullptr";
}

inline void print(std::ostream& os, const std::nullopt_t& value) {
  os << "nullopt";
}

template <typename T>
void print(std::ostream& os, T* const& value) {
  if (value == nullptr) {
    print(os, nullptr);
  } else {
    os << value;
  }
}

template <>
void print(std::ostream& os, char* const& value) {
  if (value == nullptr) {
    print(os, nullptr);
  } else {
    os << reinterpret_cast<int*>(value);
  }
}

template <>
void print(std::ostream& os, signed char* const& value) {
  if (value == nullptr) {
    print(os, nullptr);
  } else {
    os << reinterpret_cast<int*>(value);
  }
}

template <>
void print(std::ostream& os, unsigned char* const& value) {
  if (value == nullptr) {
    print(os, nullptr);
  } else {
    os << reinterpret_cast<int*>(value);
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

template <typename T>
void print(std::ostream& os, const std::optional<T>& value) {
  if (value) {
    os << '{';
    print(os, *value);
    os << '}';
  } else {
    os << "nullopt";
  }
}

template <typename T, typename Deleter>
void print(std::ostream& os, std::unique_ptr<T, Deleter>& value) {
  print(os, value.get());
}

template <typename T>
void print(std::ostream& os, std::shared_ptr<T>& value) {
  print(os, value.get());
  os << " (use_count = " << value.use_count() << ")";
}

template <typename... Ts>
void print(std::ostream& os, const std::variant<Ts...>& value) {
  os << "{";
  std::visit([&os](auto&& arg) { print(os, arg); }, value);
  os << "}";
}

template <typename T>
std::enable_if_t<std::is_integral_v<T>, void> print(std::ostream& os,
                                                    const hex<T>& value) {
  std::ostringstream tmp;
  tmp << std::hex << std::showbase << std::uppercase;
  if constexpr (sizeof(T) == 1) {
    tmp << static_cast<int>(value.val);
  } else {
    tmp << value.val;
  }
  os << tmp.str();
}

template <typename T>
std::enable_if_t<!std::is_integral_v<T>, void> print(std::ostream& os,
                                                     const hex<T>& value) {
  os << error_print("hex only support for integral type!");
}

template <typename T>
std::enable_if_t<std::is_integral_v<T>, void> print(std::ostream& os,
                                                    const oct<T>& value) {
  std::ostringstream tmp;
  tmp << std::oct << std::showbase << std::uppercase;
  if constexpr (sizeof(T) == 1) {
    tmp << static_cast<int>(value.val);
  } else {
    tmp << value.val;
  }
  os << tmp.str();
}

template <typename T>
std::enable_if_t<!std::is_integral_v<T>, void> print(std::ostream& os,
                                                     const oct<T>& value) {
  os << error_print("oct only support for integral type!");
}

template <typename T>
std::enable_if_t<std::is_integral_v<T>, void> print(std::ostream& os,
                                                    const bin<T>& value) {
  os << "0B" << std::bitset<8*sizeof(T)>(value.val);
}

template <typename T>
std::enable_if_t<!std::is_integral_v<T>, void> print(std::ostream& os,
                                                     const bin<T>& value) {
  os << error_print("bin only support for integral type!");
}
}  // namespace printer

class Timer {
 public:
  Timer() : cost(0) {}

  void start(std::string message = "") {
    if (message.size()) {
      helper::get_stream() << printer::location_print("[Timer] ")
                           << printer::message_print(message) << '\n';
    }
    start_tp = std::chrono::steady_clock::now();
  }

  void restart(std::string message = "") {
    cost = std::chrono::milliseconds(0);
    start(message);
  }

  void stop() {
    auto stop_tp = std::chrono::steady_clock::now();
    cost += std::chrono::duration_cast<std::chrono::milliseconds>(stop_tp -
                                                                  start_tp);
  }

  void log(std::string message = "") {
    helper::get_stream() << printer::location_print("[Timer] ");
    if (message.size()) {
      helper::get_stream() << printer::message_print(message) << ' ';
    }
    auto secs = std::chrono::duration_cast<std::chrono::seconds>(cost);
    helper::get_stream() << printer::message_print(
        "elapsed " + std::to_string(secs.count()) + '.' +
        helper::to_string((cost - secs).count(), 3) + "s\n");
  }

  static void show() {
    const auto now = std::chrono::system_clock::now();
    auto t = std::chrono::system_clock::to_time_t(now);
    const std::tm* tm = std::localtime(&t);
    helper::get_stream() << printer::location_print("[Timer] ")
                         << printer::message_print(
                                "current time = " +
                                helper::to_string(tm->tm_hour, 2) + ':' +
                                helper::to_string(tm->tm_min, 2) + ':' +
                                helper::to_string(tm->tm_sec, 2) + '\n');
  }

 private:
  std::chrono::milliseconds cost;
  std::chrono::steady_clock::time_point start_tp;
};

inline std::queue<std::string>& get_exprs() {
  static std::queue<std::string> exprs;
  return exprs;
}
inline std::queue<std::string>& get_types() {
  static std::queue<std::string> types;
  return types;
}

#define exprs get_exprs()
#define types get_types()
class debugHelper {
 public:
  debugHelper(const char* file_name, const char* function_name, int line)
      : os(helper::get_stream()) {
    std::string file(file_name);
    if (file.size() > MAX_LENGTH) {
      file = ".." + file.substr(file.size() - MAX_LENGTH, MAX_LENGTH);
    }
    location =
        "[" + file + ":" + std::to_string(line) + " (" + function_name + ")]";
  }

  static void push_expr(const std::string& expr) { exprs.emplace(expr); }
  static void push_type(const std::string& type) { types.emplace(type); }

  template <typename... Ts>
  void print(Ts&&... values) {
    if constexpr (sizeof...(values) > 0) {
      os << printer::location_print(location) << " ";
      print_expand(values...);
    } else {
      os << printer::message_print(std::string(100, '='));
    }
    os << '\n';
  }

 private:
  template <typename Head>
  void print_expand(Head&& head) {
    os << printer::expression_print(exprs.front()) << " = ";
    exprs.pop();

    std::stringstream value;
    printer::print(value, head);
    os << printer::value_print(value.str());

    os << " (" << printer::type_print(types.front()) << ")";
    types.pop();
  }

  template <std::size_t N>
  void print_expand(const char (&head)[N]) {
    exprs.pop();

    os << printer::message_print(head);

    types.pop();
  }

  template <typename T>
  void print_expand(type<T> head) {
    exprs.pop();

    os << printer::type_print(types.front() + " [sizeof " +
                              std::to_string(sizeof(T)) + "]");

    types.pop();
  }

  template <typename Head, typename... Tail>
  void print_expand(Head&& head, Tail&&... tail) {
    os << printer::expression_print(exprs.front()) << " = ";
    exprs.pop();

    std::stringstream value;
    printer::print(value, head);
    os << printer::value_print(value.str());

    os << " (" << printer::type_print(types.front()) << ") ";
    types.pop();

    print_expand(tail...);
  }

  template <std::size_t N, typename... Tail>
  void print_expand(const char (&head)[N], Tail&&... tail) {
    exprs.pop();

    os << printer::message_print(head) << " ";

    types.pop();

    print_expand(tail...);
  }

  template <typename T, typename... Tail>
  void print_expand(type<T> head, Tail&&... tail) {
    exprs.pop();

    os << printer::type_print(types.front() + " [sizeof " +
                              std::to_string(sizeof(T)) + "]")
       << " ";

    types.pop();

    print_expand(tail...);
  }

  std::ostream& os;
  std::string location;

  static constexpr std::size_t MAX_LENGTH = 20;
};
#undef types
#undef exprs

}  // namespace dbg

#ifdef SINGLE
#define LOG(x)                                                      \
  do {                                                              \
    dbg::debugHelper::push_expr(static_cast<std::string>(#x));      \
    dbg::debugHelper::push_type(dbg::get_type_name<decltype(x)>()); \
    dbg::debugHelper(__FILE__, __func__, __LINE__).print(x);        \
  } while (false)
#else
#define DBG_PARENS ()
#define DBG_EXPAND(arg) DBG_EXPAND1(DBG_EXPAND1(DBG_EXPAND1(DBG_EXPAND1(arg))))
#define DBG_EXPAND1(arg) DBG_EXPAND2(DBG_EXPAND2(DBG_EXPAND2(DBG_EXPAND2(arg))))
#define DBG_EXPAND2(arg) DBG_EXPAND3(DBG_EXPAND3(DBG_EXPAND3(DBG_EXPAND3(arg))))
#define DBG_EXPAND3(arg) DBG_EXPAND4(DBG_EXPAND4(DBG_EXPAND4(DBG_EXPAND4(arg))))
#define DBG_EXPAND4(arg) arg
#define DBG_FOR_EACH(func, ...) \
  __VA_OPT__(DBG_EXPAND(DBG_FOR_EACH_HELPER(func, __VA_ARGS__)))
#define DBG_FOR_EACH_HELPER(func, a1, ...) \
  func(a1) __VA_OPT__(DBG_FOR_EACH_AGAIN DBG_PARENS(func, __VA_ARGS__))
#define DBG_FOR_EACH_AGAIN() DBG_FOR_EACH_HELPER

#define DBG_SAVE_EXPR(x) \
  dbg::debugHelper::push_expr(static_cast<std::string>(#x));
#define DBG_SAVE_TYPE(x) \
  dbg::debugHelper::push_type(dbg::get_type_name<decltype(x)>());

#define LOG(...)                                                       \
  do {                                                                 \
    DBG_FOR_EACH(DBG_SAVE_EXPR, __VA_ARGS__)                           \
    DBG_FOR_EACH(DBG_SAVE_TYPE, __VA_ARGS__)                           \
    dbg::debugHelper(__FILE__, __func__, __LINE__).print(__VA_ARGS__); \
  } while (false)

#endif

#define TYPE(x) dbg::type<decltype(x)>()

#endif