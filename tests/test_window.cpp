#include <catch2/catch_test_macros.hpp>
#include <tuple>
#include <variant>

#include "coolgui/window.hpp"
#include "mock_traits.hpp"

namespace {

constexpr coolgui::WindowConfig k_cfg{
    .width = coolgui::Width{800},
    .height = coolgui::Height{600},
    .title = coolgui::Title{"test"},
};

} // namespace

TEST_CASE("BasicWindow is open after construction", "[BasicWindow]") {
  const coolgui::BasicWindow<mock::Traits> win{k_cfg};
  REQUIRE(win.is_open());
}

TEST_CASE("BasicWindow poll returns nullopt when queue is empty", "[BasicWindow]") {
  coolgui::BasicWindow<mock::Traits> win{k_cfg, mock::Traits{mock::Scenario::Empty}};
  auto evt = win.poll();
  REQUIRE(!evt.has_value());
  REQUIRE(win.is_open());
}

TEST_CASE("BasicWindow closes and destroys on CloseEvent", "[BasicWindow]") {
  coolgui::BasicWindow<mock::Traits> win{k_cfg, mock::Traits{mock::Scenario::ThenClose}};

  auto evt = win.poll();
  auto evt_val = evt.value_or(coolgui::Event{coolgui::CloseEvent{}});
  REQUIRE(evt.has_value());
  REQUIRE(std::get_if<coolgui::CloseEvent>(&evt_val) != nullptr);
  REQUIRE(!win.is_open());
  REQUIRE(win.traits().destroy_count() == 1);
}

TEST_CASE("BasicWindow poll returns ResizeEvent", "[BasicWindow]") {
  coolgui::BasicWindow<mock::Traits> win{k_cfg, mock::Traits{mock::Scenario::ThenResize}};

  auto evt = win.poll();
  auto evt_val =
      evt.value_or(coolgui::Event{coolgui::ResizeEvent{.width = coolgui::Width{0}, .height = coolgui::Height{0}}});
  REQUIRE(evt.has_value());
  auto *resize = std::get_if<coolgui::ResizeEvent>(&evt_val);
  REQUIRE(resize != nullptr);
  REQUIRE(resize->width.get() == 1280U);
  REQUIRE(resize->height.get() == 720U);
  REQUIRE(win.is_open());
}

TEST_CASE("BasicWindow run calls handler and exits on close", "[BasicWindow]") {
  coolgui::BasicWindow<mock::Traits> win{k_cfg, mock::Traits{mock::Scenario::ThenClose}};

  int close_calls = 0;
  win.run([&](const coolgui::Event &event) -> void {
    if (std::holds_alternative<coolgui::CloseEvent>(event)) {
      ++close_calls;
    }
  });

  REQUIRE(close_calls == 1);
  REQUIRE(!win.is_open());
}

TEST_CASE("BasicWindow destructor does not double-destroy after close", "[BasicWindow]") {
  coolgui::BasicWindow<mock::Traits> win{k_cfg, mock::Traits{mock::Scenario::ThenClose}};
  std::ignore = win.poll(); // triggers CloseEvent → destroy called once, open_ = false
  REQUIRE(win.traits().destroy_count() == 1);
  // destructor runs here: open_ is false → destroy is NOT called again
}

TEST_CASE("BasicWindow set_background_color on open window", "[BasicWindow][BackgroundColor]") {
  coolgui::BasicWindow<mock::Traits> win{k_cfg};
  coolgui::BackgroundColor const expected{.red = 0.25, .green = 0.50, .blue = 0.75};
  win.set_background_color(expected);
  REQUIRE(win.traits().set_bg_color_count() == 1);
  auto const actual = win.traits().last_bg_color();
  REQUIRE(actual.red == expected.red);
  REQUIRE(actual.green == expected.green);
  REQUIRE(actual.blue == expected.blue);
}

TEST_CASE("BasicWindow set_background_color on closed window is no-op", "[BasicWindow][BackgroundColor]") {
  coolgui::BasicWindow<mock::Traits> win{k_cfg, mock::Traits{mock::Scenario::ThenClose}};
  std::ignore = win.poll(); // window is now closed
  REQUIRE_FALSE(win.is_open());

  win.set_background_color({.red = 1.0, .green = 0.0, .blue = 0.0});
  // set_background_color should not have been called on traits
  REQUIRE(win.traits().set_bg_color_count() == 0);
}

TEST_CASE("BasicWindow poll returns CursorMoveEvent", "[BasicWindow][CursorMove]") {
  coolgui::BasicWindow<mock::Traits> win{k_cfg, mock::Traits{mock::Scenario::ThenCursorMove}};

  auto evt = win.poll();
  REQUIRE(evt.has_value());
  auto evt_val =
      evt.value_or(coolgui::Event{coolgui::CursorMoveEvent{.x = coolgui::CursorX{0.0}, .y = coolgui::CursorY{0.0}}});
  auto *cursor = std::get_if<coolgui::CursorMoveEvent>(&evt_val);
  REQUIRE(cursor != nullptr);
  REQUIRE(cursor->x.get() == 100.0);
  REQUIRE(cursor->y.get() == 200.0);
  REQUIRE(win.is_open());
}

TEST_CASE("BasicWindow run dispatches CursorMoveEvent then CloseEvent", "[BasicWindow][CursorMove]") {
  coolgui::BasicWindow<mock::Traits> win{k_cfg, mock::Traits{mock::Scenario::ThenCursorMoveThenClose}};

  int cursor_calls = 0;
  win.run([&](const coolgui::Event &event) -> void {
    if (std::holds_alternative<coolgui::CursorMoveEvent>(event)) {
      ++cursor_calls;
    }
  });

  REQUIRE(cursor_calls == 1);
  REQUIRE_FALSE(win.is_open());
}
