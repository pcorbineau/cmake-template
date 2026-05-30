#pragma once

#include <variant>

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

using Event = std::variant<CloseEvent, ResizeEvent, CursorMoveEvent>;

} // namespace coolgui
