#pragma once

#include <getopt.h>    // getopt_long()
#include <stdio.h>     // fprintf(), snprintf(), fputs(), sscanf_s(), remove()
#include <signal.h>    // sigaction()
#include <stdbool.h>   // bool
#include <stdint.h>    // explicitly-sized integral types
#include <inttypes.h>  // SCNu32
#include <stdlib.h>    // EXIT_SUCCESS, EXIT_FAILURE
#include <curl/curl.h> // curl_easy_*()
#include <sys/stat.h>  // stat(), open(), mkdir()
#include <string.h>    // strerror()
#include <fcntl.h>     // open()
#include <unistd.h>    // close()
#include <errno.h>     // errno
#include <time.h>      // localtime(), time(), time_t
#include <stdnoreturn.h>
#include <jansson.h>
#include <math.h>
#include "usage.h"

#if !defined(__has_attribute)
#   define __has_attribute(x) 0
#endif

#if __has_attribute(pure)
#   define ATTR_PURE __attribute__((pure))
#else
#   define ATTR_PURE
#endif

/* Configuration */
#define FALLBACK_PROVIDER "https://ptpb.pw/"
#define PB_FILE_MAX 67108864
#define PREFIX "/usr/local"

#define BUFFER_SIZE 256

static const char version_str [] = "pbpst 1.4.1\n";

static struct option os [] = {
    #define X(sh, ln, arg, desc) { ln, (bool )arg, 0, sh },
    OPERATIONS
    SNC_OPTIONS
    UPD_OPTIONS
    RMV_OPTIONS
    DBS_OPTIONS
    GEN_OPTIONS
    #undef X
    { 0, 0, 0, 0 }
};

static const char * opts_for [] = {
    [NON] = "SRUDhs:P:b:",
    [SNC] = "SRUDhP:Vb:f:l:T:F:e:L:px:rtv:#m:",
    [SHR] = "s:P:Vb:",
    [RMV] = "SRUDhP:Vb:u:y",
    [UPD] = "SRUDhP:Vb:f:l:L:T:F:e:x:rtu:#m:",
    [DBS] = "SRUDhP:Vb:id:yq:H"
};

extern struct pbpst_state {
    char * path, * url, * lexer, * vanity, * uuid, * provider, * format,
         * query, * del, * dbfile, * msg, * theme, * ext, * ln, * secs;
    enum pb_cmd cmd;
    uint16_t help: 5, priv: 5, rend: 2, term: 2, init: 2, prun: 2, verb: 2,
             prog: 2, llex: 2, lthm: 2, lfrm: 2, lspv: 2, dfpv: 2;
} state;

bool ATTR_PURE
pbpst_test_options (const struct pbpst_state *);

signed
pbpst_dispatch (const struct pbpst_state *);

noreturn void
signal_handler (signed);

void
pbpst_cleanup (void);

signed
pbpst_err (const char *);

static char * db_loc = 0, * swp_db_loc = 0;
extern json_t * mem_db, * pastes, * prov_pastes;
extern const char * def_provider;
extern bool point_of_no_return;

// vim: set ts=4 sw=4 et:
