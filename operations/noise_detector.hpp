//
// Created by mehran on 2022-02-07.
//

#ifndef ARECORD_VISUALIZE_HPP
#define ARECORD_VISUALIZE_HPP

#define WITHOUT_NUMPY
#define _USE_MATH_DEFINES

#include <vector>
#include <functional>
#include <cmath>
#include <chrono>
#include <algorithm>
#include <iostream>
#include <list>
#include "operation.h"
#include "../utility.h"

using namespace std;

class noise_detector: public operation {
public:
    noise_detector(int sample_count): operation(sample_count) {
        for (size_t i=0; i < sample_count/2; i++) {
            x.push_back(i);
        }

        reference.resize(_BELL_SIZE_);
        auto sigma = static_cast<double>(_BELL_SIGMA_);
        for (int i=(reference.size()-1)/2; i<reference.size(); i++) {
            double _p1 = static_cast<double>(i - (reference.size()-1)/2) / sigma;
            double _p2 = 1.0 / (sigma * pow(2.0 * M_PI, 0.5));
            int j = static_cast<int>((reference.size()-1)/2 - (i-(reference.size()-1)/2));
            reference[i] = reference[j] = _p2 * exp(-0.5 * _p1 * _p1) * _BELL_MAGNITUDE_;
        }

        for (int i=1; i<reference.size(); i++) {
            reference_sum += reference[i];
            reference_sq_sum += reference[i] * reference[i];
        }

        sum.resize(_NUM_POINTS_/2);
        noise_value.resize(_NUM_POINTS_/2);
        noise.resize(_NUM_POINTS_/2);
        for (int i=0; i<_NUM_POINTS_/2; i++) {
            sum[i] = 0.0;
            noise_value[i] = 0.0;
            noise[i] = false;
        }
    }
    vector<std::function<void (vector<double> spectrum, vector<double> similarity, vector<double> noises)>> on_new_sample;

protected:
    vector<double> reference;
    vector<double> sum;
    double reference_sum = 0.0;
    double reference_sq_sum = 0.0;
    list<vector<double>> history;
    vector<double> noise_value;
    vector<bool> noise;
    void process(vector<format> sample, vector<double> spectrum) override {
        if (isnan(spectrum[0]) || isinf(spectrum[0])) {
            return;
        }

        history.emplace_back(spectrum);
        for (int i=0; i<spectrum.size(); i++) {
            sum[i] += spectrum[i];
        }

        if (history.size() > _AVERAGE_HISTORY_SIZE_) {
            vector<double> retired = history.front();
            history.pop_front();
            for (int i=0; i<retired.size(); i++) {
                sum[i] -= retired[i];
            }
        }

        vector<double> average(sum.size());
        for (int i=0; i<sum.size(); i++) {
            average[i] = sum[i] / static_cast<double>(history.size());
        }

        vector<double> similarity = calculateSimilarity(average);
        for (int i=0; i<similarity.size(); i++) {
            if (similarity[i] > _NOISE_THRESHOLD_) {
                double n = noise_value[i] + _INERTIA_DELTA_/2.0;
                if (n > 0.5) {
                    noise_value[i] = 1.0;
                    noise[i] = true;
                }
                else {
                    noise_value[i] = n;
                }
            }
            else if (similarity[i] < _CLEARANCE_THRESHOLD_) {
                double n = noise_value[i] - _INERTIA_DELTA_/2.0;
                if (n < 0.5) {
                    noise_value[i] = 0.0;
                    noise[i] = false;
                }
                else {
                    noise_value[i] = n;
                }
            }
        }

        vector<double> noise_bars;
        int noise_start = -1;
        for (int i=0; i<noise.size(); i++) {
            if (noise[i] && noise_start == -1) {
                noise_start = i;
            }
            else if (!noise[i] && noise_start != -1) {
                noise_start = -1;
                noise_bars.emplace_back(static_cast<double>(i+noise_start) / (_NUM_POINTS_/2.0) * (_SAMPLE_RATE_/2.0));
            }
        }
        if (noise_start != -1) {
            noise_bars.emplace_back(static_cast<double>(noise.size()+noise_start) / (_NUM_POINTS_/2.0) * (_SAMPLE_RATE_/2.0));
        }

        for (auto callback : on_new_sample) {
            callback(average, similarity, noise_bars);
        }
    }

    vector<double> calculateSimilarity(vector<double> signal) {
        vector<double> similarity(signal.size());

        vector<double> signal_acc(signal.size()+1);
        vector<double> signal_sq_acc(signal.size()+1);
        signal_acc[0] = signal[0];
        signal_sq_acc[0] = signal[0] * signal[0];
        for (int i=1; i<signal.size(); i++) {
            signal_acc[i] = signal_acc[i-1] + signal[i];
            signal_sq_acc[i] = signal_sq_acc[i-1] + signal[i] * signal[i];
        }
        signal_acc[signal_acc.size()-1] = signal_acc[signal_acc.size()-2];
        signal_sq_acc[signal_sq_acc.size()-1] = signal_sq_acc[signal_sq_acc.size()-2];

        double n = static_cast<double>(reference.size());
        for (int center=0; center<similarity.size(); center++) {
            int from = max(0, static_cast<int>(center - (reference.size()-1) / 2));
            int to = min(static_cast<int>(signal.size()), static_cast<int>(center + (reference.size()-1) / 2));

            double x = reference_sum,
                y = 0.0,
                x2 = reference_sq_sum,
                y2 = 0.0,
                xy = 0.0;

            y += signal_acc[to] - signal_acc[from];
            y2 += signal_sq_acc[to] - signal_sq_acc[from];

            for (int i = from; i < to; i++) {
                xy += reference[i - from] * signal[i];
            }

            similarity[center] = (n * xy - x * y) / sqrt(n * x2 - x * x) / sqrt(n * y2 - y * y);
            similarity[center] = similarity[center] * similarity[center] * similarity[center];
        }

        return similarity;
    }

private:
    vector<double> x;
};

#endif //ARECORD_VISUALIZE_HPP
