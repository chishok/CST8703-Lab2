
#ifndef AC_THREAD_H
#define AC_THREAD_H

#include <errno.h>
#include <pthread.h>
#include <time.h>
#include <inttypes.h>
#include <stdbool.h>

#ifndef AC_THREAD_NUM_CPUS
/**
 * @brief Number of CPUs max for affinity
 *
 */
#define AC_THREAD_NUM_CPUS 4
#endif

/**
 * @brief Maximum length allowed for custom thread names
 *
 */
#define AC_THREAD_NAME_MAX_LEN 16

/**
 * @brief Maximum length allowed when printing
 *
 */
#define AC_THREAD_NAME_PRINT_MAX_LEN 250

/**
 * @brief Function signature for threads
 *
 */
typedef void *(*AcThreadFn)(void *);

/**
 * @brief Custom attributes of a thread.
 *
 */
typedef struct AcThreadAttributes
{
    /**
     * @brief Thread policy
     *
     */
    int policy;
    /**
     * @brief Thread priority
     *
     */
    int priority;
    /**
     * @brief Minimum stack size in kilobytes
     *
     */
    size_t stack_size_kb;
    /**
     * @brief CPU affinity set
     *
     */
    int cpu_set[AC_THREAD_NUM_CPUS];
    /**
     * @brief Size of CPU affinity
     *
     * If not using custom affinity, set to 0.
     */
    size_t cpu_setsize;
    /**
     * @brief Thread function pointer.
     *
     */
    AcThreadFn fcn;
    /**
     * @brief Options to send to thread.
     *
     */
    void *opts;
    /**
     * @brief Thread id set when thread is started.
     *
     */
    pthread_t id;
    /**
     * @brief Name of thread.
     *
     */
    char name[AC_THREAD_NAME_MAX_LEN];
} AcThreadAttributes;

/**
 * @brief Print information about thread.
 *
 * @param[in] thread_id Thread id
 */
void ac_thread_print_info(pthread_t thread_id);

/**
 * @brief Convenience function to set priority and policy of current thread.
 *
 * @param policy
 * @param priority
 * @return Result of call to @c pthread_setschedparam
 */
int ac_thread_set_scheduling_self(int policy, int priority);

/**
 * @brief Create thread configuration attributes.
 *
 * @return default thread attributes
 */
AcThreadAttributes ac_thread_attr_create();

/**
 * @brief Spawn thread with provided attributes
 *
 * @param attr Thread attributes
 * @return Success if 0, failed otherwise
 */
int ac_thread_spawn(AcThreadAttributes *attr);

/**
 * @brief Join thread
 *
 * @param attr Thread attributes
 * @param timeout Timeout (absolute CLOCK_REALTIME)
 * @return Success if 0, failed otherwise
 */
int ac_thread_join(AcThreadAttributes *attr, const struct timespec *timeout);

#endif
