#define _POSIX_C_SOURCE 200809L
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>

typedef struct Data
{
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int value;
} Data;

void *my_thread_example_wait(void *opt)
{
    Data *thread_data = (Data *)opt;

    pthread_mutex_lock(&thread_data->mutex);
    while (thread_data->value != 2)
    {
        printf("A: Waiting for condition %d == 2\n", thread_data->value);
        fflush(NULL);
        pthread_cond_wait(&thread_data->cond, &thread_data->mutex);
    }
    printf("A: Signal received. Data value is %d\n", thread_data->value);
    fflush(NULL);
    pthread_mutex_unlock(&thread_data->mutex);

    return NULL;
}

void *my_thread_example_signal(void *opt)
{
    Data *thread_data = (Data *)opt;

    pthread_mutex_lock(&thread_data->mutex);
    printf("B: Modifying data from %d to 2\n", thread_data->value);
    fflush(NULL);
    thread_data->value = 2;
    pthread_mutex_unlock(&thread_data->mutex);
    printf("B: Signalling threads that value is now %d\n", thread_data->value);
    fflush(NULL);
    pthread_cond_signal(&thread_data->cond);

    return NULL;
}

int main(void)
{
    Data my_data = {.value = 1};
    pthread_mutex_init(&my_data.mutex, NULL);
    pthread_cond_init(&my_data.cond, NULL);

    pthread_attr_t pattr;
    pthread_attr_init(&pattr);

    pthread_t id_a = 0;
    pthread_t id_b = 0;

    pthread_create(&id_a, &pattr, my_thread_example_wait, &my_data);
    pthread_create(&id_b, &pattr, my_thread_example_signal, &my_data);

    pthread_join(id_a, NULL);
    pthread_join(id_b, NULL);

    return 0;
}
