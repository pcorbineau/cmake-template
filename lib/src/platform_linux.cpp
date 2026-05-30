#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <string>

#include "coolgui/log.hpp"
#include "coolgui/platform/linux_traits.hpp"

namespace coolgui {

auto LinuxTraits::create(WindowConfig cfg) -> Handle {
  Display *display = XOpenDisplay(nullptr);
  if (display == nullptr) {
    LOGE("Failed to open X display");
    return Handle{};
  }

  int screen = DefaultScreen(display);

  XWindow window = XCreateSimpleWindow(display, RootWindow(display, screen), 0, 0, cfg.width.get(), cfg.height.get(),
                                       /*border_width=*/1, BlackPixel(display, screen), WhitePixel(display, screen));

  auto titleStr = std::string{cfg.title.get()};
  XStoreName(display, window, titleStr.c_str());

  // Register interest in window close events
  Atom wm_delete = XInternAtom(display, "WM_DELETE_WINDOW", False);
  XSetWMProtocols(display, window, &wm_delete, 1);

  XSelectInput(display, window, ExposureMask | StructureNotifyMask | KeyPressMask);
  XMapWindow(display, window);
  XFlush(display);

  LOGI("X11 window created");
  return Handle{display, window, wm_delete};
}

auto LinuxTraits::destroy(Handle &handle) -> void {
  if (handle.display != nullptr) {
    XDestroyWindow(handle.display, handle.window);
    XCloseDisplay(handle.display);
    handle.display = nullptr;
    LOGI("X11 window destroyed");
  }
}

auto LinuxTraits::poll_event(Handle &handle) -> std::optional<Event> {
  if (handle.display == nullptr) {
    return std::nullopt;
  }

  if (XPending(handle.display) == 0) {
    return std::nullopt;
  }

  XEvent xev{};
  XNextEvent(handle.display, &xev);

  if (xev.type == ClientMessage) {
    if (static_cast<Atom>(xev.xclient.data.l[0]) == handle.wm_delete_window) {
      return Event{CloseEvent{}};
    }
  }

  if (xev.type == ConfigureNotify) {
    return Event{ResizeEvent{
        Width{static_cast<u32>(xev.xconfigure.width)},
        Height{static_cast<u32>(xev.xconfigure.height)},
    }};
  }

  return std::nullopt;
}

} // namespace coolgui
