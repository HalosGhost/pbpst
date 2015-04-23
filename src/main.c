/*****************************\
* Manage pastes to pb         *
* Author: Sam Stuewe (C) 2015 *
* License: GPLv3              *
\*****************************/

#include "main.h"

struct ptpst_state {
    enum { NON, SNC, RMV, UPD/*, DBS */ } cmd;
    bool help;
};

signed
main (signed argc, char * argv []) {

    if ( argc <= 1 ) {
        fprintf(stderr, "%s%s%s", cmds_help, gen_help, more_info);
        return EXIT_FAILURE;
    }

    struct ptpst_state state = { .cmd = NON, .help = false };
    for ( signed oi = 0, c = getopt_long(argc, argv, "SRUP:hvsflLpru", os, &oi);
          c != -1; c = getopt_long(argc, argv, "SRUP:hvsflLpru", os, &oi) ) {

        switch ( c ) {
            case 'S':
                if ( state.cmd != NON ) { goto fail_multiple_cmds; }
                state.cmd = SNC; break;

            case 'R':
                if ( state.cmd != NON ) { goto fail_multiple_cmds; }
                state.cmd = RMV; break;

            case 'U':
                if ( state.cmd != NON ) { goto fail_multiple_cmds; }
                state.cmd = UPD; break;

            case 257: printf(version_str); return EXIT_SUCCESS;

            case 'h': state.help = true; break;

            default:
                fputs("Not Yet Implemented\n", stderr);
                return EXIT_FAILURE;
        }
    }

    if ( state.help ) {
        switch ( state.cmd ) {
            case SNC: printf("%s%s",   sync_help, gen_help);            break;
            case RMV: printf("%s%s",   rem_help,  gen_help);            break;
            case UPD: printf("%s%s",   upd_help,  gen_help);            break;
            case NON: printf("%s%s%s", cmds_help, gen_help, more_info); break;
        } return EXIT_SUCCESS;
    }

    return EXIT_SUCCESS;

    fail_multiple_cmds:
        fputs("Error: you can only run one operation at once\n", stderr);
        return EXIT_FAILURE;
}
// vim: set ts=4 sw=4 et:
