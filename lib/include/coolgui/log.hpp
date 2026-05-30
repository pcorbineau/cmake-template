#pragma once

#include <spdlog/spdlog.h>

namespace coolgui {

template <typename... Args> constexpr void LOGI(spdlog::format_string_t<Args...> fmt, Args &&...args) {
  if consteval {
    // compile-time: no-op
  } else {
    spdlog::info(fmt, std::forward<Args>(args)...);
  }
}

template <typename... Args> constexpr void LOGE(spdlog::format_string_t<Args...> fmt, Args &&...args) {
  if consteval {
    // compile-time: no-op
  } else {
    spdlog::error(fmt, std::forward<Args>(args)...);
  }
}

template <typename... Args> constexpr void LOGV(spdlog::format_string_t<Args...> fmt, Args &&...args) {
  if consteval {
    // compile-time: no-op
  } else {
    spdlog::debug(fmt, std::forward<Args>(args)...);
  }
}

template <typename... Args> constexpr void LOGW(spdlog::format_string_t<Args...> fmt, Args &&...args) {
  if consteval {
    // compile-time: no-op
  } else {
    spdlog::warn(fmt, std::forward<Args>(args)...);
  }
}

template <typename... Args> constexpr void LOGF(spdlog::format_string_t<Args...> fmt, Args &&...args) {
  if consteval {
    // compile time: no-op
  } else {
    spdlog::critical(fmt, std::forward<Args>(args)...);
  }
}

} // namespace coolgui
