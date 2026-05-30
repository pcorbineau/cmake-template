#include "coolgui/platform/windows_traits.hpp"

#include <string>

#include "coolgui/log.hpp"
#include "coolgui/window_types.hpp"

namespace coolgui {

// Store a pointer to the handle in the window's user data so the WndProc
// can mark close_requested without needing a global.
static LRESULT CALLBACK wnd_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
  if (msg == WM_NCCREATE) {
    auto *cs = reinterpret_cast<CREATESTRUCT *>(lparam);
    SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(cs->lpCreateParams));
    return DefWindowProcW(hwnd, msg, wparam, lparam);
  }

  auto *handle = reinterpret_cast<WindowsHandle *>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));

  switch (msg) {
  case WM_CLOSE:
    if (handle != nullptr) {
      handle->close_requested = true;
    }
    return 0;
  case WM_DESTROY:
    PostQuitMessage(0);
    return 0;
  default:
    return DefWindowProcW(hwnd, msg, wparam, lparam);
  }
}

auto WindowsTraits::create(WindowConfig cfg) -> Handle {
  static constexpr const wchar_t *kClassName = L"CoolguiWindow";

  HINSTANCE hinstance = GetModuleHandleW(nullptr);

  WNDCLASSEXW wc{};
  wc.cbSize = sizeof(wc);
  wc.style = CS_HREDRAW | CS_VREDRAW;
  wc.lpfnWndProc = wnd_proc;
  wc.hInstance = hinstance;
  wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
  wc.lpszClassName = kClassName;
  RegisterClassExW(&wc);

  Handle handle{};

  // Convert title from UTF-8 string_view to UTF-16
  auto titleUtf8 = std::string{cfg.title.get()};
  i32 wlen = MultiByteToWideChar(CP_UTF8, 0, titleUtf8.c_str(), -1, nullptr, 0);
  auto titleW = std::wstring(static_cast<usize>(wlen), L'\0');
  MultiByteToWideChar(CP_UTF8, 0, titleUtf8.c_str(), -1, titleW.data(), wlen);

  RECT rect{0, 0, static_cast<LONG>(cfg.width.get()), static_cast<LONG>(cfg.height.get())};
  AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

  handle.hwnd = CreateWindowExW(0, kClassName, titleW.c_str(), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
                                rect.right - rect.left, rect.bottom - rect.top, nullptr, nullptr, hinstance, &handle);

  if (handle.hwnd == nullptr) {
    LOGE("CreateWindowExW failed");
    return handle;
  }

  ShowWindow(handle.hwnd, SW_SHOWDEFAULT);
  UpdateWindow(handle.hwnd);
  LOGI("Win32 window created");
  return handle;
}

auto WindowsTraits::destroy(Handle &handle) -> void {
  if (handle.hwnd != nullptr) {
    DestroyWindow(handle.hwnd);
    handle.hwnd = nullptr;
    LOGI("Win32 window destroyed");
  }
}

auto WindowsTraits::poll_event(Handle &handle) -> std::optional<Event> {
  if (handle.hwnd == nullptr) {
    return std::nullopt;
  }

  if (handle.close_requested) {
    return Event{CloseEvent{}};
  }

  MSG msg{};
  if (PeekMessageW(&msg, handle.hwnd, 0, 0, PM_REMOVE) != 0) {
    TranslateMessage(&msg);
    DispatchMessageW(&msg);

    if (msg.message == WM_SIZE) {
      return Event{ResizeEvent{
          Width{static_cast<u32>(LOWORD(msg.lParam))},
          Height{static_cast<u32>(HIWORD(msg.lParam))},
      }};
    }
  }

  return std::nullopt;
}

} // namespace coolgui
