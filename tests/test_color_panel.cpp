#include <catch2/catch_test_macros.hpp>

#include "coolgui/color_panel.hpp"

namespace {
constexpr auto make_layout() -> coolgui::ColorPanel::Layout {
  return {
      .panel_rect = {.x = 10, .y = 10, .width = 200, .height = 150},
      .value_label_rect = {.x = 15, .y = 15, .width = 190, .height = 30},
      .force_button_rect = {.x = 15, .y = 50, .width = 100, .height = 25},
      .text_r_rect = {.x = 15, .y = 85, .width = 50, .height = 20},
      .text_g_rect = {.x = 75, .y = 85, .width = 50, .height = 20},
      .text_b_rect = {.x = 135, .y = 85, .width = 50, .height = 20},
  };
}
} // namespace

TEST_CASE("ColorPanel defaults", "[ColorPanel]") {
  const coolgui::ColorPanel panel{make_layout()};

  REQUIRE(panel.red() == 0.0);
  REQUIRE(panel.green() == 0.0);
  REQUIRE(panel.blue() == 0.0);
  REQUIRE_FALSE(panel.is_force_mode());
  REQUIRE_FALSE(panel.is_editing());
  REQUIRE(panel.text_r() == "0");
  REQUIRE(panel.text_g() == "0");
  REQUIRE(panel.text_b() == "0");
}

TEST_CASE("set_color updates values", "[ColorPanel]") {
  coolgui::ColorPanel panel{make_layout()};
  panel.set_color({.red = 0.5, .green = 0.3, .blue = 0.8});

  REQUIRE(panel.red() == 0.5);
  REQUIRE(panel.green() == 0.3);
  REQUIRE(panel.blue() == 0.8);
}

TEST_CASE("toggle_force_mode", "[ColorPanel]") {
  coolgui::ColorPanel panel{make_layout()};

  REQUIRE_FALSE(panel.is_force_mode());
  panel.toggle_force_mode();
  REQUIRE(panel.is_force_mode());
  panel.toggle_force_mode();
  REQUIRE_FALSE(panel.is_force_mode());
}

TEST_CASE("set_force_mode", "[ColorPanel]") {
  coolgui::ColorPanel panel{make_layout()};

  panel.set_force_mode(true);
  REQUIRE(panel.is_force_mode());
  panel.set_force_mode(false);
  REQUIRE_FALSE(panel.is_force_mode());
}

TEST_CASE("mouse click on force button toggles mode", "[ColorPanel]") {
  coolgui::ColorPanel panel{make_layout()};

  panel.handle_mouse_click(50, 60);

  REQUIRE(panel.is_force_mode());
  REQUIRE_FALSE(panel.is_editing());
}

TEST_CASE("mouse click on force button toggles back", "[ColorPanel]") {
  coolgui::ColorPanel panel{make_layout()};
  panel.handle_mouse_click(50, 60);
  panel.handle_mouse_click(50, 60);

  REQUIRE_FALSE(panel.is_force_mode());
}

TEST_CASE("mouse click on R text box enters edit mode", "[ColorPanel]") {
  coolgui::ColorPanel panel{make_layout()};

  panel.handle_mouse_click(25, 95);

  REQUIRE(panel.is_editing());
  REQUIRE(panel.text_r().empty());
}

TEST_CASE("mouse click on G text box enters edit mode", "[ColorPanel]") {
  coolgui::ColorPanel panel{make_layout()};

  panel.handle_mouse_click(85, 95);

  REQUIRE(panel.is_editing());
  REQUIRE(panel.text_g().empty());
}

TEST_CASE("mouse click on B text box enters edit mode", "[ColorPanel]") {
  coolgui::ColorPanel panel{make_layout()};

  panel.handle_mouse_click(145, 95);

  REQUIRE(panel.is_editing());
  REQUIRE(panel.text_b().empty());
}

TEST_CASE("text input when editing R appends characters", "[ColorPanel]") {
  coolgui::ColorPanel panel{make_layout()};

  panel.handle_mouse_click(25, 95);
  panel.handle_text_input('0');
  panel.handle_text_input('.');
  panel.handle_text_input('5');

  REQUIRE(panel.text_r() == "0.5");
}

TEST_CASE("text input when editing G appends characters", "[ColorPanel]") {
  coolgui::ColorPanel panel{make_layout()};

  panel.handle_mouse_click(85, 95);
  panel.handle_text_input('0');
  panel.handle_text_input('.');
  panel.handle_text_input('3');

  REQUIRE(panel.text_g() == "0.3");
}

TEST_CASE("text input when editing B appends characters", "[ColorPanel]") {
  coolgui::ColorPanel panel{make_layout()};

  panel.handle_mouse_click(145, 95);
  panel.handle_text_input('0');
  panel.handle_text_input('.');
  panel.handle_text_input('8');

  REQUIRE(panel.text_b() == "0.8");
}

TEST_CASE("backspace removes last character", "[ColorPanel]") {
  coolgui::ColorPanel panel{make_layout()};

  panel.handle_mouse_click(25, 95);
  panel.handle_text_input('0');
  panel.handle_text_input('.');
  panel.handle_text_input('5');
  panel.handle_text_input('\b');

  REQUIRE(panel.text_r() == "0.");
}

TEST_CASE("backspace on empty string is no-op", "[ColorPanel]") {
  coolgui::ColorPanel panel{make_layout()};

  panel.handle_mouse_click(25, 95);
  panel.handle_text_input('\b');

  REQUIRE(panel.text_r().empty());
}

