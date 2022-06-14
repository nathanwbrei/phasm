
// Copyright 2022, Jefferson Science Associates, LLC.
// Subject to the terms in the LICENSE file found in the top-level directory.


#include <catch.hpp>
#include "surrogate.h"
#include "sampler.h"

using namespace phasm;

namespace phasm::tests::sampling_tests {


TEST_CASE("Basic GridSampler") {

    int x = 100;
    auto i = std::make_shared<ModelVariable>();
    i->range.lower_bound_inclusive = 3;
    i->range.upper_bound_inclusive = 5;
    CallSiteVariable cs("x", any_ptr((int*) nullptr));
    cs.model_vars.push_back(i);
    cs.binding = &x;
    GridSampler<int> s(cs, i);

    bool result;
    result = s.next();
    REQUIRE(result == true);
    REQUIRE(x == 3);

    result = s.next();
    REQUIRE(result == true);
    REQUIRE(x == 4);

    result = s.next();
    REQUIRE(result == false);
    REQUIRE(x == 5);

    result = s.next();
    REQUIRE(result == true); // Wraps around
    REQUIRE(x == 3);
}


TEST_CASE("Basic FiniteSetSampler") {
    int x = 100;
    auto i = std::make_shared<ModelVariable>();
    i->range.items = {7,8,9};
    CallSiteVariable b("x", any_ptr((int*) nullptr));
    b.model_vars.push_back(i);
    b.binding = &x;
    FiniteSetSampler<int> s(b);

    bool result;
    result = s.next();
    REQUIRE(result == true);
    REQUIRE(x == 7);

    result = s.next();
    REQUIRE(result == true);
    REQUIRE(x == 8);

    result = s.next();
    REQUIRE(result == false);
    REQUIRE(x == 9);

    result = s.next();
    REQUIRE(result == true); // Wraps around
    REQUIRE(x == 7);
}

} // namespace phasm::test::sampling_tests