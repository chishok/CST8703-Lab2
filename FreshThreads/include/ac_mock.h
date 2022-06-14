
#ifndef AC_MOCK_SENSOR_H
#define AC_MOCK_SENSOR_H

/**
 * @brief Generate random floating point number between min and max values.
 *
 * Distribution is uniform and uses the c standard pseudo-random number generator (not truly random).
 *
 * @param[in,out] seed Seed is used as input and modified by the random number generator
 * @param[in] min Minimum value
 * @param[in] max Maximum value
 * @return Random number between min and max
 */
double ac_mock_random(unsigned int *seed, const double min, const double max);

/**
 * @brief Sleep for duration
 *
 * @param seed         Random number generator seed
 * @param min_duration Minimum duration in seconds
 * @param max_duration Maximum duration in seconds
 * @return             Return value of clock_nanosleep
 */
int ac_mock_delay(unsigned int *seed, double min_duration, double max_duration);

#endif
