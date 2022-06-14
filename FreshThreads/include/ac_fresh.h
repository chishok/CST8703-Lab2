#ifndef AC_FRESH_H
#define AC_FRESH_H

#include "pthread.h"

#define AC_FRESH_INFLUENCERS 1000
#define AC_FRESH_INFLUENCERS_BEFORE_APPRAISAL 50

/**
 * @brief Configuration data for fresh threads appraisal
 *
 */
typedef struct AcFreshConfig
{
    size_t num_influencers_before_appraisal;
    size_t num_influencers;
    double max_desirability;
    double min_cost;
    double max_cost;
    double influencer_change_factor;
} AcFreshConfig;

/**
 * @brief State shared between influencer threads
 *
 */
typedef struct AcFreshInfluencer
{
    pthread_mutex_t mtx;
    double desirability[AC_FRESH_INFLUENCERS];
    size_t count;
    unsigned int rng_seed;
} AcFreshInfluencer;

/**
 * @brief Appraisal start condition
 *
 */
typedef enum eAcFreshAppraisalFlag
{
    AC_FRESH_WAIT = 0,
    AC_FRESH_START_APPRAISAL
} eAcFreshAppraisalFlag;

/**
 * @brief Shared data to signal appraisal start
 *
 */
typedef struct AcFreshAppraisal
{
    pthread_mutex_t mtx;
    pthread_cond_t cond;
    eAcFreshAppraisalFlag flag;
    double desirability;
    double cost;
} AcFreshAppraisal;

/**
 * @brief All data to send to threads
 *
 */
typedef struct AcFreshDataOpts
{
    AcFreshInfluencer influencer;
    AcFreshAppraisal appraisal;
    AcFreshConfig config;
} AcFreshData;

AcFreshConfig ac_fresh_create_config();

AcFreshInfluencer ac_fresh_create_influencer();

AcFreshAppraisal ac_fresh_create_appraisal();

AcFreshData ac_fresh_create_data();

/**
 * @brief Thread function for influencer to modify desirability
 *
 * @param opts Thread data container @c AcFreshData
 * @return @c NULL
 */
void *ac_fresh_update_desirability_thread(void *opts);

/**
 * @brief Thread function for seller to appraise cost
 *
 * @param opts Thread data container @c AcFreshData
 * @return @c NULL
 */
void *ac_fresh_appraisal_thread(void *opts);

/**
 * @brief Print results of seller appraisals
 *
 * @param data Data containing summary of results
 */
void ac_fresh_print_results(const AcFreshData *data);

#endif
