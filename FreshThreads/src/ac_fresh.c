#define _POSIX_C_SOURCE 200809L
#include <unistd.h>

#include <errno.h>
#include <stdio.h>
#include <stdbool.h>

#include "ac_timing.h"
#include "ac_fresh.h"
#include "ac_mock.h"
#include "ac_thread.h"

AcFreshConfig ac_fresh_create_config()
{
    return ((AcFreshConfig){
        .num_influencers_before_appraisal = AC_FRESH_INFLUENCERS_BEFORE_APPRAISAL,
        .num_influencers = AC_FRESH_INFLUENCERS,
        .max_desirability = 1.0,
        .min_cost = 20.0,
        .max_cost = 500.0,
        .influencer_change_factor = 0.2
    });
}

AcFreshInfluencer ac_fresh_create_influencer()
{
    return ((AcFreshInfluencer){
        .desirability = {0.0},
        .count = 0,
        .rng_seed = 1234
    });
}

AcFreshAppraisal ac_fresh_create_appraisal()
{
    return ((AcFreshAppraisal){
        .desirability = 0.0,
        .cost = 0.0,
        .flag = AC_FRESH_WAIT
    });
}

AcFreshData ac_fresh_create_data()
{
    return ((AcFreshData){
        .influencer = ac_fresh_create_influencer(),
        .appraisal = ac_fresh_create_appraisal(),
        .config = ac_fresh_create_config()
    });
}

void *ac_fresh_update_desirability_thread(void *opts)
{
    // Alias data
    AcFreshData *data = (AcFreshData *)opts;
    AcFreshInfluencer *influencer = &data->influencer;
    AcFreshAppraisal *appraisal = &data->appraisal;

    // absolute timeout 1sec
    const uint32_t timeout_micros = 1e6;
    struct timespec timeout;
    if (0 != ac_timing_set_timeout_usec(CLOCK_REALTIME, timeout_micros, &timeout))
    {
        perror("Failed to set timeout in influencer thread");
        return NULL;
    }

    // update while protected
    size_t influencer_count = 0;
    double influencer_desirability = 0.0;

    /*
     * STUDENT WORK SECTION BEGIN
     * ==========================
     */

    // 1. STUDENT: Lock influencer mutex for modifying desirability
    //             Use 'timeout' and 'pthread_mutex_timedlock' and check return value.

    // WARNING: It's very important that if the mutex lock fails, there is no
    //          call to pthread_mutex_unlock later in this function.

    // previous desirability
    const double prev_desirability = (influencer->count == 0 ? 0.5 * data->config.max_desirability : influencer->desirability[influencer->count - 1]);
    // influencer modifies fresh threads desirability - randomize the effect
    double change_factor = ac_mock_random(&influencer->rng_seed, -data->config.influencer_change_factor, data->config.influencer_change_factor);
    if (change_factor >= 0.0)
    {
        // increase desirability
        influencer->desirability[influencer->count] = prev_desirability + change_factor * (data->config.max_desirability - prev_desirability);
    }
    else
    {
        // reduce desirability
        influencer->desirability[influencer->count] = prev_desirability + change_factor * prev_desirability;
    }
    // insert delay
    ac_mock_delay(&influencer->rng_seed, 0.0001, 0.001);
    // update and copy while data is protected
    influencer_desirability = influencer->desirability[influencer->count];
    influencer->count += 1;
    influencer_count = influencer->count;

    // 2. STUDENT: Unlock influencer mutex for modifying desirability
    //          Use  'pthread_mutex_unlock'

    // STUDENT: After unlocking, use 'influencer_count' to send signal to appraisals

    if (influencer_count >= data->config.num_influencers_before_appraisal)
    {
        // Variable 'do_broadcast' will be updated in protected zone. This boolean will be set to 'true' if
        // transitioning condition flag from "wait" to "start appraisals"
        bool do_broadcast = false;

        // STUDENT: lock appraisal mutex 'appraisal->mtx' to check condition, use 'pthread_mutex_timedlock'

        // lock

        // if still waiting, change flag `appraisal->flag` to AC_FRESH_START_APPRAISAL
        if (appraisal->flag == AC_FRESH_WAIT)
        {
            appraisal->flag = AC_FRESH_START_APPRAISAL;
            appraisal->desirability = influencer_desirability;
            // we will broadcast change
            do_broadcast = true;
        }

        // STUDENT: unlock appraisal mutex 'appraisal->mtx' with `pthread_mutex_unlock`

        // unlock

        if (do_broadcast)
        {
            // STUDENT: Broadcast change to condition `appraisal->flag`.
            //          Use condvar 'appraisal->cond' and `pthread_cond_broadcast`

            // broadcast
        }
    }

    /*
     * ========================
     * STUDENT WORK SECTION END
     */

    return NULL;
}

