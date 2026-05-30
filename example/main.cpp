#include "coolgui/coolgui.hpp"

namespace {
struct Visitor {
  constexpr auto operator()(const coolgui::CloseEvent & /*ev*/) { coolgui::LOGI("Close event received — goodbye!"); }
  constexpr auto operator()(const coolgui::ResizeEvent &ev) {
    coolgui::LOGI("Resize: {}x{}", ev.width.get(), ev.height.get());
  }
};
} // namespace

auto main() -> int {
  try {
    coolgui::Window window{coolgui::WindowConfig{
        .width = coolgui::Width{800},
        .height = coolgui::Height{600},
        .title = coolgui::Title{"coolgui — Hello Window"},
    }};

    window.run([](const coolgui::Event &event) -> void { std::visit(Visitor{}, event); });

    return 0;
  } catch (...) {
    return 1;
  }
}
