#pragma once

#include <optional>
#include <utility>

#include "coolgui/events.hpp"
#include "coolgui/log.hpp"
#include "coolgui/window_config.hpp"
#include "coolgui/window_traits.hpp"

namespace coolgui {

template <WindowTraits Traits> class BasicWindow {
  Traits traits_;
  Traits::Handle handle_;
  bool open_{true};

public:
  explicit constexpr BasicWindow(WindowConfig cfg, Traits traits = Traits{})
      : traits_{std::move(traits)}, handle_{[this, &cfg] -> auto { return traits_.create(cfg); }()} {
    LOGI("Window created: {}x{}", cfg.width.get(), cfg.height.get());
  }

  constexpr ~BasicWindow() {
    if (open_) {
      traits_.destroy(handle_);
    }
  }

  // Non-copyable, movable
  BasicWindow(const BasicWindow &) = delete;
  auto operator=(const BasicWindow &) -> BasicWindow & = delete;

  constexpr BasicWindow(BasicWindow &&other) noexcept
      : traits_(std::move(other.traits_)), handle_(std::move(other.handle_)), open_(other.open_) {
    other.open_ = false;
  }

  constexpr auto operator=(BasicWindow &&other) noexcept -> BasicWindow & {
    if (this != &other) {
      if (open_) {
        traits_.destroy(handle_);
      }
      traits_ = std::move(other.traits_);
      handle_ = std::move(other.handle_);
      open_ = other.open_;
      other.open_ = false;
    }
    return *this;
  }

  // Poll one pending event; returns std::nullopt when the queue is empty.
  [[nodiscard]] constexpr auto poll() -> std::optional<Event> {
    if (!open_) {
      return std::nullopt;
    }
    auto evt = traits_.poll_event(handle_);
    if (evt && std::holds_alternative<CloseEvent>(*evt)) {
      LOGI("Window close requested");
      open_ = false;
      traits_.destroy(handle_);
    }
    return evt;
  }

  // Run the event loop, calling handler(Event) for every event,
  // until a CloseEvent is received.
  constexpr auto run(auto &&handler) -> void {
    while (open_) {
      auto evt = poll();
      if (evt) {
        handler(*evt);
      }
    }
  }

  [[nodiscard]] constexpr auto is_open() const noexcept -> bool { return open_; }
  [[nodiscard]] constexpr auto traits() const noexcept -> const Traits & { return traits_; }
};

} // namespace coolgui