TEST_CASE("Enter key exits edit mode", "[ColorPanel]") {
  coolgui::ColorPanel panel{make_layout()};

  panel.handle_mouse_click(25, 95);
  panel.handle_text_input('0');
  panel.handle_text_input('.');
  panel.handle_text_input('5');
  panel.handle_text_input('\r');

  REQUIRE_FALSE(panel.is_editing());
  REQUIRE(panel.text_r() == "0.5");
}

TEST_CASE("text input when not editing is ignored", "[ColorPanel]") {
  coolgui::ColorPanel panel{make_layout()};

  panel.handle_text_input('x');

  REQUIRE(panel.text_r() == "0");
  REQUIRE(panel.text_g() == "0");
  REQUIRE(panel.text_b() == "0");
}

TEST_CASE("mouse click outside all widgets exits edit mode", "[ColorPanel]") {
  coolgui::ColorPanel panel{make_layout()};

  panel.handle_mouse_click(25, 95);
  REQUIRE(panel.is_editing());

  panel.handle_mouse_click(0, 0);

  REQUIRE_FALSE(panel.is_editing());
}

TEST_CASE("effective_color returns stored color when force is off", "[ColorPanel]") {
  coolgui::ColorPanel panel{make_layout()};
  panel.set_color({.red = 0.2, .green = 0.4, .blue = 0.6});

  auto const c = panel.effective_color();
  REQUIRE(c.red == 0.2);
  REQUIRE(c.green == 0.4);
  REQUIRE(c.blue == 0.6);
}

TEST_CASE("effective_color parses text boxes when force is on", "[ColorPanel]") {
  coolgui::ColorPanel panel{make_layout()};
  panel.set_force_mode(true);
  panel.set_text_r("0.1");
  panel.set_text_g("0.3");
  panel.set_text_b("0.5");

  auto const c = panel.effective_color();
  REQUIRE(c.red == 0.1);
  REQUIRE(c.green == 0.3);
  REQUIRE(c.blue == 0.5);
}

TEST_CASE("effective_color clamps values to [0,1]", "[ColorPanel]") {
  coolgui::ColorPanel panel{make_layout()};
  panel.set_force_mode(true);
  panel.set_text_r("2.5");
  panel.set_text_g("-1.0");
  panel.set_text_b("0.5");

  auto const c = panel.effective_color();
  REQUIRE(c.red == 1.0);
  REQUIRE(c.green == 0.0);
  REQUIRE(c.blue == 0.5);
}

TEST_CASE("effective_color handles invalid text", "[ColorPanel]") {
  coolgui::ColorPanel panel{make_layout()};
  panel.set_force_mode(true);
  panel.set_text_r("abc");
  panel.set_text_g("");
  panel.set_text_b("1.5");

  auto const c = panel.effective_color();
  REQUIRE(c.red == 0.0);
  REQUIRE(c.green == 0.0);
  REQUIRE(c.blue == 1.0);
}

TEST_CASE("display_text format", "[ColorPanel]") {
  coolgui::ColorPanel panel{make_layout()};
  panel.set_color({.red = 0.25, .green = 0.5, .blue = 0.75});

  auto const text = panel.display_text();
  REQUIRE(text.contains("0.25"));
  REQUIRE(text.contains("0.5"));
  REQUIRE(text.contains("0.75"));
}

TEST_CASE("set_text_r/g/b and retrieval", "[ColorPanel]") {
  coolgui::ColorPanel panel{make_layout()};
  panel.set_text_r("0.42");
  panel.set_text_g("0.84");
  panel.set_text_b("0.21");

  REQUIRE(panel.text_r() == "0.42");
  REQUIRE(panel.text_g() == "0.84");
  REQUIRE(panel.text_b() == "0.21");
}

TEST_CASE("layout is stored correctly", "[ColorPanel]") {
  auto const layout = make_layout();
  const coolgui::ColorPanel panel{layout};

  auto const stored = panel.layout();
  REQUIRE(stored.panel_rect == layout.panel_rect);
  REQUIRE(stored.force_button_rect == layout.force_button_rect);
  REQUIRE(stored.text_r_rect == layout.text_r_rect);
  REQUIRE(stored.text_g_rect == layout.text_g_rect);
  REQUIRE(stored.text_b_rect == layout.text_b_rect);
  REQUIRE(stored.value_label_rect == layout.value_label_rect);
}

TEST_CASE("multiple text inputs across different boxes", "[ColorPanel]") {
  coolgui::ColorPanel panel{make_layout()};

  panel.handle_mouse_click(25, 95);
  panel.handle_text_input('0');
  panel.handle_text_input('.');
  panel.handle_text_input('3');
  panel.handle_text_input('\r');

  panel.handle_mouse_click(85, 95);
  panel.handle_text_input('0');
  panel.handle_text_input('.');
  panel.handle_text_input('6');
  panel.handle_text_input('\r');

  panel.handle_mouse_click(145, 95);
  panel.handle_text_input('0');
  panel.handle_text_input('.');
  panel.handle_text_input('9');
  panel.handle_text_input('\r');

  REQUIRE(panel.text_r() == "0.3");
  REQUIRE(panel.text_g() == "0.6");
  REQUIRE(panel.text_b() == "0.9");
  REQUIRE_FALSE(panel.is_editing());
}

TEST_CASE("control characters below 32 are filtered", "[ColorPanel]") {
  coolgui::ColorPanel panel{make_layout()};

  panel.handle_mouse_click(25, 95);
  panel.handle_text_input(static_cast<char>(1));
  panel.handle_text_input('5');

  REQUIRE(panel.text_r() == "5");
}
