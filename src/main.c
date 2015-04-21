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

    struct cmd_bools cb = { false, false, false, false };

    for ( signed c = 0, oi = 0; c != -1;
          c = getopt_long(argc, argv, "SRUP:hv", cmds, &oi) ) {

        switch ( c ) {
            case 'S':
                cb.sync = true;
                break;

            case 'R':
                cb.rem = true;
                break;

            case 'U':
                cb.upd = true;
                break;

            case 'P': case 256:
                fprintf(stderr, "Not Yet Implemented: `-%c`\n", c);
                return EXIT_FAILURE;

            case 257:
                printf(version_str);
                return EXIT_SUCCESS;

            case 'h':
                cb.hlp = true;
                break;
        }
    }

    if ( (cb.sync && cb.rem) || (cb.sync && cb.upd) || (cb.rem && cb.upd) ) {
        fputs("Only one operation can be run at once\n", stderr);
        return EXIT_FAILURE;
    }

    if ( cb.hlp ) {
        printf(cb.sync ? sync_help :
               cb.rem  ? rem_help  :
               cb.upd  ? upd_help  : cmds_help);
    } return EXIT_SUCCESS;
}
// vim: set ts=4 sw=4 et:
