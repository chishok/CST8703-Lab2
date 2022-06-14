#define _POSIX_C_SOURCE 200809L
#include <unistd.h>

#include <time.h>
#include <stdlib.h>

#include "ac_mock.h"
#include "ac_timing.h"

double ac_mock_random(unsigned int *seed, const double min, const double max)
{
    return min + (max - min) * ((double) rand_r(seed)) / ((double) RAND_MAX);
}

int ac_mock_delay(unsigned int *seed, const double min_duration, const double max_duration)
{
    double delay_seconds = ac_mock_random(seed, min_duration, max_duration);
    return ac_timing_delay_seconds(delay_seconds);
}
