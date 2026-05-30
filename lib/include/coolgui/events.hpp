#pragma once

#include <variant>

#include "coolgui/widget_types.hpp"
#include "coolgui/window_types.hpp"

namespace coolgui {

struct CloseEvent {};

struct ResizeEvent {
  Width width;
  Height height;
};

struct CursorMoveEvent {
  CursorX x;
  CursorY y;
};

struct MouseButtonEvent {
  MouseButton button;
  f64 x;
  f64 y;
};

struct TextInputEvent {
  char character{};
};

using Event = std::variant<CloseEvent, ResizeEvent, CursorMoveEvent, MouseButtonEvent, TextInputEvent>;

} // namespace coolgui
