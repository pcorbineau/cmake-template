#pragma once

#include <windows.h>

#include <optional>
#include <vector>

#include "coolgui/events.hpp"
#include "coolgui/window_config.hpp"

namespace coolgui {

struct WindowsHandle {
  HWND hwnd{nullptr};
  std::vector<Event> event_queue;
};

struct WindowsTraits {
  using Handle = WindowsHandle;

  static auto create(WindowConfig cfg) -> Handle;
  static auto destroy(Handle &handle) -> void;
  static auto poll_event(Handle &handle) -> std::optional<Event>;
  static auto set_background_color(Handle &handle, BackgroundColor color) -> void;
};

} // namespace coolgui
