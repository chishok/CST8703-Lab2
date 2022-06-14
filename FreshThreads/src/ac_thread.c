#define _GNU_SOURCE
#define _POSIX_C_SOURCE 200809L
#include <unistd.h>

#include <errno.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "ac_timing.h"
#include "ac_thread.h"

static const char *ac_thread_policy_string(const int policy)
{
    const char *result = "";
    switch (policy)
    {
        case SCHED_RR:
        {
            result = "SCHED_RR";
            break;
        }
        case SCHED_FIFO:
        {
            result = "SCHED_FIFO";
            break;
        }
        case SCHED_OTHER:
        {
            result = "SCHED_OTHER";
            break;
        }
        default:
        {
            result = "UNKNOWN";
            break;
        }
    }
    return result;
}

void ac_thread_print_info(pthread_t thread_id)
{
    // set realtime scheduler priority
    int policy = SCHED_OTHER;
    struct sched_param param = {.sched_priority = 0};

    int result = pthread_getschedparam(thread_id, &policy, &param);
    if (result != 0)
    {
        errno = result;
        perror("pthread_getschedparam");
    }

    char timestamp[AC_TIMESTAMP_MAX_SIZE] = {'\0'};
    result = ac_timing_get_timestamp(timestamp, AC_TIMESTAMP_MAX_SIZE);
    if (result != 0)
    {
        errno = result;
        perror("get_timestamp");
    }

    char thread_name[AC_THREAD_NAME_PRINT_MAX_LEN + 1] = {'\0'};
    result = pthread_getname_np(thread_id, thread_name, AC_THREAD_NAME_PRINT_MAX_LEN);
    if (result != 0)
    {
        errno = result;
        perror("pthread_getname_np");
    }

    printf("Thread info:\n");
    printf("    thread name: %s\n", thread_name);
    printf("    thread policy: %s\n", ac_thread_policy_string(policy));
    printf("    thread priority: %d\n", param.sched_priority);
    printf("    thread id: %lu\n", thread_id);
    printf("    process id: %d\n", getpid());
    printf("    timestamp: %s\n", timestamp);
}

int ac_thread_set_scheduling_self(const int policy, const int priority)
{
    pthread_t thread_id = pthread_self();
    struct sched_param param = {.sched_priority = priority};
    int result = pthread_setschedparam(thread_id, policy, &param);
    if (result != 0)
    {
        errno = result;
    }
    return result;
}

AcThreadAttributes ac_thread_attr_create()
{
    return (AcThreadAttributes){
        .policy = SCHED_OTHER,
        .priority = 0,
        .stack_size_kb = 2048,
        .cpu_set = {0},
        .cpu_setsize = 0,
        .fcn = NULL,
        .opts = NULL,
        .id = 0,
        .name = {'\0'}
    };
}

int ac_thread_spawn(AcThreadAttributes *attr)
{
    pthread_attr_t pattr;
    int status = pthread_attr_init(&pattr);
    if (status != 0)
    {
        errno = status;
        perror("pthread_attr_init");
        return status;
    }

    status = pthread_attr_setinheritsched(&pattr, PTHREAD_EXPLICIT_SCHED);
    if (status != 0)
    {
        errno = status;
        perror("pthread_attr_setinheritsched");
        return status;
    }

    status = pthread_attr_setschedpolicy(&pattr, attr->policy);
    if (status != 0)
    {
        errno = status;
        perror("pthread_attr_setschedpolicy");
        return status;
    }

    struct sched_param param = {.sched_priority = attr->priority};
    status = pthread_attr_setschedparam(&pattr, &param);
    if (status != 0)
    {
        errno = status;
        perror("pthread_attr_setschedparam");
        return status;
    }

    const size_t stack_bytes = attr->stack_size_kb * 1024;
    status = pthread_attr_setstacksize(&pattr, stack_bytes);
    if (status != 0)
    {
        errno = status;
        perror("pthread_attr_setstacksize");
        return status;
    }

    status = pthread_create(&attr->id, &pattr, attr->fcn, attr->opts);
    if (status != 0)
    {
        errno = status;
        perror("pthread_create");
        return status;
    }

    if (strnlen(attr->name, AC_THREAD_NAME_MAX_LEN) > 0)
    {
        attr->name[AC_THREAD_NAME_MAX_LEN - 1] = '\0';
        status = pthread_setname_np(attr->id, attr->name);
        if (status != 0)
        {
            errno = status;
            perror("pthread_setname_np");
            return status;
        }
    }

    if (attr->cpu_setsize > 0)
    {
        cpu_set_t cpu_set;
        CPU_ZERO(&cpu_set);
        for (size_t idx = 0; idx < attr->cpu_setsize; idx++)
        {
            if (attr->cpu_set[idx])
            {
                CPU_SET(idx, &cpu_set);
            }
        }
        status = pthread_setaffinity_np(attr->id, attr->cpu_setsize, &cpu_set);
        if (status != 0)
        {
            errno = status;
            perror("pthread_setaffinity_np");
            return status;
        }
    }

    return 0;
}

int ac_thread_join(AcThreadAttributes *attr, const struct timespec *timeout)
{
    return pthread_timedjoin_np(attr->id, NULL, timeout);
}
