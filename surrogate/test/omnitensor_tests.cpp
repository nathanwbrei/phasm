

#include <tensor.hpp>
#include <catch.hpp>
#include <omnitensor.hpp>

namespace phasm {

TEST_CASE("OmnitensorBasic") {

    SECTION("1x1") {
        phasm::omnitensor t = phasm::omnitensor(phasm::DType::F32,{1},1);
        REQUIRE(t.length() == 1);
        REQUIRE(t.offset({0}) == 0);
        REQUIRE(t.offset({0}) < t.length());
    }
    SECTION("3x5") {
        phasm::omnitensor t = phasm::omnitensor(phasm::DType::F32,{3,5},1);
        REQUIRE(t.length() == 15);

        REQUIRE(t.offset({1,1}) == 6);
        REQUIRE(t.length({1,1}) == 1);

        REQUIRE(t.offset({2,4}) == 14);
        REQUIRE(t.length({2,4}) == 1);

        REQUIRE(t.offset({1}) == 5);
        REQUIRE(t.length({1}) == 5);

        REQUIRE(t.offset({2}) == 10);
        REQUIRE(t.length({2}) == 5);

        REQUIRE(t.offset({}) == 0);
        REQUIRE(t.length({}) == 15);
    }
    SECTION("2x3x5") {
        phasm::omnitensor t = phasm::omnitensor(phasm::DType::F32,{2,3,5},1);
        REQUIRE(t.length() == 30);
        REQUIRE(t.offset({0,1,1}) == 6);
        REQUIRE(t.length({0,1,1}) == 1);

        REQUIRE(t.offset({0,1}) == 5);
        REQUIRE(t.length({0,1}) == 5);

        REQUIRE(t.offset({0}) == 0);
        REQUIRE(t.length({0}) == 15);

        REQUIRE(t.offset({}) == 0);
        REQUIRE(t.length({}) == 30);

        REQUIRE(t.offset({1,1,1}) == 21);
        REQUIRE(t.length({1,1,1}) == 1);

        REQUIRE(t.offset({1,1}) == 20);
        REQUIRE(t.length({1,1}) == 5);

        REQUIRE(t.offset({1}) == 15);
        REQUIRE(t.length({1}) == 15);
    }
}

} // namespace phasm