void *ac_fresh_appraisal_thread(void *opts)
{
    // print thread info
    ac_thread_print_info(pthread_self());
    fflush(NULL);

    // Alias data
    AcFreshData *data = (AcFreshData *)opts;
    AcFreshAppraisal *appraisal = &data->appraisal;

    // absolute timeout to wait on start appraisal signal (1 second)
    const uint32_t timeout_wait_micros = 1e6;
    struct timespec timeout_wait;
    if (0 != ac_timing_set_timeout_usec(CLOCK_REALTIME, timeout_wait_micros, &timeout_wait))
    {
        perror("Failed to set timeout in seller thread");
        return NULL;
    }

    // absolute timeout 100 ms
    const uint32_t timeout_micros = 100000;
    struct timespec timeout;
    if (0 != ac_timing_set_timeout_usec(CLOCK_REALTIME, timeout_micros, &timeout))
    {
        perror("Failed to set timeout in seller thread");
        return NULL;
    }

    /*
     * STUDENT WORK SECTION BEGIN
     * ==========================
     */

    // 1. STUDENT: lock appraisal mutex 'appraisal->mtx' to check condition
    //          Use 'int status = pthread_mutex_timedlock()' with 'timeout'

    int status = 1;

    // break out either when signalled to start appraisal or failed wait on condvar
    while ((appraisal->flag != AC_FRESH_START_APPRAISAL) && (status == 0))
    {
        // 2. STUDENT: Wait for condition signal to check flag.
        //          use `pthread_cond_timedwait` with 'timeout_wait' and appraisal condvar with mutex.
        //          set return value to 'status'

        if (status != 0)
        {
            // 3. STUDENT: If failed wait, pthread_mutex_unlock appraisal->mtx before we return.

            // WARNING: Make sure any locked mutexes are unlocked exactly once before exiting scope in which mutex was locked.

            // unlock

            errno = status;
            perror("Failed to wait on appraisal condition variable signal");
            return NULL;
        }
    }

    // If we got here, the only possible way is if `appraisal->flag == AC_FRESH_START_APPRAISAL`
    // since we return in while loop above in case of error (including timeout)
    // so we calculate the cost from desirability factor now

    appraisal->cost = appraisal->desirability * (data->config.max_cost - data->config.min_cost) + data->config.min_cost;

    // 4. STUDENT: unlock appraisal mutex 'appraisal->mtx' with `pthread_mutex_unlock`

    /*
     * ========================
     * STUDENT WORK SECTION END
     */

    return NULL;
}

void ac_fresh_print_results(const AcFreshData *data)
{
    const AcFreshAppraisal *appraisal = &data->appraisal;
    const double final_desirability = data->influencer.desirability[data->config.num_influencers - 1];
    const double final_cost = final_desirability * (data->config.max_cost - data->config.min_cost) + data->config.min_cost;

    printf("Fresh Threads Social Marketplace\n");
    printf("================================\n");
    printf("Inputs:\n");
    printf("    influencer change factor: %.2f\n", data->config.influencer_change_factor);
    printf("    number of influencers: %zu\n", data->config.num_influencers);
    printf("    minimum cost: $%.2f\n", data->config.min_cost);
    printf("    maximum cost: $%.2f\n", data->config.max_cost);
    printf("Appraisal Results:\n");
    printf("    latest desirability: %f\n", final_desirability);
    printf("    latest cost: $%.2f\n", final_cost);
    printf("    appraisal after %zu influencer mentions:\n", data->config.num_influencers_before_appraisal);
    printf("        desirability %f\n", appraisal->desirability);
    printf("        cost $%.2f\n", appraisal->cost);
}

