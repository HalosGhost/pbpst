#pragma once

#include <stdio.h>
#include <libintl.h>
#include <locale.h>

#define _(str) gettext(str)

enum pb_cmd { NON = 0, SNC = 'S', SHR = 's', RMV = 'R', UPD = 'U', DBS = 'D' };

#define OPERATIONS \
    X('S', "sync", 0, _("Create a paste")) \
    X('s', "shorten", "URL", _("Create a redirect to URL")) \
    X('R', "remove", 0, _("Remove a paste")) \
    X('U', "update", 0, _("Update a paste")) \
    X('D', "database", 0, _("Operate on the database"))

#define SNC_OPTIONS \
    X('f', "file", "FILE", _("Use FILE to create paste")) \
    X('l', "lexer", "LANG", _("Lex paste for LANG")) \
    X('T', "theme", "THEME", _("Use THEME for style")) \
    X('F', "format", "FORM", _("Format paste for FORM")) \
    X('e', "extension", "EXT", _("Specify file extension as EXT")) \
    X('L', "line", "LINE", _("Highlight LINE in paste")) \
    X('p', "private", 0, _("Return a less-guessable paste ID")) \
    X('x', "sunset", "TIME", _("Set the paste to auto-deletion in TIME")) \
    X('r', "render", 0, _("Render paste (from md or rst) to HTML")) \
    X('t', "term", 0, _("Show Asciinema presentation")) \
    X('#', "progress", 0, _("Show an upload progress bar")) \
    X('m', "message", "MSG", _("Save MSG as a note for the paste")) \
    X('v', "vanity", "NAME", _("Use NAME as a custom ID"))

#define UPD_OPTIONS \
    X('u', "uuid", "UUID", _("Target paste with UUID"))

#define RMV_OPTIONS \
    X('y', "prune", 0, _("Delete all expired pastes"))

#define DBS_OPTIONS \
    X('i', "init", 0, _("Initialize a default database")) \
    X('H', "providers", 0, _("List all providers in the database")) \
    X('q', "query", "STR", _("Search the database for a paste matching STR")) \
    X('d', "delete", "UUID",  _("Target paste with UUID")) \
    X(260, "set-default", 0, _("Set provider (from `-P`) as default"))

#define GEN_OPTIONS \
    X('h', "help", 0, _("List this help and exit")) \
    X('b', "dbpath", "PATH", _("Use the database at PATH")) \
    X('P', "provider", "HOST", _("Use HOST as the pb provider")) \
    X('V', "verbose", 0, _("Output verbosely")) \
    X(256, "version", 0, _("List the version and exit")) \
    X(257, "list-lexers", 0, _("List available lexers and exit")) \
    X(258, "list-themes", 0, _("List available themes and exit")) \
    X(259, "list-formats", 0, _("List available formatters and exit"))

struct optdesc {
    char * full, * arg, * desc;
    signed long flag;
};

signed
print_usage (const enum pb_cmd, signed);

signed
enum_args (enum pb_cmd, FILE *);
