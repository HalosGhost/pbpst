#include <getopt.h>    // getopt_long()
#include <stdio.h>     // fprintf(), fputs(), sscanf_s()
#include <stdbool.h>   // bool
#include <stdint.h>    // explicitly-sized integral types
#include <inttypes.h>  // SCNu32
#include <stdlib.h>    // EXIT_SUCCESS
#include <curl/curl.h> // curl_easy_*()
#include <sys/stat.h>  // fstat()
#include <string.h>    // strncat()
#include <fcntl.h>     // open()
#include <unistd.h>    // close()

#define BUFFER_SIZE 256
#define FILE_MAX 67108864 // 64 MiB

static const char version_str [] = "ptpst 0.0.1\n";

static struct option os [] = {
    /* commands */
    { "sync",     no_argument,       0, 'S' },
    { "remove",   no_argument,       0, 'R' },
    { "update",   no_argument,       0, 'U' },

    /* sync/upd options */
    { "shorten",  required_argument, 0, 's' },
    { "file",     required_argument, 0, 'f' },
    { "lexer",    required_argument, 0, 'l' },
    { "line",     required_argument, 0, 'L' },
    { "private",  no_argument,       0, 'p' }, // sync-only
    { "render",   no_argument,       0, 'r' },
    { "vanity",   required_argument, 0, 'v' },
    { "help",     no_argument,       0, 'h' },

    /* for rem and upd */
    { "uuid",     required_argument, 0, 'u' },

    /* generic options */
    { "provider", required_argument, 0, 'P' },
    { "help",     no_argument,       0, 'h' },
    { "verbose",  no_argument,       0, 256 },
    { "version",  no_argument,       0, 257 },
    { 0,          0,                 0, 0   }
};

static const char gen_help [] =
    "\n"
    "  -h, --help          List this help and exit\n"
    "  -P, --provider      Specify an alternative pb host\n"
    "      --verbose       Output verbosely\n"
    "      --version       List the version and exit\n";

static const char cmds_help [] =
    "Usage: ptpst <operation> [option ...]\n"
    "ptpst -- a simple tool to pastebin from the command-line\n\n"
    "Operations:\n"
    "  -S, --sync          Create a paste\n"
    "  -R, --remove        Delete a paste\n"
    "  -U, --update        Update a paste\n\n"
    "Options:";

static const char more_info [] =
    "\nRun `ptpst -h` with an operation for help with that operation\n";

static const char sync_help [] =
    "Usage: ptpst {-S --sync} [option ...]\n\n"
    "Options:\n"
    "  -s, --shorten=URL   Create a redirect to URL instead of pasting\n"
    "  -f, --file=FILE     Create a paste from FILE\n"
    "  -l, --lexer=LANG    Lex paste with LANG\n"
    "  -L, --line=LINE     Highlight LINE in paste\n"
    "  -p, --private       Return a less-guessable Id for paste\n"
    "  -r, --render        Render paste from rst to HTML\n"
    "  -v, --vanity=NAME   Use NAME as a custom Id\n";

static const char rem_help [] =
    "Usage: ptpst {-R --remove} [option ...]\n\n"
    "Options:\n"
    "  -u, --uuid=UUID     Use UUID as authentication credential\n";

static const char upd_help [] =
    "Usage: ptpst {-U --update} [option ...]\n\n"
    "Options:\n"
    "  -s, --shorten=URL   Created a redirect pointing to URL\n"
    "  -f, --file=FILE     Use FILE for content of paste\n"
    "  -l, --lexer=LANG    Lex paste with LANG\n"
    "  -L, --line=LINE     Highlight LINE\n"
    "  -r, --render        Render paste from rst to HTML\n"
    "  -u, --uuid=UUID     Use UUID as authentication credential\n"
    "  -v, --vanity=NAME   Use NAME as a custom Id\n";

enum pb_cmd { NON = 0, SNC = 'S', RMV = 'R', UPD = 'U' }; // DBS

struct ptpst_state {
    char * path, * url, * lexer, * vanity, * uuid, * provider;
    enum pb_cmd cmd;
    uint32_t ln;
    uint64_t help: 16, priv: 16, rend: 16, verb: 16;
};

CURLcode
paste (const struct ptpst_state *);

// vim: set ts=4 sw=4 et: