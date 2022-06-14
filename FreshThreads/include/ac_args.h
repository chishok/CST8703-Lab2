
#ifndef AC_ARGS_H
#define AC_ARGS_H

#include <stdio.h>

#include "ac_file.h"

/**
 * @brief Command line arguments
 *
 */
typedef struct AcArgs
{
    /**
     * @brief Binary data file name
     *
     */
    char filename[AC_FILE_MAX_LEN];
} AcArgs;

/**
 * @brief Create arguments data structure
 *
 * @return Arguments with default values.
 */
AcArgs ac_args_create();

/**
 * @brief Parse command line arguments from main function.
 *
 * Populates data from command line input. This function may cause program to
 * exit with failure on error.
 *
 * @param argc Main function argument count
 * @param argv Main function argument vector
 * @param args Output arguments data structure
 */
void ac_args_parse(int argc, char *argv[], AcArgs *args);

/**
 * @brief Validate command line arguments. If any argument is invalid,
 * an error message is printed and program exits.
 *
 * @param args Input command line arguments data structure.
 */
void ac_args_validate(const AcArgs *args, const char *prog);

#endif
