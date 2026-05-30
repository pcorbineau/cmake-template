#pragma once

#include <charconv>
#include <cstdlib>
#include <limits>
#include <span>
#include <string>
#include <type_traits>

namespace mylib {

auto add(int l, int t) -> int;

// not called
constexpr auto add_constexpr(int l, int t) -> int { return l + t; }

// not called
inline auto add_inline(int l, int t) -> int { return l + t; }

// called
constexpr auto add_constexpr2(int l, int t) -> int { return l + t; }

// called
inline auto add_inline2(int l, int t) -> int { return l + t; }

constexpr auto to_string(std::integral auto value) -> std::string {
  using T = std::remove_cvref_t<decltype(value)>;

  constexpr auto size = std::numeric_limits<T>::digits10 + std::is_signed_v<T> + 1;
  std::string result(size, '\0');
  char *begin = result.data();
  char *end = std::next(begin, size);
  auto [ptr, ec] = std::to_chars(begin, end, value);
  // consider std::to_chars will never fail for integral types, so we can ignore the error code

  const auto actual_size = std::span{begin, ptr}.size();
  result.resize(actual_size);
  return result;
}

static_assert(to_string(123) == "123");
static_assert(to_string(-123) == "-123");
static_assert(to_string(0) == "0");

} // namespace mylib
