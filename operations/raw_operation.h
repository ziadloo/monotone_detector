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
#include <QThread>
#include "../utility.h"

class raw_operation: public QThread {
Q_OBJECT
public:
    raw_operation();
    void new_sample(const std::vector<format>& sample);
    void stop() { EXIT = true; signal_ready.release(); QThread::quit(); }

protected:
    void run() override;
    virtual void process() = 0;
    std::mutex guard;
    std::list<std::vector<format>> sample_history;

private:
    bool EXIT = false;
    semaphore signal_ready;
};


#endif //ARECORD_RAW_OPERATION_H
