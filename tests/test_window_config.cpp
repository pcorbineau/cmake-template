#include <catch2/catch_test_macros.hpp>

#include "coolgui/window_config.hpp"
#include "coolgui/window_types.hpp"

TEST_CASE("WindowConfig can be constructed with strong types", "[WindowConfig]")
{
    constexpr coolgui::WindowConfig cfg{
        .width  = coolgui::Width{ 1920 },
        .height = coolgui::Height{ 1080 },
        .title  = coolgui::Title{ "test" },
    };
    STATIC_REQUIRE(cfg.width.get() == 1920U);
    STATIC_REQUIRE(cfg.height.get() == 1080U);
    STATIC_REQUIRE(cfg.title.get() == "test");
}
