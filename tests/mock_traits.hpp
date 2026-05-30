#pragma once

#include <cstdint>
#include <optional>

#include "coolgui/events.hpp"
#include "coolgui/window_config.hpp"
#include "coolgui/window_types.hpp"

namespace mock {

struct Handle {
  int id{0};
};

enum class Scenario : std::uint8_t {
  Empty,      // poll_event always returns nullopt
  ThenClose,  // poll_event returns CloseEvent once, then nullopt
  ThenResize, // poll_event returns ResizeEvent once, then nullopt
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
    case Scenario::Empty:
    default:
      return std::nullopt;
    }
  }

  [[nodiscard]] constexpr auto poll_count() const noexcept -> int { return poll_count_; }
  [[nodiscard]] constexpr auto destroy_count() const noexcept -> int { return destroy_count_; }

private:
  Scenario scenario_{Scenario::Empty};
  int poll_count_{0};
  int destroy_count_{0};
};

} // namespace mock
