/*****************************\
* Manage pastes to pb         *
* Author: Sam Stuewe (C) 2015 *
* License: GPLv3              *
\*****************************/

#include "main.h"

struct ptpst_state {
    char * path, * lexer, * vanity, * uuid, * provider;
    enum { NON, SNC, RMV, UPD } cmd; // DBS
    uint32_t line_hl;
    uint64_t help: 16, priv: 16, rend: 32;
};

signed
main (signed argc, char * argv []) {

    if ( argc <= 1 ) {
        fprintf(stderr, "%s%s%s", cmds_help, gen_help, more_info);
        return EXIT_FAILURE;
    }

    struct ptpst_state state = { .path = 0, .lexer = 0, .vanity = 0, .uuid = 0,
                                 .provider = 0, .cmd = NON, .line_hl = 0,
                                 .help = false, .priv = false, .rend = false };

    const char vos [] = "SRUP:hv:s:f:l:L:pru:";
    for ( signed oi = 0, c = getopt_long(argc, argv, vos, os, &oi);
          c != -1; c = getopt_long(argc, argv, vos, os, &oi) ) {

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

            case 'L': sscanf(optarg, "%" SCNu32, &state.line_hl); break;
            case 'r': state.rend = true; break;
            case 'p': state.priv = true; break;
            case 'h': state.help = true; break;
            case 257: printf(version_str); return EXIT_SUCCESS;

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
