/*****************************\
* Manage pastes to pb         *
* Author: Sam Stuewe (C) 2015 *
* License: GPLv3              *
\*****************************/

#include "main.h"

struct cmd_bools {
    bool sync: 2, rem: 2, upd: 2, hlp: 2;
};

signed
main (signed argc, char * argv []) {

    if ( argc <= 1 ) {
        fprintf(stderr, cmds_help);
        return EXIT_FAILURE;
    }

    enum ptpst_cmd {
        NONE, SYNC, REMOVE, UPDATE //, DATABASE
    };

    enum ptpst_cmd cmd = NONE;
    for ( signed oi = 0, c = getopt_long(argc, argv, "SRUP:hvsflLpru", os, &oi);
          c != -1; c = getopt_long(argc, argv, "SRUP:hvsflLpru", os, &oi) ) {

        switch ( c ) {
            case 'S':
                if ( cmd != NONE ) { goto fail_multiple_cmds; }
                cmd = SYNC; break;

            case 'R':
                if ( cmd != NONE ) { goto fail_multiple_cmds; }
                cmd = REMOVE; break;

            case 'U':
                if ( cmd != NONE ) { goto fail_multiple_cmds; }
                cmd = UPDATE; break;

            case 257: printf(version_str); return EXIT_SUCCESS;

            case 'h':
                printf(cmd == SYNC   ? sync_help :
                       cmd == REMOVE ? rem_help  :
                       cmd == UPDATE ? upd_help  : cmds_help);
                return EXIT_SUCCESS;

            default:
                fputs("Not Yet Implemented\n", stderr);
                return EXIT_FAILURE;
        }
    } return EXIT_SUCCESS;

    fail_multiple_cmds:
        fputs("Error: you can only run one operation at once\n", stderr);
        return EXIT_FAILURE;
}
// vim: set ts=4 sw=4 et:
