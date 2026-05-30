#include <catch2/catch_test_macros.hpp>

#include "coolgui/strong_type.hpp"
#include "coolgui/window_types.hpp"

TEST_CASE("StrongType stores and retrieves its value", "[StrongType]")
{
    constexpr coolgui::Width w{ 800U };
    STATIC_REQUIRE(w.get() == 800U);

    constexpr coolgui::Height h{ 600U };
    STATIC_REQUIRE(h.get() == 600U);
}

TEST_CASE("StrongType get() on mutable instance returns reference", "[StrongType]")
{
    coolgui::Width w{ 100U };
    w.get() = 200U;
    REQUIRE(w.get() == 200U);
}

TEST_CASE("Width and Height are distinct types", "[StrongType]")
{
    STATIC_REQUIRE(!std::is_same_v<coolgui::Width, coolgui::Height>);
}
