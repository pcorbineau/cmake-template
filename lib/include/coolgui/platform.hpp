#pragma once

#include "coolgui/window.hpp"

#ifdef __APPLE__
#include "coolgui/platform/macos_traits.hpp"
namespace coolgui {
using NativeTraits = MacosTraits;
}
#elifdef __linux__
#if defined(COOLGUI_USE_WAYLAND)
#include "coolgui/platform/wayland_traits.hpp"
namespace coolgui {
using NativeTraits = WaylandTraits;
}
#else
#include "coolgui/platform/x11_traits.hpp"
namespace coolgui {
using NativeTraits = X11Traits;
}
#endif
#elif defined(_WIN32)
#include "coolgui/platform/windows_traits.hpp"
namespace coolgui {
using NativeTraits = WindowsTraits;
}
#else
#error "Unsupported platform"
#endif

namespace coolgui {
using Window = BasicWindow<NativeTraits>;
} // namespace coolgui
