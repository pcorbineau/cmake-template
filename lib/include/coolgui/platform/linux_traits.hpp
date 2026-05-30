#pragma once

#include <optional>

#include "coolgui/events.hpp"
#include "coolgui/window_config.hpp"

struct _XDisplay;
using Display = _XDisplay;
using XWindow = unsigned long;

namespace coolgui {

struct LinuxHandle {
  Display *display{nullptr};
  XWindow window{};
  XWindow wm_delete_window{}; // Atom for WM_DELETE_WINDOW protocol
};

struct LinuxTraits {
  using Handle = LinuxHandle;

  static auto create(WindowConfig cfg) -> Handle;
  static auto destroy(Handle &handle) -> void;
  static auto poll_event(Handle &handle) -> std::optional<Event>;
  static auto set_background_color(Handle &handle, BackgroundColor color) -> void;
};

} // namespace coolgui
