#define _POSIX_C_SOURCE 200809L
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>

typedef struct Data
{
    pthread_mutex_t mutex;
    int value;
} Data;

void *my_thread_example(void *opt)
{
    Data *thread_data = (Data *)opt;

    pthread_mutex_lock(&thread_data->mutex);
    thread_data->value += 1;
    printf("Data value is %d\n", thread_data->value);
    fflush(NULL);
    pthread_mutex_unlock(&thread_data->mutex);

    return NULL;
}

int main(void)
{
    Data my_data = {.value = 1};
    pthread_mutex_init(&my_data.mutex, NULL);

    pthread_attr_t pattr;
    pthread_attr_init(&pattr);

    pthread_t id_a = 0;
    pthread_t id_b = 0;

    pthread_create(&id_a, &pattr, my_thread_example, &my_data);
    pthread_create(&id_b, &pattr, my_thread_example, &my_data);

    pthread_join(id_a, NULL);
    pthread_join(id_b, NULL);

    return 0;
}
