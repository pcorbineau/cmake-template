#pragma once

#include <cstdint>
#include <optional>

#include "coolgui/events.hpp"
#include "coolgui/window_config.hpp"
#include "coolgui/window_types.hpp"

namespace mock {
using coolgui::i32;
using coolgui::u8;

struct Handle {
  i32 id{0};
};

enum class Scenario : u8 {
  Empty,                   // poll_event always returns nullopt
  ThenClose,               // poll_event returns CloseEvent once, then nullopt
  ThenResize,              // poll_event returns ResizeEvent once, then nullopt
  ThenCursorMove,          // poll_event returns CursorMoveEvent once, then nullopt
  ThenCursorMoveThenClose, // poll_event returns CursorMoveEvent, then CloseEvent, then nullopt
};

struct Traits {
  using Handle = mock::Handle;

  constexpr Traits() = default;
  constexpr Traits(Scenario s) : scenario_(s) {}

  static constexpr auto create(coolgui::WindowConfig /*cfg*/) -> Handle { return Handle{42}; }

  auto destroy(Handle & /*handle*/) -> void { ++destroy_count_; }

  auto poll_event(Handle & /*handle*/) -> std::optional<coolgui::Event> {
    ++poll_count_;
    switch (scenario_) {
    case Scenario::ThenClose:
      scenario_ = Scenario::Empty;
      return coolgui::Event{coolgui::CloseEvent{}};
    case Scenario::ThenResize:
      scenario_ = Scenario::Empty;
      return coolgui::Event{coolgui::ResizeEvent{
          .width = coolgui::Width{1280},
          .height = coolgui::Height{720},
      }};
    case Scenario::ThenCursorMove:
      scenario_ = Scenario::Empty;
      return coolgui::Event{coolgui::CursorMoveEvent{
          .x = coolgui::CursorX{100.0},
          .y = coolgui::CursorY{200.0},
      }};
    case Scenario::ThenCursorMoveThenClose:
      scenario_ = Scenario::ThenClose;
      return coolgui::Event{coolgui::CursorMoveEvent{
          .x = coolgui::CursorX{150.0},
          .y = coolgui::CursorY{250.0},
      }};
    case Scenario::Empty:
    default:
      return std::nullopt;
    }
  }

  auto set_background_color(Handle & /*handle*/, coolgui::BackgroundColor color) -> void {
    ++set_bg_color_count_;
    last_bg_color_ = color;
  }

  [[nodiscard]] constexpr auto poll_count() const noexcept -> i32 { return poll_count_; }
  [[nodiscard]] constexpr auto destroy_count() const noexcept -> i32 { return destroy_count_; }
  [[nodiscard]] constexpr auto set_bg_color_count() const noexcept -> i32 { return set_bg_color_count_; }
  [[nodiscard]] constexpr auto last_bg_color() const noexcept -> coolgui::BackgroundColor { return last_bg_color_; }

private:
  Scenario scenario_{Scenario::Empty};
  i32 poll_count_{0};
  i32 destroy_count_{0};
  i32 set_bg_color_count_{0};
  coolgui::BackgroundColor last_bg_color_{};
};

} // namespace mock
