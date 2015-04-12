/*****************************\
* Manage pastes to pb         *
* Author: Sam Stuewe (C) 2015 *
* License: GPLv3              *
\*****************************/

#include "main.h"

signed
main (signed argc, char * argv []) {

    static const struct argp_option os [] = {
        { 0,          0,   0,      0, "Commands:",                1 },
        { "create",   'c', 0,      0, "Create new paste",         0 },
        { "delete",   'd', 0,      0, "Delete paste",             0 },
        { "render",   'r', 0,      0, "Render a paste",           0 },
        { "shorten",  's', 0,      0, "Create a shortened URL",   0 },
        { "update",   'u', 0,      0, "Update paste",             0 },
        { 0,          0,   0,      0, "Options:",                 2 },
        { "file",     'f', "FILE", 0, "Use FILE for content",     0 },
        { "uuid",     'U', "UUID", 0, "Operate on paste at UUID", 0 },
        { "vanity",   'v', "STR",  0, "Use STR as vanity string", 0 },
        { "verbose",  160, 0,      0, "Make libcurl verbose",     0 },
        { "provider", 'p', "HOST", 0, "Use HOST pb as provider",  0 },
        { "lexer",    'l', "STR",  0, "Use lexer STR",            0 },
        { "line",     'L', "NUM",  0, "Highlight line NUM",       0 },
        { "private",  'P', 0,      0, "Make paste private",       0 },
        { 0,          0,   0,      0, 0,                          0 }
    };

    static struct argp argp = { os, parse_opt, 0, doc, 0, 0, 0 };
    argp_parse(&argp, argc, argv, 0, 0 , 0);
    return EXIT_SUCCESS;
}

static error_t
parse_opt (int32_t key, char * arg, struct argp_state * state) {

    switch ( key ) {
        case 'c': case 'd': case 'r': case 's': case 'u':
            fputs("Commands are not yet implemented\n", stderr);
            break;

        case 'f': case 'U': case 'v': case 160: case 'p': case 'l':
        case 'L': case 'P':
            fputs("Modification options are not yet implemented\n", stderr);
            break;

        default:
            return ARGP_ERR_UNKNOWN;
    } return 0;
}

// vim: set ts=4 sw=4 et:
