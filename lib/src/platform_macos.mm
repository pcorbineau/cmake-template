#import <Cocoa/Cocoa.h>
#import <objc/runtime.h>

#include <string>

#include "coolgui/platform/macos_traits.hpp"

// ---------------------------------------------------------------------------
// AppDelegate — handles applicationDidFinishLaunching and termination
// ---------------------------------------------------------------------------
@interface CoolguiAppDelegate : NSObject <NSApplicationDelegate>
@end

@implementation CoolguiAppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)__unused notification {
  [NSApp stop:nil]; // Unblock the initial run loop spin used during creation
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)__unused sender {
  return YES;
}

@end

static const char kEventQueueKey = 0;

// ---------------------------------------------------------------------------
// CoolguiContentView — custom NSView that reports mouse-moved events
// ---------------------------------------------------------------------------
@interface CoolguiContentView : NSView
@end

@implementation CoolguiContentView

- (void)mouseMoved:(NSEvent *)event {
  NSValue *value = objc_getAssociatedObject(self, &kEventQueueKey);
  if (value != nil) {
    std::vector<coolgui::Event> *queue = nullptr;
    [value getValue:(void *)&queue];
    if (queue != nullptr) {
      NSPoint const location = [event locationInWindow];
      queue->emplace_back(coolgui::CursorMoveEvent{
          .x = coolgui::CursorX{static_cast<coolgui::f64>(location.x)},
          .y = coolgui::CursorY{static_cast<coolgui::f64>(location.y)},
      });
    }
  }
}

@end

// ---------------------------------------------------------------------------
// WindowDelegate — pushes CloseEvent into the C++ event queue
// ---------------------------------------------------------------------------
@interface CoolguiWindowDelegate : NSObject <NSWindowDelegate>
@end

@implementation CoolguiWindowDelegate

- (BOOL)windowShouldClose:(NSWindow *)sender {
  NSValue *value = objc_getAssociatedObject(sender, &kEventQueueKey);
  if (value != nil) {
    std::vector<coolgui::Event> *queue = nullptr;
    [value getValue:(void *)&queue];
    if (queue != nullptr) {
      queue->emplace_back(coolgui::CloseEvent{});
    }
  }
  return NO; // We manage destruction ourselves
}

@end

// ---------------------------------------------------------------------------
// MacosTraits implementation
// ---------------------------------------------------------------------------
namespace coolgui {

auto MacosTraits::create(WindowConfig cfg) -> Handle {
  if ([NSApplication sharedApplication] == nil) {
    [NSApplication sharedApplication];
  }

  [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];

  static auto *const kAppDelegate = [[CoolguiAppDelegate alloc] init];
  [NSApp setDelegate:kAppDelegate];

  NSRect const frame = NSMakeRect(0, 0, static_cast<CGFloat>(cfg.width.get()), static_cast<CGFloat>(cfg.height.get()));

  NSWindowStyleMask const style = NSWindowStyleMaskTitled | NSWindowStyleMaskClosable |
                                  NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable;

  NSWindow *const window = [[NSWindow alloc] initWithContentRect:frame
                                                       styleMask:style
                                                         backing:NSBackingStoreBuffered
                                                           defer:NO];

  auto titleStr = std::string{cfg.title.get()};
  if (auto *const title = [NSString stringWithUTF8String:titleStr.c_str()]) {
    window.title = title;
  } else {
    window.title = @"";
  }

  // Set initial background color
  NSColor *const bgColor = [NSColor colorWithCalibratedRed:cfg.background_color.red
                                                     green:cfg.background_color.green
                                                      blue:cfg.background_color.blue
                                                     alpha:1.0];
  window.backgroundColor = bgColor;

  // Install custom content view for cursor tracking
  CoolguiContentView *const contentView = [[CoolguiContentView alloc] initWithFrame:frame];
  window.contentView = contentView;

  NSTrackingArea *const trackingArea =
      [[NSTrackingArea alloc] initWithRect:contentView.bounds
                                   options:NSTrackingMouseMoved | NSTrackingActiveInActiveApp | NSTrackingInVisibleRect
                                     owner:contentView
                                  userInfo:nil];
  [contentView addTrackingArea:trackingArea];
  [trackingArea release];
  [window setAcceptsMouseMovedEvents:YES];

  auto *const winDelegate = [[CoolguiWindowDelegate alloc] init];
  [window setDelegate:winDelegate];
  objc_setAssociatedObject(window, @selector(delegate), winDelegate, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
  [winDelegate release];

  [window center];
  [window makeKeyAndOrderFront:nil];
  [NSApp activateIgnoringOtherApps:YES];

  // Spin the run loop once so the window becomes visible before returning.
  [NSApp run];

  Handle h{.window = window, .event_queue = {}};
  NSValue *const queuePtr = [NSValue valueWithPointer:&h.event_queue];
  objc_setAssociatedObject(window, &kEventQueueKey, queuePtr, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
  objc_setAssociatedObject(contentView, &kEventQueueKey, queuePtr, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
  [contentView release];
  return h;
}

auto MacosTraits::destroy(Handle &handle) -> void {
  if (handle.window != nullptr) {
    [handle.window close];
    handle.window = nullptr;
  }
}

auto MacosTraits::poll_event(Handle &handle) -> std::optional<Event> {
  if (handle.window == nullptr) {
    return std::nullopt;
  }

  // Pop queued events first (pushed by the window/content view delegates)
  if (!handle.event_queue.empty()) {
    auto evt = handle.event_queue.front();
    handle.event_queue.erase(handle.event_queue.begin());
    return evt;
  }

  // Drain one event from the Cocoa event queue (non-blocking)
  NSEvent *const event = [NSApp nextEventMatchingMask:NSEventMaskAny
                                            untilDate:nil
                                               inMode:NSDefaultRunLoopMode
                                              dequeue:YES];
  if (event != nil) {
    // Check for resize via NSWindowDidResizeNotification is handled via
    // the run loop; for now forward all events to NSApp and check window frame.
    NSSize const size = [handle.window contentView].frame.size;

    [NSApp sendEvent:event];

    // Detect resize: compare with last known size would require storing it —
    // for now we emit a ResizeEvent when the content size changes after
    // dispatching. A future iteration can track previous size in the Handle.
    NSSize const newSize = [handle.window contentView].frame.size;
    if (newSize.width != size.width || newSize.height != size.height) {
      return Event{ResizeEvent{
          .width = Width{static_cast<u32>(newSize.width)},
          .height = Height{static_cast<u32>(newSize.height)},
      }};
    }
  }

  return std::nullopt;
}

auto MacosTraits::set_background_color(Handle &handle, BackgroundColor color) -> void {
  if (handle.window != nullptr) {
    NSColor *const nsColor = [NSColor colorWithCalibratedRed:color.red green:color.green blue:color.blue alpha:1.0];
    handle.window.backgroundColor = nsColor;
  }
}

} // namespace coolgui
