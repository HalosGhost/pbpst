/*****************************\
* Manage pastes to pb         *
* Author: Sam Stuewe (C) 2015 *
* License: GPLv3              *
\*****************************/

#include "main.h"

struct ptpst_state {
    enum { NONE, SYNC, REMOVE, UPDATE/*, DATABASE */ } cmd;
    bool help;
};

signed
main (signed argc, char * argv []) {

    if ( argc <= 1 ) {
        fprintf(stderr, "%s%s%s", cmds_help, gen_help, more_info);
        return EXIT_FAILURE;
    }

    struct ptpst_state state = { .cmd = NONE, .help = false };
    for ( signed oi = 0, c = getopt_long(argc, argv, "SRUP:hvsflLpru", os, &oi);
          c != -1; c = getopt_long(argc, argv, "SRUP:hvsflLpru", os, &oi) ) {

        switch ( c ) {
            case 'S':
                if ( state.cmd != NONE ) { goto fail_multiple_cmds; }
                state.cmd = SYNC; break;

            case 'R':
                if ( state.cmd != NONE ) { goto fail_multiple_cmds; }
                state.cmd = REMOVE; break;

            case 'U':
                if ( state.cmd != NONE ) { goto fail_multiple_cmds; }
                state.cmd = UPDATE; break;

            case 257: printf(version_str); return EXIT_SUCCESS;

            case 'h': state.help = true; break;

            default:
                fputs("Not Yet Implemented\n", stderr);
                return EXIT_FAILURE;
        }
    }

    if ( state.help ) {
        switch ( state.cmd ) {
            case SYNC:   printf("%s%s",   sync_help, gen_help); break;
            case REMOVE: printf("%s%s",   rem_help,  gen_help); break;
            case UPDATE: printf("%s%s",   upd_help,  gen_help); break;
            case NONE:   printf("%s%s%s", cmds_help, gen_help,
                                more_info);                     break;
        } return EXIT_SUCCESS;
    }

    return EXIT_SUCCESS;

    fail_multiple_cmds:
        fputs("Error: you can only run one operation at once\n", stderr);
        return EXIT_FAILURE;
}
// vim: set ts=4 sw=4 et:
