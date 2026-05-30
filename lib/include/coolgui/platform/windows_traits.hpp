#pragma once

#include <windows.h>

#include <optional>

#include "coolgui/events.hpp"
#include "coolgui/window_config.hpp"

namespace coolgui {

struct WindowsHandle {
  HWND hwnd{nullptr};
  bool close_requested{false};
};

struct WindowsTraits {
  using Handle = WindowsHandle;

  static auto create(WindowConfig cfg) -> Handle;
  static auto destroy(Handle &handle) -> void;
  static auto poll_event(Handle &handle) -> std::optional<Event>;
};

} // namespace coolgui
