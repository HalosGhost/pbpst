#pragma once

#include <stdio.h>

enum pb_cmd { NON = 0, SNC = 'S', SHR = 's', RMV = 'R', UPD = 'U', DBS = 'D' };

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

signed
print_usage (enum pb_cmd, signed);
