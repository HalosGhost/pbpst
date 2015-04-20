/*****************************\
* Manage pastes to pb         *
* Author: Sam Stuewe (C) 2015 *
* License: GPLv3              *
\*****************************/

#include "main.h"

signed
main (signed argc, char * argv []) {

    if ( argc <= 1 ) {
        fprintf(stderr, cmds_help);
        return EXIT_FAILURE;
    }

    for ( signed c = 0, oi = 0; c != -1;
          c = getopt_long(argc, argv, "SRUP:hv", cmds, &oi) ) {

        switch ( c ) {
            case 'S': case 'R': case 'U': case 'P': case 256:
                fputs("Not Yet Implemented\n", stderr);
                return EXIT_FAILURE;

            case 257:
                printf(version_str);
                return EXIT_SUCCESS;

            case 'h':
                printf(cmds_help);
                return EXIT_SUCCESS;
        }
    } return EXIT_SUCCESS;
}
// vim: set ts=4 sw=4 et:
