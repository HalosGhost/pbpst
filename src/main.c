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

    signed exit_status = EXIT_SUCCESS;

    struct ptpst_state state = { .path = 0, .url = 0, .lexer = 0, .vanity = 0,
                                 .uuid = 0, .provider = 0, .cmd = NON, .ln = 0,
                                 .help = false, .priv = false, .rend = false };

    const char vos [] = "SRUP:hv:s:f:l:L:pru:";
    for ( signed oi = 0, c = getopt_long(argc, argv, vos, os, &oi);
          c != -1; c = getopt_long(argc, argv, vos, os, &oi) ) {

        size_t l = optarg ? strlen(optarg) : 0;
        char ** state_var = (c == 's' ? &state.url      :
                             c == 'f' ? &state.path     :
                             c == 'l' ? &state.lexer    :
                             c == 'v' ? &state.vanity   :
                             c == 'u' ? &state.uuid     :
                             c == 'P' ? &state.provider : 0);

        switch ( c ) {
            case 'S': case 'R': case 'U':
                if ( state.cmd ) {
                    fputs("Error: you can only run one operation at a time\n",
                          stderr);
                    exit_status = EXIT_FAILURE; goto cleanup;
                } state.cmd = (enum pb_cmd )c; break;

            case 's': case 'f': case 'l': case 'v': case 'u': case 'P':
                *state_var = (char * )malloc(l + 1);
                strncpy(*state_var, optarg, l);
                break;

            case 'L': sscanf(optarg, "%" SCNu32, &state.ln); break;
            case 'r': state.rend = true; break;
            case 'p': state.priv = true; break;
            case 'h': state.help = true; break;
            case 257: printf(version_str); goto cleanup;
        }
    }

    /**
     * TODO:
     **
     * validate that all options passed are legitimate for the operation
     * write and call the libcurl functions to actually interact with pb
     */

    if ( state.help ) {
        switch ( state.cmd ) {
            case SNC: printf("%s%s",   sync_help, gen_help);            break;
            case RMV: printf("%s%s",   rem_help,  gen_help);            break;
            case UPD: printf("%s%s",   upd_help,  gen_help);            break;
            case NON: printf("%s%s%s", cmds_help, gen_help, more_info); break;
        } goto cleanup;
    }

    cleanup:
        if ( state.url )      { free(state.url);      }
        if ( state.path )     { free(state.path);     }
        if ( state.lexer )    { free(state.lexer);    }
        if ( state.vanity )   { free(state.vanity);   }
        if ( state.uuid )     { free(state.uuid);     }
        if ( state.provider ) { free(state.provider); }
        return exit_status;
}
// vim: set ts=4 sw=4 et:
