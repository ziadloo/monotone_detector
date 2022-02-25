//
// Created by mehran on 2022-02-08.
//

#include "operation.h"

#include <utility>
#include <iostream>

operation::operation(int buffer_size) {
    sample_buffer.resize(buffer_size);
    spectrum_buffer.resize(buffer_size/2);
    this->start();
}

void operation::run() {
    while (!EXIT) {
        signal_ready.acquire();
        if (EXIT) {
            break;
        }
        guard.lock();
        std::vector<format> _sample_buffer(sample_buffer.size());
        std::vector<double> _spectrum_buffer(spectrum_buffer.size());

        copy(begin(sample_buffer), end(sample_buffer), begin(_sample_buffer));
        copy(begin(spectrum_buffer), end(spectrum_buffer), begin(_spectrum_buffer));
        guard.unlock();

        process(_sample_buffer, _spectrum_buffer);
    }
}

void operation::new_sample(std::vector<format> sample, std::vector<double> spectrum) {
    guard.lock();
    copy(begin(sample), end(sample), begin(sample_buffer));
    copy(begin(spectrum), end(spectrum), begin(spectrum_buffer));
    guard.unlock();
    signal_ready.release();
}
