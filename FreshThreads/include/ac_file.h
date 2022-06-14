
#ifndef AC_FILE_H
#define AC_FILE_H

#define AC_FILE_MAX_LEN 255

/**
 * @brief Write binary data to file.
 *
 * @param path File path. Must be a null-terminated string.
 * @param data Pointer to raw data to be written to file.
 * @param data_size Size of raw data to write.
 * @return 0 on success, error code otherwise.
 */
int ac_file_write(const char *path, const void *data, const size_t data_size);

#endif
