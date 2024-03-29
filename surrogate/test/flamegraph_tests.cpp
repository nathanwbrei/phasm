
#include "flamegraph.hpp"
#include <catch.hpp>

TEST_CASE("Flame graph basics") {
    Flamegraph fg;
    fg.add("main;outer;inner 22");

    REQUIRE(fg.root->children[0]->symbol == "main");
    REQUIRE(fg.root->children[0]->own_sample_count == 0);
    REQUIRE(fg.root->children[0]->total_sample_count == 22);

    REQUIRE(fg.root->children[0]->children[0]->symbol == "outer");
    REQUIRE(fg.root->children[0]->children[0]->own_sample_count == 0);
    REQUIRE(fg.root->children[0]->children[0]->total_sample_count == 22);

    REQUIRE(fg.root->children[0]->children[0]->children[0]->symbol == "inner");
    REQUIRE(fg.root->children[0]->children[0]->children[0]->own_sample_count == 22);
    REQUIRE(fg.root->children[0]->children[0]->children[0]->total_sample_count == 22);

    fg.add("main;fun with spaces 33");
    REQUIRE(fg.root->children[0]->symbol == "main");
    REQUIRE(fg.root->children[0]->own_sample_count == 0);
    REQUIRE(fg.root->children[0]->total_sample_count == 55);

    REQUIRE(fg.root->children[0]->children[1]->symbol == "fun with spaces");
    REQUIRE(fg.root->children[0]->children[1]->own_sample_count == 33);
    REQUIRE(fg.root->children[0]->children[1]->total_sample_count == 33);

    fg.add("main 5");
    REQUIRE(fg.root->children[0]->symbol == "main");
    REQUIRE(fg.root->children[0]->own_sample_count == 5);
    REQUIRE(fg.root->children[0]->total_sample_count == 60);

    fg.printTree();
    fg.printFolded();
}

TEST_CASE("Flame graph filter by event loop") {
    Flamegraph fg;
    fg.add("main;outer;inner 200");
    fg.add("main;outer;tower;huh 50");
    fg.add("main;outer;tiny 1");
    fg.add("main;fun with spaces 33");
    fg.add("main;outer;inner;another 8");
    fg.add("main;meh 2");
    fg.add("main 1000");
    fg.add("main;fun with spaces;f 2");
    fg.filter("outer", 0.01, 0.95);

    std::cout << "Printing tree" << std::endl;
    fg.printTree();

    std::cout << std::endl << "Printing folded" << std::endl;
    fg.printFolded(false);

    std::cout << std::endl << "Printing candidates" << std::endl;
    fg.printCandidates();

    std::cout << std::endl << "Printing color palette" << std::endl;
    fg.printColorPalette();
}






