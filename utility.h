//
// Created by mehran on 2022-02-06.
//

#ifndef ARECORD_UTILITY_H
#define ARECORD_UTILITY_H

#include <vector>

#define _REAL_ 0
#define _IMAG_ 1
#define _SAMPLE_RATE_ 44100
#define _NUM_POINTS_ 4096 //_SAMPLE_RATE_/16
#define _FRAMES_PER_SEC_ 20
#define _SAMPLES_PER_FRAME_ _SAMPLE_RATE_/_FRAMES_PER_SEC_
#define _BELL_SIZE_ _NUM_POINTS_ * 1000 / (_SAMPLE_RATE_ / 2)
#define _BELL_SIGMA_ _NUM_POINTS_ * 10 / (_SAMPLE_RATE_ / 2)
#define _BELL_MAGNITUDE_ 100.0
#define _AVERAGE_HISTORY_SIZE_ 20

#define _PLOT_MIN_Y_ -100.0
#define _PLOT_MAX_Y_ 100.0

#define _NOISE_THRESHOLD_ 0.10
#define _CLEARANCE_THRESHOLD_ 0.05
#define _SMOOTH_WINDOW_SIZE_ 5
#define _INERTIA_DELTA_ 1.0/100.0

typedef short format;
typedef std::vector<double> vector_double;


#endif //ARECORD_UTILITY_H
