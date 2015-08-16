#pragma once

#include <getopt.h>    // getopt_long()
#include <stdio.h>     // fprintf(), snprintf(), fputs(), sscanf_s(), remove()
#include <signal.h>    // sigaction()
#include <stdbool.h>   // bool
#include <stdint.h>    // explicitly-sized integral types
#include <inttypes.h>  // SCNu32
#include <stdlib.h>    // EXIT_SUCCESS, EXIT_FAILURE
#include <curl/curl.h> // curl_easy_*()
#include <sys/stat.h>  // fstat(), open(), mkdir()
#include <string.h>    // strerror()
#include <fcntl.h>     // open()
#include <unistd.h>    // close()
#include <errno.h>     // errno
#include <jansson.h>

#define BUFFER_SIZE 256
#define FILE_MAX 67108864 // 64 MiB

static const char version_str [] = "pbpst 0.6.0\n";

static struct option os [] = {
    /* commands */
    { "sync",      no_argument,       0, 'S' },
    { "remove",    no_argument,       0, 'R' },
    { "update",    no_argument,       0, 'U' },
    { "database",  no_argument,       0, 'D' },

    /* sync/upd options */
    { "shorten",   required_argument, 0, 's' },
    { "file",      required_argument, 0, 'f' },
    { "lexer",     required_argument, 0, 'l' },
    { "line",      required_argument, 0, 'L' },
    { "private",   no_argument,       0, 'p' }, // sync-only
    { "render",    no_argument,       0, 'r' },
    { "vanity",    required_argument, 0, 'v' },
    { "help",      no_argument,       0, 'h' },
    { "progress",  no_argument,       0, '#' },
    { "message",   required_argument, 0, 'm' },

    /* for rem and upd */
    { "uuid",      required_argument, 0, 'u' },

    /* db options */
    { "init",      no_argument,       0, 'i' },
    { "query",     required_argument, 0, 'q' },
    { "delete",    required_argument, 0, 'd' },

    /* generic options */
    { "provider",  required_argument, 0, 'P' },
    { "help",      no_argument,       0, 'h' },
    { "dbpath",    required_argument, 0, 'b' },
    { "verbose",   no_argument,       0, 256 },
    { "version",   no_argument,       0, 257 },
    { 0,           0,                 0, 0   }
};

static const char gen_help [] =
    "\n"
    "  -h, --help          List this help and exit\n"
    "  -b, --dbpath=PATH   Use the database at PATH\n"
    "  -P, --provider      Specify an alternative pb host\n"
    "      --verbose       Output verbosely\n"
    "      --version       List the version and exit\n";

static const char cmds_help [] =
    "Usage: pbpst <operation> [option ...]\n"
    "pbpst -- a simple tool to pastebin from the command-line\n\n"
    "Operations:\n"
    "  -S, --sync          Create a paste\n"
    "  -R, --remove        Delete a paste\n"
    "  -U, --update        Update a paste\n"
    "  -D, --database      Operate on the database\n\n"
    "Options:";

static const char more_info [] =
    "\nRun `pbpst -h` with an operation for help with that operation\n";

static const char sync_help [] =
    "Usage: pbpst {-S --sync} [option ...]\n\n"
    "Options:\n"
    "  -s, --shorten=URL   Create a redirect to URL instead of pasting\n"
    "  -f, --file=FILE     Create a paste from FILE\n"
    "  -l, --lexer=LANG    Lex paste with LANG\n"
    "  -L, --line=LINE     Highlight LINE in paste\n"
    "  -p, --private       Return a less-guessable Id for paste\n"
    "  -r, --render        Render paste from rst to HTML\n"
    "  -v, --vanity=NAME   Use NAME as a custom Id\n"
    "  -#, --progress      Show a progress bar for the upload\n"
    "  -m, --message=MSG   Use MSG as the note in the database\n";

static const char rem_help [] =
    "Usage: pbpst {-R --remove} [option ...]\n\n"
    "Options:\n"
    "  -u, --uuid=UUID     Use UUID as authentication credential\n";

static const char upd_help [] =
    "Usage: pbpst {-U --update} [option ...]\n\n"
    "Options:\n"
    "  -f, --file=FILE     Use FILE for content of paste\n"
    "  -l, --lexer=LANG    Lex paste with LANG\n"
    "  -L, --line=LINE     Highlight LINE\n"
    "  -r, --render        Render paste from rst to HTML\n"
    "  -u, --uuid=UUID     Use UUID as authentication credential\n"
    "  -v, --vanity=NAME   Use NAME as a custom Id\n"
    "  -#, --progress      Show a progress bar for the upload\n"
    "  -m, --message=MSG   Use MSG as the note in the database\n";

static const char dbs_help [] =
    "Usage: pbpst {-D --database} [option ...]\n\n"
    "Options:\n"
    "  -i, --init          Initalize a default database (no clobbering)\n"
    "  -q, --query=STR     Search the database for a paste matching STR\n"
    "  -d, --delete=UUID   Manually delete the paste with UUID\n";

enum pb_cmd { NON = 0, SNC = 'S', RMV = 'R', UPD = 'U', DBS = 'D' };

extern struct pbpst_state {
    char * path, * url, * lexer, * vanity, * uuid, * provider,
         * query, * del, * dbfile, * msg;
    enum pb_cmd cmd;
    uint32_t ln;
    uint16_t help, priv, rend: 8, init: 8, verb: 8, prog: 8;
} state;

bool
pbpst_test_options (const struct pbpst_state *);

signed
pbpst_dispatch (const struct pbpst_state *);

void
signal_handler (signed);

static char * db_loc = 0, * swp_db_loc = 0;
extern json_t * mem_db, * pastes, * prov_pastes;
extern const char * const sys_siglist [];
extern const char * def_provider;

// vim: set ts=4 sw=4 et:
