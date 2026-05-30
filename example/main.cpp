#include "coolgui/coolgui.hpp"

namespace {
using coolgui::f64;

class Visitor {
  coolgui::Window *window_;
  f64 width_;
  f64 height_;

public:
  explicit Visitor(coolgui::Window &win, coolgui::Width w, coolgui::Height h)
      : window_{&win}, width_{static_cast<f64>(w.get())}, height_{static_cast<f64>(h.get())} {}

  void operator()(const coolgui::CloseEvent & /*ev*/) const { coolgui::LOGI("Close event received — goodbye!"); }
  void operator()(const coolgui::ResizeEvent &ev) {
    width_ = static_cast<f64>(ev.width.get());
    height_ = static_cast<f64>(ev.height.get());
    coolgui::LOGI("Resize: {}x{}", ev.width.get(), ev.height.get());
  }
  void operator()(const coolgui::CursorMoveEvent &ev) const {
    f64 const r = ev.x.get() / width_;
    f64 const g = ev.y.get() / height_;
    f64 const b = 1.0 - ((r + g) / 2.0);
    window_->set_background_color({.red = r, .green = g, .blue = b});
  }
};
} // namespace

auto main() -> int {
  try {
    coolgui::Window window{coolgui::WindowConfig{
        .width = coolgui::Width{800},
        .height = coolgui::Height{600},
        .title = coolgui::Title{"coolgui — Cursor Color"},
    }};

    Visitor visitor{window, coolgui::Width{800}, coolgui::Height{600}};
    window.run([&visitor](const coolgui::Event &event) -> void { std::visit(visitor, event); });

    return 0;
  } catch (...) {
    return 1;
  }
}
