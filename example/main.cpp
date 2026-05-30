#include "coolgui/coolgui.hpp"

namespace {
using coolgui::f64;

constexpr auto k_panel_layout() -> coolgui::ColorPanel::Layout {
  return {
      .panel_rect = {.x = 10, .y = 10, .width = 200, .height = 150},
      .value_label_rect = {.x = 20, .y = 18, .width = 180, .height = 25},
      .force_button_rect = {.x = 20, .y = 50, .width = 80, .height = 22},
      .text_r_rect = {.x = 25, .y = 85, .width = 50, .height = 22},
      .text_g_rect = {.x = 85, .y = 85, .width = 50, .height = 22},
      .text_b_rect = {.x = 145, .y = 85, .width = 50, .height = 22},
  };
}

class Visitor {
  coolgui::Window *window_;
  f64 width_;
  f64 height_;
  coolgui::ColorPanel color_panel_;

public:
  explicit Visitor(coolgui::Window &win, coolgui::Width w, coolgui::Height h)
      : window_{&win},
        width_{static_cast<f64>(w.get())},
        height_{static_cast<f64>(h.get())},
        color_panel_{k_panel_layout()} {
    window_->render_color_panel(color_panel_);
  }

  void operator()(const coolgui::CloseEvent & /*ev*/) const { coolgui::LOGI("Close event received — goodbye!"); }

  void operator()(const coolgui::ResizeEvent &ev) {
    width_ = static_cast<f64>(ev.width.get());
    height_ = static_cast<f64>(ev.height.get());
    coolgui::LOGI("Resize: {}x{}", ev.width.get(), ev.height.get());
  }

  void operator()(const coolgui::CursorMoveEvent &ev) {
    f64 const r = ev.x.get() / width_;
    f64 const g = ev.y.get() / height_;
    f64 const b = 1.0 - ((r + g) / 2.0);

    // Update panel values
    color_panel_.set_color({.red = r, .green = g, .blue = b});

    // Apply color: use forced values if force mode is on
    if (color_panel_.is_force_mode()) {
      window_->set_background_color(color_panel_.effective_color());
    } else {
      window_->set_background_color({.red = r, .green = g, .blue = b});
    }

    // Refresh overlay display
    window_->render_color_panel(color_panel_);
  }

  void operator()(const coolgui::MouseButtonEvent &ev) {
    color_panel_.handle_mouse_click(ev.x, ev.y);

    // If force mode was toggled on, apply the forced color immediately
    if (color_panel_.is_force_mode()) {
      window_->set_background_color(color_panel_.effective_color());
    }

    window_->render_color_panel(color_panel_);
  }

  void operator()(const coolgui::TextInputEvent &ev) {
    color_panel_.handle_text_input(ev.character);

    if (color_panel_.is_force_mode()) {
      window_->set_background_color(color_panel_.effective_color());
    }

    window_->render_color_panel(color_panel_);
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
