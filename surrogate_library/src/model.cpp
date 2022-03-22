
// Copyright 2022, Jefferson Science Associates, LLC.
// Subject to the terms in the LICENSE file found in the top-level directory.

#include "model.h"
#include <iostream>

void Model::dump_captures_to_csv(std::ostream& os) {
    // print column header
    for (auto input : inputs) {
        std::cout << input->name << ", ";
    }
    for (size_t i=0; i<outputs.size(); ++i) {
        std::cout << outputs[i]->name;
        if (i < (outputs.size()-1)) std::cout << ", ";
    }
    std::cout << std::endl;

    // print body
    for (size_t i=0; i<captured_rows; ++i) {
        for (size_t j=0; j<inputs.size(); ++j) {
            inputs[j]->stringify(std::cout, i);
            std::cout << ", ";
        }
        for (size_t j=0; j<outputs.size(); ++j) {
            outputs[j]->stringify(std::cout, i);
            if (j < (outputs.size()-1)) std::cout << ", ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}