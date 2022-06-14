#define _POSIX_C_SOURCE 200809L
#include <unistd.h>

#include <errno.h>
#include <time.h>

#include "ac_timing.h"

AcTimer ac_timer_create()
{
    return (AcTimer){.step_size = {.tv_sec = 0, .tv_nsec = 0},
                     .start = {.tv_sec = 0, .tv_nsec = 0},
                     .target = {.tv_sec = 0, .tv_nsec = 0},
                     .elapsed = {.tv_sec = 0, .tv_nsec = 0},
                     .real = {.tv_sec = 0, .tv_nsec = 0}};
}

int ac_timer_start(AcTimer *timer, const uint32_t step_size_usec)
{
    int result = clock_gettime(CLOCK_MONOTONIC, &timer->start);
    if (result == 0)
    {
        result = clock_gettime(CLOCK_REALTIME, &timer->real);
        // save start time as target
        timer->target = timer->start;
        timer->elapsed.tv_sec = 0;
        timer->elapsed.tv_nsec = 0;
        // step size
        timer->step_size.tv_sec = step_size_usec / AC_MEGA;
        timer->step_size.tv_nsec = (step_size_usec - (timer->step_size.tv_sec * AC_MEGA)) * AC_KILO;
    }
    return result;
}

int ac_timer_advance_and_wait(AcTimer *timer)
{
    // advance target
    timer->target.tv_sec += timer->step_size.tv_sec;
    timer->target.tv_nsec += timer->step_size.tv_nsec;
    if (timer->target.tv_nsec > AC_GIGA)
    {
        timer->target.tv_sec += 1;
        timer->target.tv_nsec -= AC_GIGA;
    }
    // sleep to target
    int result = clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &timer->target, NULL);
    if (result == 0)
    {
        // get new time after sleep
        result = clock_gettime(CLOCK_MONOTONIC, &timer->elapsed);
        clock_gettime(CLOCK_REALTIME, &timer->real);
        // get elapsed
        timer->elapsed.tv_sec -= timer->start.tv_sec;
        timer->elapsed.tv_nsec -= timer->start.tv_nsec;
        if (timer->elapsed.tv_nsec < 0)
        {
            timer->elapsed.tv_sec -= 1;
            timer->elapsed.tv_nsec += AC_GIGA;
        }
    }
    return result;
}

uint64_t ac_timer_get_elapsed_usec(AcTimer *timer)
{
    return (uint64_t)(timer->elapsed.tv_sec * AC_MEGA) + (uint64_t)(timer->elapsed.tv_nsec / AC_KILO);
}

int ac_timing_get_timestamp(char *timestamp, const size_t size)
{
    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);
    size_t len = strftime(timestamp, size, "%Y-%m-%d %H:%M:%S", &timeinfo);
    return (len == 0 ? EMSGSIZE : 0);
}

void ac_timing_add_usec(const uint32_t time_usec, struct timespec *ts)
{
    // add time
    const uint32_t seconds_to_add = time_usec / AC_MEGA;
    ts->tv_sec += (time_t)seconds_to_add;
    ts->tv_nsec += (long)((time_usec - (seconds_to_add * AC_MEGA)) * AC_KILO);
    if (ts->tv_nsec > AC_GIGA)
    {
        ts->tv_sec += 1;
        ts->tv_nsec -= AC_GIGA;
    }
}

int ac_timing_set_timeout_usec(clockid_t clock_id, const uint32_t timeout_usec, struct timespec *ts)
{
    int status = clock_gettime(clock_id, ts);
    if (status == 0)
    {
        ac_timing_add_usec(timeout_usec, ts);
    }
    return status;
}

int ac_timing_delay_seconds(double duration)
{
    // add time
    struct timespec ts;
    ts.tv_sec = (time_t)duration;
    ts.tv_nsec = (long)((duration - ((double)ts.tv_sec)) * AC_GIGA);
    return clock_nanosleep(CLOCK_MONOTONIC, 0, &ts, NULL);
}
