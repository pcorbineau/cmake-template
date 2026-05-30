#pragma once

#include "coolgui/window.hpp"

#if defined(__APPLE__)
#include "coolgui/platform/macos_traits.hpp"
namespace coolgui {
using NativeTraits = MacosTraits;
}
#elif defined(__linux__)
#if defined(COOLGUI_USE_WAYLAND)
#include "coolgui/platform/wayland_traits.hpp"
namespace coolgui {
using NativeTraits = WaylandTraits;
}
#else
#include "coolgui/platform/linux_traits.hpp"
namespace coolgui {
using NativeTraits = LinuxTraits;
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
