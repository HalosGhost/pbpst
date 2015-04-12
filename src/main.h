#include <stdio.h>     // fprintf(), fputs()
#include <stdbool.h>   // bool
#include <stdint.h>    // explicitly-sized integral types
#include <stdlib.h>    // EXIT_SUCCESS
#include <argp.h>      // parse_opt(), argp_parse()
#include <curl/curl.h> // curl_easy_*()
#include <sys/stat.h>  // fstat()
#include <string.h>    // strncat()
#include <fcntl.h>     // open()
#include <unistd.h>    // close()

const char * argp_program_version = "ptpst 0.0.1";
const char * argp_program_bug_address = "<halosghost@archlinux.info>";
static const char doc [] = "ptpst -- a libcurl client for pb";

#define BUFFER_SIZE 256
#define FILE_MAX 62914560 // 60 MiB

struct args {
    uint8_t file_count;

    // commands
    bool crt: 1, del: 1, rnd: 1, shr: 1, upd: 1;
    bool verbosity: 1, priv: 2;
};

static error_t
parse_opt (int32_t, char *, struct argp_state *);

// vim: set ts=4 sw=4 et:
