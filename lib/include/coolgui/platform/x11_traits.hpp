#pragma once

#include <optional>

#include "coolgui/events.hpp"
#include "coolgui/window_config.hpp"

struct _XDisplay;
using Display = _XDisplay;
using XWindow = unsigned long;
using XColormap = unsigned long;

namespace coolgui {

struct X11Handle {
  Display *display{nullptr};
  XWindow window{};
  XWindow wm_delete_window{};
  XColormap colormap{};
};

struct X11Traits {
  using Handle = X11Handle;

  static auto create(WindowConfig cfg) -> Handle;
  static auto destroy(Handle &handle) -> void;
  static auto poll_event(Handle &handle) -> std::optional<Event>;
  static auto set_background_color(Handle &handle, BackgroundColor color) -> void;
};

} // namespace coolgui
