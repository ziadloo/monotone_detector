//
// Created by mehran on 2022-02-06.
//

#ifndef ARECORD_CONSUMER_H
#define ARECORD_CONSUMER_H

#include <vector>
#include <functional>
#include <fftw3.h>
#include "lib/queue/readerwriterqueue.h"
#include "../utility.h"
#include "task.h"
#include "../operations/operation.h"
#include "../operations/raw_operation.h"
#include "./producer.h"

using namespace std;
using namespace moodycamel;

class consumer: public task {
public:
    consumer(shared_ptr<producer> p, int sample_rate, int num_per_points);
    double amplitude(fftw_complex const result) const;
    double magnitude(fftw_complex const result) const;
    void stop() override;

    std::vector<operation*> operations;
    std::vector<raw_operation*> raw_operations;

protected:
    void thread_code() override;

private:
    int NUM_POINTS;
    shared_ptr<producer> pro;
};


#endif //ARECORD_CONSUMER_H
