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

#if !defined(__has_attribute)
#   define __has_attribute(x) 0
#endif

#if __has_attribute(pure)
#   define ATTR_PURE __attribute__((pure))
#else
#   define ATTR_PURE
#endif

/* Configuration */
#define FALLBACK_PROVIDER "https://pb.gehidore.net/"
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

static const char gen_help [] =
    "\n"
    "  -h, --help           List this help and exit\n"
    "  -b, --dbpath=PATH    Use the database at PATH\n"
    "  -P, --provider=HOST  Use HOST as the pb provider\n"
    "  -V, --verbose        Output verbosely\n"
    "      --list-lexers    List available lexers and exit\n"
    "      --list-themes    List available themes and exit\n"
    "      --list-formats   List available formatters and exit\n"
    "      --version        List the version and exit\n";

static const char cmds_help [] =
    "Usage: pbpst <operation> [option ...]\n"
    "pbpst -- a simple tool to pastebin from the command-line\n\n"
    "Operations:\n"
    "  -S, --sync           Create a paste\n"
    "  -s, --shorten=URL    Create a redirect to URL\n"
    "  -R, --remove         Delete a paste\n"
    "  -U, --update         Update a paste\n"
    "  -D, --database       Operate on the database\n\n"
    "Options:";

static const char more_info [] =
    "\nRun `pbpst -h` with an operation for help with that operation\n";

static const char sync_help [] =
    "Usage: pbpst {-S --sync} [option ...]\n\n"
    "Options:\n"
    "  -f, --file=FILE      Create a paste from FILE\n"
    "  -l, --lexer=LANG     Lex paste with LANG\n"
    "  -T, --theme=THEME    Style paste with pygments theme THEME\n"
    "  -F, --format=FORM    Format paste for FORM\n"
    "  -e, --extension=EXT  Specify MIME-type as EXT\n"
    "  -L, --line=LINE      Highlight LINE in paste\n"
    "  -p, --private        Return a less-guessable Id for paste\n"
    "  -x, --sunset=SECS    Slate the paste for auto-sunset in SECS seconds\n"
    "  -r, --render         Render paste from rst to HTML\n"
    "  -t, --term           Handle Asciinema videos\n"
    "  -v, --vanity=NAME    Use NAME as a custom Id\n"
    "  -#, --progress       Show a progress bar for the upload\n"
    "  -m, --message=MSG    Use MSG as the note in the database\n";

static const char rem_help [] =
    "Usage: pbpst {-R --remove} [option ...]\n\n"
    "Options:\n"
    "  -u, --uuid=UUID      Remotely delete paste with UUID\n"
    "  -y, --prune          Remotely delete all expired pastes\n";

static const char upd_help [] =
    "Usage: pbpst {-U --update} [option ...]\n\n"
    "Options:\n"
    "  -f, --file=FILE      Use FILE for content of paste\n"
    "  -l, --lexer=LANG     Lex paste with LANG\n"
    "  -F, --format=FORM    Format paste for FORM\n"
    "  -L, --line=LINE      Highlight LINE\n"
    "  -T, --theme=THEME    Style paste with pygments theme THEME\n"
    "  -e, --extension=EXT  Specify MIME-type as EXT\n"
    "  -x, --sunset=SECS    Slate the paste for auto-sunset in SECS seconds\n"
    "  -r, --render         Render paste from rst to HTML\n"
    "  -t, --term           Handle Asciinema videos\n"
    "  -u, --uuid=UUID      Use UUID as authentication credential\n"
    "  -#, --progress       Show a progress bar for the upload\n"
    "  -m, --message=MSG    Use MSG as the note in the database\n";

static const char dbs_help [] =
    "Usage: pbpst {-D --database} [option ...]\n\n"
    "Options:\n"
    "  -i, --init           Initalize a default database (no clobbering)\n"
    "  -H, --providers      List all providers in the database\n"
    "  -q, --query=STR      Search the database for a paste matching STR\n"
    "  -d, --delete=UUID    Locally delete paste with UUID\n"
    "      --set-default    Set provider (from -P) as default\n"
    "  -y, --prune          Locally delete all expired pastes\n";

enum pb_cmd { NON = 0, SNC = 'S', SHR = 's', RMV = 'R', UPD = 'U', DBS = 'D' };

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

static char * db_loc = 0, * swp_db_loc = 0;
extern json_t * mem_db, * pastes, * prov_pastes;
extern const char * def_provider;
extern bool point_of_no_return;

// vim: set ts=4 sw=4 et:
