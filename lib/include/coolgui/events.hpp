#pragma once

#include <variant>

#include "coolgui/window_types.hpp"

namespace coolgui {

struct CloseEvent {};

struct ResizeEvent {
    Width  width;
    Height height;
};

using Event = std::variant<CloseEvent, ResizeEvent>;

} // namespace coolgui
