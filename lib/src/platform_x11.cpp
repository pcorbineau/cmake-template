#include <X11/Xlib.h>
#include <string>

#include "coolgui/log.hpp"
#include "coolgui/platform/x11_traits.hpp"

namespace coolgui {

auto X11Traits::create(WindowConfig cfg) -> Handle {
  Display *display = XOpenDisplay(nullptr);
  if (display == nullptr) {
    LOGE("Failed to open X display");
    return Handle{};
  }

  i32 const screen = DefaultScreen(display);
  Colormap const colormap = DefaultColormap(display, screen);

  XWindow const window =
      XCreateSimpleWindow(display, RootWindow(display, screen), 0, 0, cfg.width.get(), cfg.height.get(),
                          /*border_width=*/1, BlackPixel(display, screen), WhitePixel(display, screen));

  // Set initial background color
  XColor xcolor{};
  xcolor.red = static_cast<unsigned short>(static_cast<i32>(cfg.background_color.red * 65535.0));
  xcolor.green = static_cast<unsigned short>(static_cast<i32>(cfg.background_color.green * 65535.0));
  xcolor.blue = static_cast<unsigned short>(static_cast<i32>(cfg.background_color.blue * 65535.0));
  xcolor.flags = DoRed | DoGreen | DoBlue;
  if (XAllocColor(display, colormap, &xcolor) != 0) {
    XSetWindowBackground(display, window, xcolor.pixel);
  }

  auto titleStr = std::string{cfg.title.get()};
  XStoreName(display, window, titleStr.c_str());

  // Register interest in window close events
  Atom const wm_delete = XInternAtom(display, "WM_DELETE_WINDOW", False);
  XSetWMProtocols(display, window, &wm_delete, 1);

  XSelectInput(display, window, ExposureMask | StructureNotifyMask | KeyPressMask);
  XMapWindow(display, window);
  XFlush(display);

  LOGI("X11 window created");
  return Handle{display, window, wm_delete, colormap};
}

auto X11Traits::destroy(Handle &handle) -> void {
  if (handle.display != nullptr) {
    XDestroyWindow(handle.display, handle.window);
    XCloseDisplay(handle.display);
    handle.display = nullptr;
    LOGI("X11 window destroyed");
  }
}

auto X11Traits::poll_event(Handle &handle) -> std::optional<Event> {
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
        .width = Width{static_cast<u32>(xev.xconfigure.width)},
        .height = Height{static_cast<u32>(xev.xconfigure.height)},
    }};
  }

  return std::nullopt;
}

auto X11Traits::set_background_color(Handle &handle, BackgroundColor color) -> void {
  if (handle.display == nullptr) {
    return;
  }

  XColor xcolor{};
  xcolor.red = static_cast<unsigned short>(static_cast<i32>(color.red * 65535.0));
  xcolor.green = static_cast<unsigned short>(static_cast<i32>(color.green * 65535.0));
  xcolor.blue = static_cast<unsigned short>(static_cast<i32>(color.blue * 65535.0));
  xcolor.flags = DoRed | DoGreen | DoBlue;
  if (XAllocColor(handle.display, handle.colormap, &xcolor) != 0) {
    XSetWindowBackground(handle.display, handle.window, xcolor.pixel);
    XClearWindow(handle.display, handle.window);
    XFlush(handle.display);
  }
}

} // namespace coolgui
