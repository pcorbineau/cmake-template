#pragma once

#include <memory>
#include <optional>
#include <vector>

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

  std::vector<Event> event_queue;
  bool configured = false; // true after first xdg_surface configure

  bool resize_pending = false; // triggers wl_surface_commit in xdg_surface_configure
};

struct WaylandHandle {
  std::unique_ptr<WaylandState> state;
};

struct WaylandTraits {
  using Handle = WaylandHandle;

  static auto create(WindowConfig cfg) -> Handle;
  static auto destroy(Handle &handle) -> void;
  static auto poll_event(Handle &handle) -> std::optional<Event>;
  static auto set_background_color(Handle &handle, BackgroundColor color) -> void;
};

} // namespace coolgui
