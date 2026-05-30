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

  auto create(WindowConfig cfg) -> Handle;
  auto destroy(Handle &handle) -> void;
  auto poll_event(Handle &handle) -> std::optional<Event>;
};

} // namespace coolgui
