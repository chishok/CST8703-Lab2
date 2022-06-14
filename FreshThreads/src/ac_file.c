#define _POSIX_C_SOURCE 200809L
#include <unistd.h>

#include <string.h>
#include <stdio.h>
#include <errno.h>

#include "ac_file.h"

int ac_file_write(const char *path, const void *data, const size_t data_size)
{
    FILE *fptr = NULL;

    size_t len = strnlen(path, AC_FILE_MAX_LEN);
    if (len == 0 || len == AC_FILE_MAX_LEN) {
        return ENAMETOOLONG;
    }

    fptr = fopen(path, "w");
    if (fptr == NULL) {
        return errno;
    }

    if (fwrite(data, data_size, 1, fptr) != 1)
    {
        fclose(fptr);
        return errno;
    }

    if (fclose(fptr) != 0)
    {
        return errno;
    }

    return 0;
}
