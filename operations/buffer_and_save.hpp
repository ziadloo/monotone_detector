//
// Created by mehran on 2022-02-07.
//

#ifndef ARECORD_DETECT_AND_SAVE_HPP
#define ARECORD_DETECT_AND_SAVE_HPP

#include <cmath>
#include <cstring>
#include <utility>
#include <algorithm>
#include <vector>
#include <functional>
#include <numeric>
#include <iostream>
#include "./raw_operation.h"
#include "../lib/AudioFile.h"
#include "../utility.h"

using namespace std;

class buffer_and_save: public raw_operation {
public:
    void start_recording(vector<double> _noises) {
        recording = true;
        started_at = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        auto s_started_at = string(ctime(&started_at));

        cout << "Started at: " << s_started_at.substr(0, s_started_at.size()-1) <<
            ", with noises: " <<
            accumulate(begin(_noises), end(_noises), string(), [](const std::string& a, const double& b) -> std::string {
                return a + (a.length() > 0 ? ", " : "") + to_string(static_cast<int>(round(b)));
            }) << endl;
    }

    void stop_recording(vector<double> _noises) {
        recording = false;

        std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        auto s_now = string(ctime(&now));

        cout << "Stopped at: " << s_now.substr(0, s_now.size()-1) <<
            " (" << std::chrono::duration<int, std::milli>(now-started_at).count() << " seconds)" <<
            ", with noises: " <<
            accumulate(begin(_noises), end(_noises), string(), [](const std::string& a, const double& b) -> std::string {
                return a + (a.length() > 0 ? ", " : "") + to_string(static_cast<int>(round(b)));
            }) << endl << endl;

        AudioFile<float> a;
        a.setNumChannels(1);
        int totalNumberOfSamples = accumulate(begin(sample_buffer), end(sample_buffer), 0,
                                              [](int t, vector<format> i) { return t + i.size(); });
        a.setNumSamplesPerChannel(totalNumberOfSamples);
        a.setSampleRate(_SAMPLE_RATE_);

        int s = 0;
        for (auto v: sample_buffer) {
            for (int i = 0; i < v.size(); i++, s++) {
                a.samples[0][s] = static_cast<float>(v[i]) / static_cast<float>(std::numeric_limits<format>::max());
            }
        }

        string filename = s_now.substr(0, s_now.size()-1);
        std::replace( begin(filename), end(filename), ':', '-');
        a.save( filename + ".wav", AudioFileFormat::Wave);

        sample_buffer.clear();
    }

protected:
    bool recording = false;
    std::time_t started_at;
    list<vector<format>> sample_buffer;
    void process() override {
        if (recording) {
            if (sample_buffer.size() < 10000) {
                copy(begin(sample_history), end(sample_history), back_inserter(sample_buffer));
            }
            sample_history.clear();
        }
    }
};

#endif //ARECORD_DETECT_AND_SAVE_HPP
