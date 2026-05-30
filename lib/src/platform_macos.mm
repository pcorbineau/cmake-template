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

// ---------------------------------------------------------------------------
// WindowDelegate — tracks close button clicks
// ---------------------------------------------------------------------------
@interface CoolguiWindowDelegate : NSObject <NSWindowDelegate>
@property(nonatomic, assign) BOOL closeRequested;
@end

@implementation CoolguiWindowDelegate

- (instancetype)init {
  self = [super init];
  _closeRequested = NO;
  return self;
}

- (BOOL)windowShouldClose:(NSWindow *)__unused sender {
  self.closeRequested = YES;
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

  NSWindowStyleMask const style = NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable |
                                  NSWindowStyleMaskResizable;

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

  auto *const winDelegate = [[CoolguiWindowDelegate alloc] init];
  [window setDelegate:winDelegate];
  objc_setAssociatedObject(window, @selector(delegate), winDelegate, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
  [winDelegate release];

  [window center];
  [window makeKeyAndOrderFront:nil];
  [NSApp activateIgnoringOtherApps:YES];

  // Spin the run loop once so the window becomes visible before returning.
  [NSApp run];

  return Handle{window};
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

  // Check if close was requested via the window delegate
  auto *const winDelegate = static_cast<CoolguiWindowDelegate *>([handle.window delegate]);
  if (winDelegate != nil && winDelegate.closeRequested) {
    winDelegate.closeRequested = NO;
    return Event{CloseEvent{}};
  }

  // Drain one event from the Cocoa event queue (non-blocking)
  NSEvent *const event = [NSApp nextEventMatchingMask:NSEventMaskAny untilDate:nil inMode:NSDefaultRunLoopMode dequeue:YES];
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

} // namespace coolgui
