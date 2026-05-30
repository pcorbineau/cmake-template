#include "coolgui/coolgui.hpp"

struct Visitor {
  constexpr auto operator()(const coolgui::CloseEvent &) { coolgui::LOGI("Close event received — goodbye!"); }
  constexpr auto operator()(const coolgui::ResizeEvent &ev) {
    coolgui::LOGI("Resize: {}x{}", ev.width.get(), ev.height.get());
  }
};

int main() {
  coolgui::Window window{coolgui::WindowConfig{
      .width = coolgui::Width{800},
      .height = coolgui::Height{600},
      .title = coolgui::Title{"coolgui — Hello Window"},
  }};

  window.run([](const coolgui::Event &event) { std::visit(Visitor{}, event); });

  return 0;
}
