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
#include <time.h>      // localtime(), time(), time_t
#include <jansson.h>

#define BUFFER_SIZE 256
#define FILE_MAX 67108864 // 64 MiB

static const char version_str [] = "pbpst 0.6.0\n";

static struct option os [] = {
    /* commands */
    { "sync",         no_argument,       0, 'S' },
    { "remove",       no_argument,       0, 'R' },
    { "update",       no_argument,       0, 'U' },
    { "database",     no_argument,       0, 'D' },

    /* sync/upd options */
    { "shorten",      required_argument, 0, 's' },
    { "file",         required_argument, 0, 'f' },
    { "lexer",        required_argument, 0, 'l' },
    { "theme",        required_argument, 0, 't' },
    { "extension",    required_argument, 0, 'e' },
    { "line",         required_argument, 0, 'L' },
    { "private",      no_argument,       0, 'p' }, // sync-only
    { "render",       no_argument,       0, 'r' },
    { "vanity",       required_argument, 0, 'v' },
    { "help",         no_argument,       0, 'h' },
    { "progress",     no_argument,       0, '#' },
    { "message",      required_argument, 0, 'm' },
    { "sunset",       required_argument, 0, 'x' },
    { "uuid",         required_argument, 0, 'u' }, // rem/upd

    /* db options */
    { "init",         no_argument,       0, 'i' },
    { "query",        required_argument, 0, 'q' },
    { "delete",       required_argument, 0, 'd' },
    { "prune",        no_argument,       0, 'y' }, // rem/db

    /* generic options */
    { "provider",     required_argument, 0, 'P' },
    { "dbpath",       required_argument, 0, 'b' },
    { "verbose",      no_argument,       0, 'V' },
    { "help",         no_argument,       0, 'h' },
    { "version",      no_argument,       0, 256 },
    { "list-lexers",  no_argument,       0, 257 },
    { "list-themes",  no_argument,       0, 258 },
    { 0,              0,                 0, 0   }
};

static const char gen_help [] =
    "\n"
    "  -h, --help           List this help and exit\n"
    "  -b, --dbpath=PATH    Use the database at PATH\n"
    "  -P, --provider       Specify an alternative pb host\n"
    "  -V, --verbose        Output verbosely\n"
    "      --list-lexers    List available lexers and exit\n"
    "      --list-themes    List available themes and exit\n"
    "      --version        List the version and exit\n";

static const char cmds_help [] =
    "Usage: pbpst <operation> [option ...]\n"
    "pbpst -- a simple tool to pastebin from the command-line\n\n"
    "Operations:\n"
    "  -S, --sync           Create a paste\n"
    "  -R, --remove         Delete a paste\n"
    "  -U, --update         Update a paste\n"
    "  -D, --database       Operate on the database\n\n"
    "Options:";

static const char more_info [] =
    "\nRun `pbpst -h` with an operation for help with that operation\n";

static const char sync_help [] =
    "Usage: pbpst {-S --sync} [option ...]\n\n"
    "Options:\n"
    "  -s, --shorten=URL    Create a redirect to URL instead of pasting\n"
    "  -f, --file=FILE      Create a paste from FILE\n"
    "  -l, --lexer=LANG     Lex paste with LANG\n"
    "  -t, --theme=THEME    Style paste with pygments theme THEME\n"
    "  -e, --extension=EXT  Specify MIME-type as EXT\n"
    "  -L, --line=LINE      Highlight LINE in paste\n"
    "  -p, --private        Return a less-guessable Id for paste\n"
    "  -x, --sunset=SECS    Slate the paste for auto-sunset in SECS seconds\n"
    "  -r, --render         Render paste from rst to HTML\n"
    "  -v, --vanity=NAME    Use NAME as a custom Id\n"
    "  -#, --progress       Show a progress bar for the upload\n"
    "  -m, --message=MSG    Use MSG as the note in the database\n";

static const char rem_help [] =
    "Usage: pbpst {-R --remove} [option ...]\n\n"
    "Options:\n"
    "  -u, --uuid=UUID      Use UUID as authentication credential\n"
    "  -y, --prune          Remotely delete all expired pastes\n";

static const char upd_help [] =
    "Usage: pbpst {-U --update} [option ...]\n\n"
    "Options:\n"
    "  -f, --file=FILE      Use FILE for content of paste\n"
    "  -l, --lexer=LANG     Lex paste with LANG\n"
    "  -L, --line=LINE      Highlight LINE\n"
    "  -t, --theme=THEME    Style paste with pygments theme THEME\n"
    "  -e, --extension=EXT  Specify MIME-type as EXT\n"
    "  -x, --sunset=SECS    Slate the paste for auto-sunset in SECS seconds\n"
    "  -r, --render         Render paste from rst to HTML\n"
    "  -u, --uuid=UUID      Use UUID as authentication credential\n"
    "  -v, --vanity=NAME    Use NAME as a custom Id\n"
    "  -#, --progress       Show a progress bar for the upload\n"
    "  -m, --message=MSG    Use MSG as the note in the database\n";

static const char dbs_help [] =
    "Usage: pbpst {-D --database} [option ...]\n\n"
    "Options:\n"
    "  -i, --init           Initalize a default database (no clobbering)\n"
    "  -q, --query=STR      Search the database for a paste matching STR\n"
    "  -d, --delete=UUID    Manually delete the paste with UUID\n"
    "  -y, --prune          Locally delete all expired pastes\n";

enum pb_cmd { NON = 0, SNC = 'S', RMV = 'R', UPD = 'U', DBS = 'D' };

static const char * opts_for [] = {
    [NON] = "SRUDh",
    [SNC] = "SRUDhP:Vb:s:f:l:t:e:L:px:rv:#m:",
    [RMV] = "SRUDhP:Vb:u:y",
    [UPD] = "SRUDhP:Vb:f:l:L:t:e:x:ru:v:#m:",
    [DBS] = "SRUDhP:Vb:id:y"
};

extern struct pbpst_state {
    char * path, * url, * lexer, * vanity, * uuid, * provider,
         * query, * del, * dbfile, * msg, * theme, * ext, * ln, * secs;
    enum pb_cmd cmd;
    uint16_t help, priv, rend, init: 8, prun: 8, verb: 8, prog: 8, llex: 8, lthm: 8;
} state;

bool
pbpst_test_options (const struct pbpst_state *);

signed
pbpst_dispatch (const struct pbpst_state *);

void
signal_handler (signed);

void
pbpst_cleanup (void);

static char * db_loc = 0, * swp_db_loc = 0;
extern json_t * mem_db, * pastes, * prov_pastes;
extern const char * const sys_siglist [];
extern const char * def_provider;

static const char signal_err [] =
    "pbpst: Received %s\x1b[?25h\n"
    "pbpst: Note that your swap db will need to be manually checked\n";

// vim: set ts=4 sw=4 et:
