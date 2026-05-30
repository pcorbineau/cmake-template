#pragma once

#include <algorithm>
#include <cstdlib>
#include <string>
#include <string_view>

#include "coolgui/widget_types.hpp"
#include "coolgui/window_types.hpp"

namespace coolgui {

class ColorPanel {
public:
  enum class ActiveTextBox : u8 { None, R, G, B };

  struct Layout {
    WidgetRect panel_rect;
    WidgetRect value_label_rect;
    WidgetRect force_button_rect;
    WidgetRect text_r_rect;
    WidgetRect text_g_rect;
    WidgetRect text_b_rect;
  };

  explicit constexpr ColorPanel(Layout layout) : layout_{layout} {}

  void set_color(const BackgroundColor &color) {
    red_ = color.red;
    green_ = color.green;
    blue_ = color.blue;
  }

  [[nodiscard]] constexpr auto red() const noexcept -> f64 { return red_; }
  [[nodiscard]] constexpr auto green() const noexcept -> f64 { return green_; }
  [[nodiscard]] constexpr auto blue() const noexcept -> f64 { return blue_; }

  void set_force_mode(bool enabled) { force_mode_ = enabled; }
  [[nodiscard]] constexpr auto is_force_mode() const noexcept -> bool { return force_mode_; }

  void toggle_force_mode() { force_mode_ = !force_mode_; }

  void set_text_r(std::string_view text) { text_r_ = text; }
  void set_text_g(std::string_view text) { text_g_ = text; }
  void set_text_b(std::string_view text) { text_b_ = text; }

  [[nodiscard]] auto text_r() const noexcept -> const std::string & { return text_r_; }
  [[nodiscard]] auto text_g() const noexcept -> const std::string & { return text_g_; }
  [[nodiscard]] auto text_b() const noexcept -> const std::string & { return text_b_; }

  void handle_mouse_click(f64 x, f64 y) {
    if (layout_.force_button_rect.contains(x, y)) {
      toggle_force_mode();
      active_text_box_ = ActiveTextBox::None;
    } else if (layout_.text_r_rect.contains(x, y)) {
      active_text_box_ = ActiveTextBox::R;
      text_r_.clear();
    } else if (layout_.text_g_rect.contains(x, y)) {
      active_text_box_ = ActiveTextBox::G;
      text_g_.clear();
    } else if (layout_.text_b_rect.contains(x, y)) {
      active_text_box_ = ActiveTextBox::B;
      text_b_.clear();
    } else {
      active_text_box_ = ActiveTextBox::None;
    }
  }

  void handle_text_input(char c) {
    if (active_text_box_ == ActiveTextBox::None) { return; }
    auto &active = active_text();
    if (c == '\b') {
      if (!active.empty()) { active.pop_back(); }
    } else if (c == '\r' || c == '\n') {
      active_text_box_ = ActiveTextBox::None;
    } else if (c >= 32) {
      active.push_back(c);
    }
  }

  [[nodiscard]] auto effective_color() const noexcept -> BackgroundColor {
    auto parse = [](const std::string &s) -> f64 {
      if (s.empty()) { return 0.0; }
      char *end{};
      auto val = std::strtod(s.c_str(), &end);
      if (end == s.c_str()) { return 0.0; }
      return std::clamp(val, 0.0, 1.0);
    };
    if (force_mode_) {
      return {.red = parse(text_r_), .green = parse(text_g_), .blue = parse(text_b_)};
    }
    return {.red = red_, .green = green_, .blue = blue_};
  }

  [[nodiscard]] auto display_text() const -> std::string {
    auto fmt = [](f64 v) -> std::string {
      auto s = std::to_string(v);
      auto dot = s.find('.');
      if (dot != std::string::npos && s.size() > dot + 4) { s.resize(dot + 4); }
      return s;
    };
    return "R: " + fmt(red_) + " G: " + fmt(green_) + " B: " + fmt(blue_);
  }

  [[nodiscard]] constexpr auto layout() const noexcept -> const Layout & { return layout_; }

  [[nodiscard]] constexpr auto is_editing() const noexcept -> bool {
    return active_text_box_ != ActiveTextBox::None;
  }

  [[nodiscard]] constexpr auto active_text_box() const noexcept -> ActiveTextBox { return active_text_box_; }

private:
  [[nodiscard]] auto active_text() -> std::string & {
    switch (active_text_box_) {
    case ActiveTextBox::R: return text_r_;
    case ActiveTextBox::G: return text_g_;
    case ActiveTextBox::B: return text_b_;
    case ActiveTextBox::None: return text_r_;
    }
    return text_r_;
  }

  Layout layout_;
  f64 red_{};
  f64 green_{};
  f64 blue_{};
  bool force_mode_{false};
  ActiveTextBox active_text_box_{ActiveTextBox::None};
  std::string text_r_{"0"};
  std::string text_g_{"0"};
  std::string text_b_{"0"};
};

} // namespace coolgui
