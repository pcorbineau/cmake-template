#include <catch2/catch_test_macros.hpp>
#include <mylib/mylib.hpp>

TEST_CASE("Test add function", "[add]") {
  REQUIRE(mylib::add(2, 3) == 5);
  REQUIRE(mylib::add(-1, 1) == 0);
  REQUIRE(mylib::add(0, 0) == 0);
}

TEST_CASE("Test add_constexpr2 function", "[add_constexpr2]") {
  REQUIRE(mylib::add_constexpr2(2, 3) == 5);
  REQUIRE(mylib::add_constexpr2(-1, 1) == 0);
  REQUIRE(mylib::add_constexpr2(0, 0) == 0);
}

TEST_CASE("Test add_inline2 function", "[add_inline2]") {
  REQUIRE(mylib::add_inline2(2, 3) == 5);
  REQUIRE(mylib::add_inline2(-1, 1) == 0);
  REQUIRE(mylib::add_inline2(0, 0) == 0);
}

TEST_CASE("Test to_string function", "[to_string]") {
  REQUIRE(mylib::to_string(std::numeric_limits<std::uint64_t>::max()) == "18446744073709551615");
}
