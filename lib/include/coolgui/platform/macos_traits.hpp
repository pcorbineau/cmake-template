#pragma once

#include <optional>
#include <string>
#include <vector>

#include "coolgui/color_panel.hpp"
#include "coolgui/events.hpp"
#include "coolgui/window_config.hpp"

// Forward-declare the Objective-C types as opaque pointers so this header
// remains includable from pure C++ translation units.
#ifdef __OBJC__
#import <Cocoa/Cocoa.h>
#else
using NSWindow = struct objc_object;
using NSView = struct objc_object;
using NSButton = struct objc_object;
using NSTextField = struct objc_object;
using NSApplication = struct objc_object;
#endif

namespace coolgui {

struct MacosHandle {
  NSWindow *window{nullptr};
  NSView *content_view{nullptr};
  std::vector<Event> event_queue;

  // Overlay views (display-only, lazily created)
  NSView *overlay_view{nullptr};
  NSTextField *value_label{nullptr};
  NSButton *force_indicator{nullptr};
  NSTextField *text_r_field{nullptr};
  NSTextField *text_g_field{nullptr};
  NSTextField *text_b_field{nullptr};
};

struct MacosTraits {
  using Handle = MacosHandle;

  static auto create(WindowConfig cfg) -> Handle;
  static auto destroy(Handle &handle) -> void;
  static auto poll_event(Handle &handle) -> std::optional<Event>;
  static auto set_background_color(Handle &handle, BackgroundColor color) -> void;

  // Overlay rendering (macOS-specific extension)
  static auto render_color_panel(Handle &handle, const ColorPanel &panel) -> void;
};

} // namespace coolgui
