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
#include <libintl.h>
#include <locale.h>
#include <math.h>
#include "usage.h"

#define _(str) gettext(str)

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

#define BUFFER_SIZE 256

static const char version_str [] = "pbpst 1.3.0\n";

static struct option os [] = {
    /* commands */
    { "sync",         0, 0, 'S' },
    { "remove",       0, 0, 'R' },
    { "update",       0, 0, 'U' },
    { "database",     0, 0, 'D' },

    /* shorten */
    { "shorten",      1, 0, 's' },

    /* sync/upd options */
    { "file",         1, 0, 'f' },
    { "lexer",        1, 0, 'l' },
    { "theme",        1, 0, 'T' },
    { "format",       1, 0, 'F' },
    { "extension",    1, 0, 'e' },
    { "line",         1, 0, 'L' },
    { "private",      0, 0, 'p' }, // sync-only
    { "render",       0, 0, 'r' },
    { "term",         0, 0, 't' },
    { "vanity",       1, 0, 'v' },
    { "help",         0, 0, 'h' },
    { "progress",     0, 0, '#' },
    { "message",      1, 0, 'm' },
    { "sunset",       1, 0, 'x' },
    { "uuid",         1, 0, 'u' }, // rem/upd

    /* db options */
    { "init",         0, 0, 'i' },
    { "providers",    0, 0, 'H' },
    { "query",        1, 0, 'q' },
    { "delete",       1, 0, 'd' },
    { "set-default",  0, 0, 260 },
    { "prune",        0, 0, 'y' }, // rem/db

    /* generic options */
    { "provider",     1, 0, 'P' },
    { "dbpath",       1, 0, 'b' },
    { "verbose",      0, 0, 'V' },
    { "help",         0, 0, 'h' },
    { "version",      0, 0, 256 },
    { "list-lexers",  0, 0, 257 },
    { "list-themes",  0, 0, 258 },
    { "list-formats", 0, 0, 259 },
    { 0,              0, 0, 0   }
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
