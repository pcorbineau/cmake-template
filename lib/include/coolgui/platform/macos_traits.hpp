#pragma once

#include <optional>
#include <vector>

#include "coolgui/events.hpp"
#include "coolgui/window_config.hpp"

// Forward-declare the Objective-C types as opaque pointers so this header
// remains includable from pure C++ translation units.
#ifdef __OBJC__
#import <Cocoa/Cocoa.h>
#else
using NSWindow = struct objc_object;
using NSApplication = struct objc_object;
#endif

namespace coolgui {

struct MacosHandle {
  NSWindow *window{nullptr};
  std::vector<Event> event_queue;
};

struct MacosTraits {
  using Handle = MacosHandle;

  static auto create(WindowConfig cfg) -> Handle;
  static auto destroy(Handle &handle) -> void;
  static auto poll_event(Handle &handle) -> std::optional<Event>;
};

} // namespace coolgui
