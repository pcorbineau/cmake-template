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
  coolgui::BasicWindow<mock::Traits> win{k_cfg};
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
  REQUIRE(evt.has_value());
  REQUIRE(std::holds_alternative<coolgui::CloseEvent>(*evt));
  REQUIRE(!win.is_open());
  REQUIRE(win.traits().destroy_count == 1);
}

TEST_CASE("BasicWindow poll returns ResizeEvent", "[BasicWindow]") {
  coolgui::BasicWindow<mock::Traits> win{k_cfg, mock::Traits{mock::Scenario::ThenResize}};

  auto evt = win.poll();
  REQUIRE(evt.has_value());
  REQUIRE(std::holds_alternative<coolgui::ResizeEvent>(*evt));
  auto &resize = std::get<coolgui::ResizeEvent>(*evt);
  REQUIRE(resize.width.get() == 1280U);
  REQUIRE(resize.height.get() == 720U);
  REQUIRE(win.is_open());
}

TEST_CASE("BasicWindow run calls handler and exits on close", "[BasicWindow]") {
  coolgui::BasicWindow<mock::Traits> win{k_cfg, mock::Traits{mock::Scenario::ThenClose}};

  int close_calls = 0;
  win.run([&](const coolgui::Event &event) {
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
  REQUIRE(win.traits().destroy_count == 1);
  // destructor runs here: open_ is false → destroy is NOT called again
}
