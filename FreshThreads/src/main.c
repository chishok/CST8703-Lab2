#define _POSIX_C_SOURCE 200809L
#include <unistd.h>

#include <errno.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "ac_timing.h"
#include "ac_thread.h"
#include "ac_fresh.h"
#include "ac_args.h"
#include "ac_file.h"

int main(int argc, char *argv[])
{
    // arguments
    AcArgs args = ac_args_create();
    ac_args_parse(argc, argv, &args);

    // 2 second timeout
    const uint32_t timeout_microseconds = 2e6;
    // create data
    AcFreshData data = ac_fresh_create_data();

    /*
     * STUDENT WORK SECTION BEGIN
     * ==========================
     */

    // 1. try different seeds as part of analysis (do not submit with modified values)
    data.influencer.rng_seed = 1234;
    // 2. try different change factor as part of analysis (do not submit with modified values)
    data.config.influencer_change_factor = 0.2;

    /*
     * ========================
     * STUDENT WORK SECTION END
     */

    // initalize mutexes and condvars
    int retval = pthread_mutex_init(&data.influencer.mtx, NULL);
    if (retval != 0)
    {
        errno = retval;
        perror("Failed to initialize influencer mutex");
        return EXIT_FAILURE;
    }
    retval = pthread_mutex_init(&data.appraisal.mtx, NULL);
    if (retval != 0)
    {
        errno = retval;
        perror("Failed to initialize appraisal mutex");
        return EXIT_FAILURE;
    }
    retval = pthread_cond_init(&data.appraisal.cond, NULL);
    if (retval != 0)
    {
        errno = retval;
        perror("Failed to initialize appraisal condvar");
        return EXIT_FAILURE;
    }

    // set thread information
    // influencers
    AcThreadAttributes influencer_threads[AC_FRESH_INFLUENCERS];
    for (size_t k = 0; k < data.config.num_influencers; ++k)
    {
        influencer_threads[k] = ac_thread_attr_create();
        // each thread adjusts desirability factor
        influencer_threads[k].fcn = ac_fresh_update_desirability_thread;
        influencer_threads[k].opts = &data;
    }
    // appraisal
    AcThreadAttributes appraisal_thread = ac_thread_attr_create();
    appraisal_thread.opts = &data;
    appraisal_thread.fcn = ac_fresh_appraisal_thread;

    // spawn threads
    for (size_t k = 0; k < data.config.num_influencers; ++k)
    {
        if (ac_thread_spawn(&influencer_threads[k]) != 0)
        {
            fprintf(stderr, "Failed to spawn influencer thread\n");
            return EXIT_FAILURE;
        }
    }
    if (ac_thread_spawn(&appraisal_thread) != 0)
    {
        fprintf(stderr, "Failed to spawn appraisal thread\n");
        return EXIT_FAILURE;
    }

    // timeout for thread join
    struct timespec timeout = {.tv_sec=0, .tv_nsec=0};
    if (ac_timing_set_timeout_usec(CLOCK_REALTIME, timeout_microseconds, &timeout) != 0)
    {
        perror("failed to set timeout");
        return EXIT_FAILURE;
    }

    // join threads
    for (size_t k = 0; k < data.config.num_influencers; ++k)
    {
        ac_thread_join(&influencer_threads[k], &timeout);
    }
    ac_thread_join(&appraisal_thread, &timeout);

    // print results
    ac_fresh_print_results(&data);

    // write to file
    retval = ac_file_write(args.filename, data.influencer.desirability, sizeof(double) * data.config.num_influencers);
    if (retval != 0)
    {
        perror("Failed to write data file");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
