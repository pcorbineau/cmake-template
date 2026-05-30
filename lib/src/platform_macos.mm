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
// CoolguiContentView — custom NSView that reports mouse/key events
// ---------------------------------------------------------------------------
@interface CoolguiContentView : NSView
@end

@implementation CoolguiContentView

- (BOOL)acceptsFirstResponder {
  return YES;
}

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

- (void)mouseDown:(NSEvent *)event {
  NSValue *value = objc_getAssociatedObject(self, &kEventQueueKey);
  if (value != nil) {
    std::vector<coolgui::Event> *queue = nullptr;
    [value getValue:(void *)&queue];
    if (queue != nullptr) {
      NSPoint const location = [event locationInWindow];
      queue->emplace_back(coolgui::MouseButtonEvent{
          .button = coolgui::MouseButton::Left,
          .x = static_cast<coolgui::f64>(location.x),
          .y = static_cast<coolgui::f64>(location.y),
      });
    }
  }
}

- (void)keyDown:(NSEvent *)event {
  NSValue *value = objc_getAssociatedObject(self, &kEventQueueKey);
  if (value != nil) {
    std::vector<coolgui::Event> *queue = nullptr;
    [value getValue:(void *)&queue];
    if (queue != nullptr) {
      NSString *chars = [event characters];
      if ([chars length] > 0) {
        queue->emplace_back(coolgui::TextInputEvent{
            .character = static_cast<char>([chars characterAtIndex:0]),
        });
      }
    }
  }
}

