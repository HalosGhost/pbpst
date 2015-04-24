/*****************************\
* Manage pastes to pb         *
* Author: Sam Stuewe (C) 2015 *
* License: GPLv3              *
\*****************************/

#include "main.h"

enum pb_cmd { NON = 0, SNC = 'S', RMV = 'R', UPD = 'U' }; // DBS

struct ptpst_state {
    char * path, * url, * lexer, * vanity, * uuid, * provider;
    enum pb_cmd cmd;
    uint32_t ln;
    uint64_t help: 16, priv: 16, rend: 32;
};

signed
main (signed argc, char * argv []) {

    if ( argc <= 1 ) {
        fprintf(stderr, "%s%s%s", cmds_help, gen_help, more_info);
        return EXIT_FAILURE;
    }

    struct ptpst_state state = { .path = 0, .url = 0, .lexer = 0, .vanity = 0,
                                 .uuid = 0, .provider = 0, .cmd = NON, .ln = 0,
                                 .help = false, .priv = false, .rend = false };

    const char vos [] = "SRUP:hv:s:f:l:L:pru:";
    for ( signed oi = 0, c = getopt_long(argc, argv, vos, os, &oi);
          c != -1; c = getopt_long(argc, argv, vos, os, &oi) ) {

        size_t l = optarg ? strlen(optarg) : 0;

        switch ( c ) {
            case 'S': case 'R': case 'U':
                if ( state.cmd ) { goto fail_multiple_cmds; }
                state.cmd = (enum pb_cmd )c; break;

            case 's':
                if ( !state.url && !state.path ) {
                    state.url = (char * )malloc(l + 1);
                    strncpy(state.url, optarg, l);
                } break;

            case 'f':
                if ( !state.path && !state.url ) {
                    state.path = (char * )malloc(l + 1);
                    strncpy(state.path, optarg, l);
               } break;

            case 'l':
                if ( !state.lexer ) {
                    state.lexer = (char * )malloc(l + 1);
                    strncpy(state.lexer, optarg, l);
                } break;

            case 'v':
                if ( !state.vanity ) {
                    state.vanity = (char * )malloc(l + 1);
                    strncpy(state.vanity, optarg, l);
                } break;

            case 'u':
                if ( !state.uuid ) {
                    state.uuid = (char * )malloc(l + 1);
                    strncpy(state.uuid, optarg, l);
                } break;

            case 'P':
                if ( !state.provider ) {
                    state.provider = (char * )malloc(l + 1);
                    strncpy(state.provider, optarg, l);
                } break;

            case 'L': sscanf(optarg, "%" SCNu32, &state.ln); break;
            case 'r': state.rend = true; break;
            case 'p': state.priv = true; break;
            case 'h': state.help = true; break;
            case 257: printf(version_str); return EXIT_SUCCESS;
        }
    }

    if ( state.help ) {
        switch ( state.cmd ) {
            case SNC: printf("%s%s",   sync_help, gen_help);            break;
            case RMV: printf("%s%s",   rem_help,  gen_help);            break;
            case UPD: printf("%s%s",   upd_help,  gen_help);            break;
            case NON: printf("%s%s%s", cmds_help, gen_help, more_info); break;
        } goto success_cleanup;
    }

    success_cleanup:
        if ( state.url )      { free(state.url);      }
        if ( state.path )     { free(state.path);     }
        if ( state.lexer )    { free(state.lexer);    }
        if ( state.vanity )   { free(state.vanity);   }
        if ( state.uuid )     { free(state.uuid);     }
        if ( state.provider ) { free(state.provider); }
        return EXIT_SUCCESS;

    fail_multiple_cmds:
        fputs("Error: you can only run one operation at once\n", stderr);
        return EXIT_FAILURE;
}
// vim: set ts=4 sw=4 et:
