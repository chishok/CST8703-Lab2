#define _POSIX_C_SOURCE 200809L
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>

#include "ac_args.h"

AcArgs ac_args_create()
{
    return (AcArgs){
        .filename = {"data/dataLog.dat"}
    };
}

static void ac_args_print_usage_and_exit(const char *prog)
{
    fprintf(stderr,
            "Usage: %s\n"
            "  [-f FILENAME]  data output file path\n", prog);
    exit(EXIT_FAILURE);
}

void ac_args_validate(const AcArgs *args, const char *prog)
{
    (void)args;
    (void)prog;
}

void ac_args_parse(int argc, char *argv[], AcArgs *args)
{
    const char *options = "f:";
    opterr = 0;
    int opt = getopt(argc, argv, options);
    while (opt != -1)
    {
        switch (opt)
        {
            case 'f':
            {
                int retval = snprintf(args->filename, AC_FILE_MAX_LEN, "%s", optarg);
                if (retval <= 0 || retval == AC_FILE_MAX_LEN)
                {
                    fprintf(stderr, "Failed to retrieve filename from option `-%c'\n", optopt);
                    ac_args_print_usage_and_exit(argv[0]);
                }
                break;
            }
            case '?':
            {
                if (optopt == 'f')
                {
                    fprintf(stderr, "Option -%c requires an argument.\n", optopt);
                }
                else
                {
                    fprintf(stderr, "Unknown option `-%c'.\n", optopt);
                }
                ac_args_print_usage_and_exit(argv[0]);
                break;
            }
            default:
            {
                ac_args_print_usage_and_exit(argv[0]);
            }
        }
        opt = getopt(argc, argv, options);
    }

    printf("Arguments parsed\n");
    printf("  filename: %s\n", args->filename);

    ac_args_validate(args, argv[0]);
}
