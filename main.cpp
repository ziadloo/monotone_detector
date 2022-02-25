#include <QApplication>
#include "FrequencyPlotter.hpp"

#include <iostream>
#include <csignal>
#include <unistd.h>
#include <algorithm>
#include "lib/queue/readerwriterqueue.h"
#include "utility.h"
#include "pc/producer.h"
#include "pc/consumer.h"
#include "operations/buffer_and_save.hpp"
#include "operations/noise_detector.hpp"
#include <chrono>

using namespace moodycamel;
using namespace std;

typedef std::chrono::high_resolution_clock Time;
typedef std::chrono::duration<float> fsec;

shared_ptr<producer> producer_pointer;
shared_ptr<consumer> consumer_pointer;


int main( int argc, char* argv[] )
{
    qRegisterMetaType<vector_double>("vector_double");

    producer_pointer = make_shared<producer>(_SAMPLE_RATE_);
    consumer_pointer = make_shared<consumer>(producer_pointer, _SAMPLE_RATE_, _NUM_POINTS_, _SAMPLES_PER_FRAME_,
                                             _SMOOTH_WINDOW_SIZE_);

    auto detector = noise_detector(_NUM_POINTS_);
    auto saver = buffer_and_save();

    consumer_pointer->operations.emplace_back(&detector);
    consumer_pointer->raw_operations.emplace_back(&saver);

    consumer_pointer->run();
    producer_pointer->run();

    QObject::connect(&detector, SIGNAL(onNewSample(vector_double, vector_double, vector_double)), &saver, SLOT(set_vectors(vector_double, vector_double, vector_double)));

    int r = 0;
    if (argc == 1 || string(argv[1]) != "--no-gui") {
        QApplication app( argc, argv );

        FrequencyPlotter plotter;

        QObject::connect(&detector, SIGNAL(onNewSample(vector_double, vector_double, vector_double)), &plotter, SLOT(set_vectors(vector_double, vector_double, vector_double)));

        plotter.show();

        r = app.exec();

        consumer_pointer->stop();
        producer_pointer->stop();
    }
    else {
        struct sigaction sigIntHandler;
        sigIntHandler.sa_handler = [](sig_atomic_t s) {
            producer_pointer->stop();
            consumer_pointer->stop();

            cout << endl;
        };
        sigemptyset(&sigIntHandler.sa_mask);
        sigIntHandler.sa_flags = 0;
        sigaction(SIGINT, &sigIntHandler, NULL);

        pause();
    }

    return r;
}
