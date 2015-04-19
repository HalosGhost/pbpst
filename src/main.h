#include <getopt.h>    // getopt_long()
#include <stdio.h>     // fprintf(), fputs()
#include <stdbool.h>   // bool
#include <stdint.h>    // explicitly-sized integral types
#include <stdlib.h>    // EXIT_SUCCESS
#include <curl/curl.h> // curl_easy_*()
#include <sys/stat.h>  // fstat()
#include <string.h>    // strncat()
#include <fcntl.h>     // open()
#include <unistd.h>    // close()

#define BUFFER_SIZE 256
#define FILE_MAX 62914560 // 60 MiB

static struct option cmds [] = {
    { "sync",     no_argument,       0, 'S' },
    { "remove",   no_argument,       0, 'R' },
    { "update",   no_argument,       0, 'U' },
    { "provider", required_argument, 0, 'p' },
    { "help",     no_argument,       0, 'h' },
    { "verbose",  no_argument,       0, 'v' },
    { "version",  no_argument,       0, 0   },
    { 0,          0,                 0, 0   }
};

static struct option sync_opts [] = {
    { "shorten", required_argument, 0, 's' },
    { "file",    required_argument, 0, 'f' },
    { "lexer",   required_argument, 0, 'l' },
    { "line",    required_argument, 0, 'L' },
    { "private", no_argument,       0, 'P' },
    { "render",  no_argument,       0, 'r' },
    { "vanity",  required_argument, 0, 'V' },
    { "help",    no_argument,       0, 'h' },
    { 0,         0,                 0, 0   }
};

static struct option rem_opts [] = {
    { "uuid", required_argument, 0, 'u' },
    { "help", no_argument,       0, 'h' },
    { 0,      0,                 0, 0   },
};

static struct option upd_opts [] = {
    { "shorten", required_argument, 0, 's' },
    { "file",    required_argument, 0, 'f' },
    { "lexer",   required_argument, 0, 'l' },
    { "line",    required_argument, 0, 'L' },
    { "render",  no_argument,       0, 'r' },
    { "uuid",    required_argument, 0, 'u' },
    { "help",    no_argument,       0, 'h' },
    { 0,         0,                 0, 0   }
};

// vim: set ts=4 sw=4 et:
