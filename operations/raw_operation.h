//
// Created by mehran on 2022-02-08.
//

#ifndef ARECORD_RAW_OPERATION_H
#define ARECORD_RAW_OPERATION_H

#include <mutex>
#include "../lib/semaphore.hpp"
#include <vector>
#include <list>
#include <thread>
#include "../utility.h"

class raw_operation {
public:
    raw_operation();
    void new_sample(const std::vector<format>& sample);
    void stop() { EXIT = true; signal_ready.release(); process_runner->join(); }

protected:
    virtual void process() = 0;
    std::mutex guard;
    std::list<std::vector<format>> sample_history;

private:
    bool EXIT = false;
    semaphore signal_ready;
    std::unique_ptr<std::thread> process_runner;
};


#endif //ARECORD_RAW_OPERATION_H