- (void)insertText:(id)string {
  // Suppress beep from insertText: when there's no active editor
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

  // Install custom content view for cursor/key tracking
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

  Handle h{.window = window, .content_view = contentView, .event_queue = {}};
  NSValue *const queuePtr = [NSValue valueWithPointer:&h.event_queue];
  objc_setAssociatedObject(window, &kEventQueueKey, queuePtr, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
  objc_setAssociatedObject(contentView, &kEventQueueKey, queuePtr, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
  [contentView release];
  return h;
}

auto MacosTraits::destroy(Handle &handle) -> void {
  if (handle.window != nullptr) {
    // Release overlay views
    if (handle.overlay_view != nullptr) {
      [handle.overlay_view removeFromSuperview];
      [handle.overlay_view release];
      handle.overlay_view = nullptr;
    }
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
    NSSize const size = [handle.window contentView].frame.size;

    [NSApp sendEvent:event];

    // Detect resize
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

auto MacosTraits::render_color_panel(Handle &handle, const ColorPanel &panel) -> void {
  if (handle.window == nullptr || handle.content_view == nullptr) { return; }

  auto const layout = panel.layout();
  NSView *const parent = handle.content_view;

  if (handle.overlay_view == nullptr) {
    // First call — create all subviews
    handle.overlay_view = [[NSView alloc] initWithFrame:NSMakeRect(
        static_cast<CGFloat>(layout.panel_rect.x),
        static_cast<CGFloat>(layout.panel_rect.y),
        static_cast<CGFloat>(layout.panel_rect.width),
        static_cast<CGFloat>(layout.panel_rect.height))];

    // Label for current values
    handle.value_label = [[NSTextField alloc] initWithFrame:NSMakeRect(
        static_cast<CGFloat>(layout.value_label_rect.x),
        static_cast<CGFloat>(layout.value_label_rect.y),
        static_cast<CGFloat>(layout.value_label_rect.width),
        static_cast<CGFloat>(layout.value_label_rect.height))];
    [handle.value_label setEditable:NO];
    [handle.value_label setBezeled:NO];
    [handle.value_label setDrawsBackground:NO];
    [handle.value_label setFont:[NSFont boldSystemFontOfSize:13]];
    [handle.overlay_view addSubview:handle.value_label];

    // Force indicator (checkbox-style, display-only)
    handle.force_indicator = [[NSButton alloc] initWithFrame:NSMakeRect(
        static_cast<CGFloat>(layout.force_button_rect.x),
        static_cast<CGFloat>(layout.force_button_rect.y),
        static_cast<CGFloat>(layout.force_button_rect.width),
        static_cast<CGFloat>(layout.force_button_rect.height))];
    [handle.force_indicator setButtonType:NSButtonTypeSwitch];
    [handle.force_indicator setTitle:NSLocalizedString(@"Force", nil)];
    [handle.force_indicator setEnabled:NO];
    [handle.overlay_view addSubview:handle.force_indicator];

    // Text field R
    handle.text_r_field = [[NSTextField alloc] initWithFrame:NSMakeRect(
        static_cast<CGFloat>(layout.text_r_rect.x),
        static_cast<CGFloat>(layout.text_r_rect.y),
        static_cast<CGFloat>(layout.text_r_rect.width),
        static_cast<CGFloat>(layout.text_r_rect.height))];
    [handle.text_r_field setEditable:NO];
    [handle.text_r_field setBezeled:YES];
    [handle.text_r_field setDrawsBackground:YES];
    [handle.text_r_field setAlignment:NSTextAlignmentCenter];
    [handle.text_r_field setFont:[NSFont systemFontOfSize:11]];
    [handle.overlay_view addSubview:handle.text_r_field];

    // Text field G
    handle.text_g_field = [[NSTextField alloc] initWithFrame:NSMakeRect(
        static_cast<CGFloat>(layout.text_g_rect.x),
        static_cast<CGFloat>(layout.text_g_rect.y),
        static_cast<CGFloat>(layout.text_g_rect.width),
        static_cast<CGFloat>(layout.text_g_rect.height))];
    [handle.text_g_field setEditable:NO];
    [handle.text_g_field setBezeled:YES];
    [handle.text_g_field setDrawsBackground:YES];
    [handle.text_g_field setAlignment:NSTextAlignmentCenter];
    [handle.text_g_field setFont:[NSFont systemFontOfSize:11]];
    [handle.overlay_view addSubview:handle.text_g_field];

    // Text field B
    handle.text_b_field = [[NSTextField alloc] initWithFrame:NSMakeRect(
        static_cast<CGFloat>(layout.text_b_rect.x),
        static_cast<CGFloat>(layout.text_b_rect.y),
        static_cast<CGFloat>(layout.text_b_rect.width),
        static_cast<CGFloat>(layout.text_b_rect.height))];
    [handle.text_b_field setEditable:NO];
    [handle.text_b_field setBezeled:YES];
    [handle.text_b_field setDrawsBackground:YES];
    [handle.text_b_field setAlignment:NSTextAlignmentCenter];
    [handle.text_b_field setFont:[NSFont systemFontOfSize:11]];
    [handle.overlay_view addSubview:handle.text_b_field];

    // Add a subtle background
    [handle.overlay_view setWantsLayer:YES];
    handle.overlay_view.layer.backgroundColor = [[NSColor colorWithCalibratedWhite:0.95 alpha:0.9] CGColor];
    handle.overlay_view.layer.cornerRadius = 5.0;
    handle.overlay_view.layer.borderWidth = 1.0;
    handle.overlay_view.layer.borderColor = [[NSColor lightGrayColor] CGColor];

    [parent addSubview:handle.overlay_view];
    [handle.overlay_view release];
  }

  // Update values — stringWithUTF8String returns non-nil for valid ASCII/UTF-8 C strings
  auto setLabel = [](NSTextField *field, const std::string &text) -> void {
    NSString *const nsStr = [NSString stringWithUTF8String:text.c_str()];
    field.stringValue = nsStr != nil ? nsStr : @"";
  };
  auto setButtonState = [](NSButton *button, bool on) -> void {
    button.state = on ? NSControlStateValueOn : NSControlStateValueOff;
  };

  setLabel(handle.value_label, panel.display_text());
  setButtonState(handle.force_indicator, panel.is_force_mode());
  setLabel(handle.text_r_field, panel.text_r());
  setLabel(handle.text_g_field, panel.text_g());
  setLabel(handle.text_b_field, panel.text_b());
}

} // namespace coolgui
