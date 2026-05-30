#pragma once

#include "coolgui/window_types.hpp"

namespace coolgui {

struct WidgetRect {
  f64 x{};
  f64 y{};
  f64 width{};
  f64 height{};

  [[nodiscard]] constexpr auto contains(f64 px, f64 py) const noexcept -> bool {
    return px >= x && px < x + width && py >= y && py < y + height;
  }
};

[[nodiscard]] constexpr auto operator==(const WidgetRect &lhs, const WidgetRect &rhs) noexcept -> bool {
  return lhs.x == rhs.x && lhs.y == rhs.y && lhs.width == rhs.width && lhs.height == rhs.height;
}

enum class MouseButton : u8 {
  Left,
  Right,
  Middle,
};

} // namespace coolgui
