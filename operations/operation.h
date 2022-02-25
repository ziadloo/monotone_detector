//
// Created by mehran on 2022-02-08.
//

#ifndef ARECORD_OPERATION_H
#define ARECORD_OPERATION_H

#include <mutex>
#include <vector>
#include "../lib/semaphore.hpp"
#include "../utility.h"
#include <thread>
#include <QThread>

class operation: public QThread {
Q_OBJECT
public:
    operation(int buffer_size);
    void new_sample(std::vector<format> sample, std::vector<double> spectrum);
    void stop() { EXIT = true; signal_ready.release(); QThread::quit(); }

protected:
    void run() override;
    virtual void process(std::vector<format> sample, std::vector<double> spectrum) = 0;

private:
    bool EXIT = false;
    std::vector<format> sample_buffer;
    std::vector<double> spectrum_buffer;
    std::mutex guard;
    semaphore signal_ready;
};


#endif //ARECORD_OPERATION_H
