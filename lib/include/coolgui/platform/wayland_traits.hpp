#pragma once

#include <memory>
#include <optional>

#include "coolgui/events.hpp"
#include "coolgui/window_config.hpp"

// Forward-declare wayland types to keep this header free of wayland-client.h.
// The full types are only needed in the .cpp implementation.
struct wl_display;
struct wl_registry;
struct wl_compositor;
struct wl_surface;
struct xdg_wm_base;
struct xdg_surface;
struct xdg_toplevel;

namespace coolgui {

// All mutable Wayland state lives behind a unique_ptr so that the address
// remains stable after a move — the C listener callbacks hold a raw back-
// pointer to this struct.
struct WaylandState {
  wl_display *display = nullptr;
  wl_registry *registry = nullptr;
  wl_compositor *compositor = nullptr;
  wl_surface *surface = nullptr;
  xdg_wm_base *xdg_wm = nullptr;
  xdg_surface *xdg_surf = nullptr;
  xdg_toplevel *toplevel = nullptr;

  bool close_requested = false;
  bool configured = false; // true after first xdg_surface configure

  u32 pending_width = 0; // 0 = compositor didn't suggest a size
  u32 pending_height = 0;
  bool resize_pending = false;
};

struct WaylandHandle {
  std::unique_ptr<WaylandState> state;
};

struct WaylandTraits {
  using Handle = WaylandHandle;

  static auto create(WindowConfig cfg) -> Handle;
  static auto destroy(Handle &handle) -> void;
  static auto poll_event(Handle &handle) -> std::optional<Event>;
};

} // namespace coolgui
