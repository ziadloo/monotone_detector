//
// Created by mehran on 2022-02-08.
//

#include "raw_operation.h"

#include <utility>
#include <iostream>

raw_operation::raw_operation() {
    this->start();
}

void raw_operation::run() {
    while (!EXIT) {
        signal_ready.acquire();
        if (EXIT) {
            break;
        }
        guard.lock();
        process();
        guard.unlock();
    }
}

void raw_operation::new_sample(const std::vector<format>& sample) {
    guard.lock();
    sample_history.emplace_back(sample);
    while (sample_history.size() > 100) {
        sample_history.pop_front();
    }
    guard.unlock();
    signal_ready.release();
}
