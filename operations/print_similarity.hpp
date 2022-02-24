//
// Created by mehran on 2022-02-07.
//

#ifndef ARECORD_PRINT_SIMILARITY_HPP
#define ARECORD_PRINT_SIMILARITY_HPP

#include <vector>
#include <functional>
#include "../pc/consumer.h"
#include <iostream>
#include "operation.h"

#include <algorithm>
#include <iostream>
#include <vector>
#include <cmath>

using std::vector;
using std::cout;
using std::endl;

class print_similarity: public operation {
public:
    print_similarity(int buffer_size): operation(buffer_size) {}

protected:
    void process(vector<format> sample, vector<double> spectrum) override {
        /*
        double spectrum_correlation = _consumer->correlation(&spectrum[0]);
        double spectrum_similarity = _consumer->cosine_similarity(&spectrum[0]);
        cout << "\r" << spectrum_correlation << " -- " << spectrum_similarity << "                     "
                << *std::max_element(begin(sample), end(sample)) << " -- " << *std::max_element(begin(sample), end(sample)) << "                     "
            << std::flush;
            */
    }
};

#endif //ARECORD_PRINT_SIMILARITY_HPP
