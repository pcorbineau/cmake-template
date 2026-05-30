#include "coolgui/platform/windows_traits.hpp"

#include <bit>
#include <string>

#include "coolgui/log.hpp"
#include "coolgui/window_types.hpp"

namespace coolgui {
namespace {

auto wnd_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) -> LRESULT {
  if (msg == WM_NCCREATE) {
    auto *cs = std::bit_cast<CREATESTRUCT *>(lparam);
    SetWindowLongPtrW(hwnd, GWLP_USERDATA, std::bit_cast<LONG_PTR>(cs->lpCreateParams));
    return DefWindowProcW(hwnd, msg, wparam, lparam);
  }

  auto *handle = std::bit_cast<WindowsHandle *>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));

  switch (msg) {
  case WM_CLOSE:
    if (handle != nullptr) {
      handle->event_queue.emplace_back(coolgui::CloseEvent{});
    }
    return 0;
  case WM_DESTROY:
    PostQuitMessage(0);
    return 0;
  case WM_ERASEBKGND:
    if (handle != nullptr) {
      RECT rc{};
      GetClientRect(hwnd, &rc);
      COLORREF const cref =
          RGB(static_cast<BYTE>(handle->bg_color.red * 255.0), static_cast<BYTE>(handle->bg_color.green * 255.0),
              static_cast<BYTE>(handle->bg_color.blue * 255.0));
      HBRUSH const brush = CreateSolidBrush(cref);
      FillRect(reinterpret_cast<HDC>(wparam), &rc, brush);
      DeleteObject(brush);
      return 1;
    }
    return DefWindowProcW(hwnd, msg, wparam, lparam);
  default:
    return DefWindowProcW(hwnd, msg, wparam, lparam);
  }
}

} // namespace

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
  handle.bg_color = cfg.background_color;

  // Convert title from UTF-8 string_view to UTF-16
  auto titleUtf8 = std::string{cfg.title.get()};
  i32 const wlen = MultiByteToWideChar(CP_UTF8, 0, titleUtf8.c_str(), -1, nullptr, 0);
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

  // Pop queued events first (pushed by wnd_proc callbacks)
  if (!handle.event_queue.empty()) {
    auto evt = handle.event_queue.front();
    handle.event_queue.erase(handle.event_queue.begin());
    return evt;
  }

  MSG msg{};
  if (PeekMessageW(&msg, handle.hwnd, 0, 0, PM_REMOVE) != 0) {
    TranslateMessage(&msg);
    DispatchMessageW(&msg);

    // After dispatching, the callback may have pushed to the queue
    if (!handle.event_queue.empty()) {
      auto evt = handle.event_queue.front();
      handle.event_queue.erase(handle.event_queue.begin());
      return evt;
    }

    if (msg.message == WM_SIZE) {
      return Event{ResizeEvent{
          .width = Width{static_cast<u32>(LOWORD(msg.lParam))},
          .height = Height{static_cast<u32>(HIWORD(msg.lParam))},
      }};
    }
  }

  return std::nullopt;
}

auto WindowsTraits::set_background_color(Handle &handle, BackgroundColor color) -> void {
  handle.bg_color = color;
  if (handle.hwnd != nullptr) {
    InvalidateRect(handle.hwnd, nullptr, TRUE);
  }
}

} // namespace coolgui
