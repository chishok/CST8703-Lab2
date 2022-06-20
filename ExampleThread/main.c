#define _POSIX_C_SOURCE 200809L
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>

typedef struct Data
{
    int value;
} Data;

void *my_thread_example(void *opt)
{
    Data *thread_data = (Data *)opt;
    printf("Data value is %d\n", thread_data->value);
    return NULL;
}

int main(void)
{
    Data my_data = {.value = 1};

    pthread_attr_t pattr;
    pthread_attr_init(&pattr);

    pthread_t id_a = 0;
    pthread_t id_b = 0;

    pthread_create(&id_a, &pattr, my_thread_example, &my_data);
    my_data.value = 2;
    pthread_create(&id_b, &pattr, my_thread_example, &my_data);

    pthread_join(id_a, NULL);
    pthread_join(id_b, NULL);

    return 0;
}
