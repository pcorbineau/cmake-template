#include <catch2/catch_test_macros.hpp>

#include "coolgui/widget_types.hpp"

TEST_CASE("WidgetRect contains point", "[WidgetRect]") {
  constexpr coolgui::WidgetRect r{.x = 10, .y = 20, .width = 100, .height = 50};

  STATIC_REQUIRE(r.contains(10, 20));
  STATIC_REQUIRE(r.contains(109, 69));
  STATIC_REQUIRE(r.contains(60, 45));
  STATIC_REQUIRE_FALSE(r.contains(9, 20));
  STATIC_REQUIRE_FALSE(r.contains(10, 19));
  STATIC_REQUIRE_FALSE(r.contains(110, 70));
  STATIC_REQUIRE_FALSE(r.contains(10, 70));
}

TEST_CASE("WidgetRect equality", "[WidgetRect]") {
  constexpr coolgui::WidgetRect a{.x = 1, .y = 2, .width = 3, .height = 4};
  constexpr coolgui::WidgetRect b{.x = 1, .y = 2, .width = 3, .height = 4};
  constexpr coolgui::WidgetRect c{.x = 5, .y = 2, .width = 3, .height = 4};

  STATIC_REQUIRE(a == b);
  STATIC_REQUIRE_FALSE(a == c);
}

TEST_CASE("WidgetRect zero-size rect contains no points", "[WidgetRect]") {
  constexpr coolgui::WidgetRect r{};

  STATIC_REQUIRE_FALSE(r.contains(0, 0));
  STATIC_REQUIRE_FALSE(r.contains(1, 0));
  STATIC_REQUIRE_FALSE(r.contains(0, 1));
}

TEST_CASE("WidgetRect contains negative coordinates", "[WidgetRect]") {
  constexpr coolgui::WidgetRect r{.x = -50, .y = -50, .width = 100, .height = 100};

  STATIC_REQUIRE(r.contains(-50, -50));
  STATIC_REQUIRE(r.contains(0, 0));
  STATIC_REQUIRE(r.contains(49, 49));
  STATIC_REQUIRE_FALSE(r.contains(-51, 0));
  STATIC_REQUIRE_FALSE(r.contains(0, 50));
}

TEST_CASE("MouseButton values", "[MouseButton]") {
  STATIC_REQUIRE(static_cast<int>(coolgui::MouseButton::Left) == 0);
  STATIC_REQUIRE(static_cast<int>(coolgui::MouseButton::Right) == 1);
  STATIC_REQUIRE(static_cast<int>(coolgui::MouseButton::Middle) == 2);
}
