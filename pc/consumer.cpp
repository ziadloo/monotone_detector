//
// Created by mehran on 2022-02-06.
//

#include <cmath>
#include <cstring>
#include <utility>
#include <algorithm>
#include <numeric>
#include <iostream>
#include <fstream>
#include "consumer.h"
#include "../utility.h"
#include <limits>
#include <math.h>

using namespace std;

consumer::consumer(shared_ptr<producer> p, int sample_rate, int num_of_points) :
    task(move(p->get_queue()), sample_rate), NUM_POINTS(num_of_points), pro(move(p)) {
}

void consumer::thread_code() {
    fftw_complex waveform[NUM_POINTS];
    fftw_complex result[NUM_POINTS];
    fftw_plan plan = fftw_plan_dft_1d(NUM_POINTS,
                                      waveform,
                                      result,
                                      FFTW_FORWARD,
                                      FFTW_ESTIMATE);
    for (int i = 0; i < NUM_POINTS; i++) {
        waveform[i][_REAL_] = 0.0;
        waveform[i][_IMAG_] = 0.0;
    }

    vector<double> hamming(NUM_POINTS);
    double hammingMag = 0.0;
    {
        double alpha = 0.54;
        for (int i = 0; i < NUM_POINTS; i++) {
            hamming[i] = alpha - (1.0 - alpha) * cos(2.0 * M_PI / (NUM_POINTS-1) * i);
            hammingMag += hamming[i];
        }
    }

    vector<format> sample;
    vector<format> buffer(NUM_POINTS);
    int bufferPointer = 0;
    while (!EXIT) {
        if (queue->wait_dequeue_timed(sample, std::chrono::milliseconds(500))) {
            
            int sampleSize = static_cast<int>(sample.size());
            for (int i=0; i<sampleSize; i++) {
                buffer[bufferPointer] = sample[i];
                bufferPointer = (bufferPointer + 1) % NUM_POINTS;
            }

            vector<format> sampleSeg(NUM_POINTS);
            format d = buffer[(bufferPointer+0+NUM_POINTS) % NUM_POINTS];
            waveform[0][_REAL_] = static_cast<double>(d) * hamming[0];
            sampleSeg[0] = d;
            for (int i=1; i<NUM_POINTS; i++) {
                d = buffer[(bufferPointer+i+NUM_POINTS) % NUM_POINTS];
                waveform[i][_REAL_] = (static_cast<double>(d) - 0.97 * waveform[i-1][_REAL_]) * hamming[i];
                sampleSeg[i] = d;
            }
            fftw_execute(plan);

            vector<double> spectrum(NUM_POINTS / 2);
            for (int i = 0; i < NUM_POINTS / 2; ++i) {
                spectrum[i] = 20.0 * log10(magnitude(result[i])
                                           * 2.0
                                           / hammingMag
                                           / static_cast<double>(std::numeric_limits<format>::max()));
            }

            for (auto& op: operations) {
                op->new_sample(sampleSeg, spectrum);
            }

            for (auto& op: raw_operations) {
                op->new_sample(sample);
            }
        }
        else {
            pro->reset();
        }
    }

    fftw_destroy_plan(plan);
}

double consumer::amplitude(fftw_complex const result) const {
    return sqrt(result[_REAL_] * result[_REAL_] +
                result[_IMAG_] * result[_IMAG_]);
}

double consumer::magnitude(fftw_complex const result) const {
    return result[_REAL_] * result[_REAL_] +
                result[_IMAG_] * result[_IMAG_];
}

void consumer::stop() {
    task::stop();
    for (auto& op : operations) {
        op->stop();
    }
    for (auto& op : raw_operations) {
        op->stop();
    }
}
