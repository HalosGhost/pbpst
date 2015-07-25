/*****************************\
* Manage pastes to pb         *
* Author: Sam Stuewe (C) 2015 *
* License: GPLv3              *
\*****************************/

#include "main.h"
#include "pb.h"
#include "pbpst_db.h"

signed
main (signed argc, char * argv []) {

    if ( argc <= 1 ) {
        fprintf(stderr, "%s%s%s", cmds_help, gen_help, more_info);
        return EXIT_FAILURE;
    }

    signed exit_status = EXIT_SUCCESS;

    struct pbpst_state state = { .path = 0, .url = 0, .lexer = 0, .vanity = 0,
                                 .uuid = 0, .provider = 0, .dbfile = 0,
                                 .query = 0, .del = 0, .cmd = NON, .ln = 0,
                                 .help = false, .priv = false, .rend = false,
                                 .verb = false, .ncnf = false, .prog = false};

    const char vos [] = "SRUDP:hv:s:f:l:L:pru:b:q:d:n#";
    for ( signed oi = 0, c = getopt_long(argc, argv, vos, os, &oi);
          c != -1; c = getopt_long(argc, argv, vos, os, &oi) ) {

        size_t l = optarg ? strlen(optarg) : 0;
        char ** state_var = 0;

        switch ( c ) {
            case 'S': case 'R': case 'U': case 'D':
                if ( state.cmd ) {
                    fputs("Error: you can only run one operation at a time\n",
                          stderr);
                    exit_status = EXIT_FAILURE; goto cleanup;
                } state.cmd = (enum pb_cmd )c; break;

            case 's': state_var = &state.url;      goto svcase;
            case 'f': state_var = &state.path;     goto svcase;
            case 'l': state_var = &state.lexer;    goto svcase;
            case 'v': state_var = &state.vanity;   goto svcase;
            case 'u': state_var = &state.uuid;     goto svcase;
            case 'q': state_var = &state.query;    goto svcase;
            case 'd': state_var = &state.del;      goto svcase;
            case 'P': state_var = &state.provider; goto svcase;
            case 'b': state_var = &state.dbfile;   goto svcase;
            svcase:
                *state_var = (char * )malloc(l + 1);
                strncpy(*state_var, optarg, l);
                break;

            case 'L': sscanf(optarg, "%" SCNu32, &state.ln); break;
            case '#': state.prog = true; break;
            case 'n': state.ncnf = true; break;
            case 'r': state.rend = true; break;
            case 'p': state.priv = true; break;
            case 'h': state.help = true; break;
            case 256: state.verb = true; break;
            case 257: printf(version_str); goto cleanup;
            default:  goto cleanup;
        }
    }

    if ( state.help ) {
        switch ( state.cmd ) {
            case SNC: printf("%s%s",   sync_help, gen_help);            break;
            case RMV: printf("%s%s",   rem_help,  gen_help);            break;
            case UPD: printf("%s%s",   upd_help,  gen_help);            break;
            case DBS: printf("%s%s",   dbs_help,  gen_help);            break;
            case NON: printf("%s%s%s", cmds_help, gen_help, more_info); break;
        } goto cleanup;
    }

    switch ( state.cmd ) {
        case SNC:
            if ( (state.url && (state.path || state.lexer  || state.rend ||
                                state.ln   || state.vanity )) || state.uuid ) {
                fprintf(stderr, "Error: erroneous option. See `%s -Sh`\n",
                        argv[0]); goto cleanup;
            } break;

        case RMV:
            if ( state.path || state.url  || state.lexer || state.vanity ||
                 state.ln   || state.priv || state.rend  || !state.uuid  ||
                 state.prog ) {
                fprintf(stderr, "Error: erroneous option. See `%s -Rh`\n",
                        argv[0]); goto cleanup;
            } break;

        case UPD:
            if ( !state.uuid || state.priv ) {
                fprintf(stderr, "Error: erroneous option. See `%s -Uh`\n",
                        argv[0]); goto cleanup;
            } break;

        case DBS:
            if ( state.prog || (state.query && (state.ncnf || state.del)) ) {
                fprintf(stderr, "Error: erroneos option. See `%s -Dh`\n",
                        argv[0]); goto cleanup;
            } break;

        case NON:
            printf("%s%s%s", cmds_help, gen_help, more_info);
            goto cleanup;
    }

    signed lckst = db_lockfile_init();
    if ( lckst != EXIT_SUCCESS ) {
        exit_status = lckst;
        goto cleanup;
    }

    /**
     * TODO
     **
     * Check for swapdb (error out with message if it exists)
     * Locate the database (handling the does-not-exist case)
     * Read-in the database
     * Set state.provider according to the db (falling back on ptpb if needed)
     */

    // Make sure we have a sane provider string
    if ( !state.provider ) {
        size_t len = strlen("https://ptpb.pw/") + 1;
        state.provider = malloc(len);
        if ( !state.provider ) {
            exit_status = CURLE_OUT_OF_MEMORY;
            goto cleanup;
        } snprintf(state.provider, len, "https://ptpb.pw/");
    } else {
        size_t len = strlen(state.provider);
        if ( state.provider[len - 1] != '/' ) {
            state.provider = realloc(state.provider, len + 2);
            char * suffix = state.provider + len;
            *suffix = '/'; *(suffix + 1) = '\0';
        }
    }

    exit_status = pbpst_dispatch(&state);

    /**
     * TODO
     **
     * Write resultant in-memory db to swapdb
     * move swapdb to db location
     * remove lockfile
     */

    cleanup:
        db_lockfile_cleanup();
        free(state.url);
        free(state.path);
        free(state.lexer);
        free(state.vanity);
        free(state.uuid);
        free(state.query);
        free(state.del);
        free(state.provider);
        free(state.dbfile);
        return exit_status;
}

signed
pbpst_dispatch (const struct pbpst_state * state) {
    switch ( state->cmd ) {
        case SNC:
        case UPD: return pb_paste(state);
        case RMV: return pb_remove(state);
        case DBS: return EXIT_FAILURE; // replace later
        case NON: return EXIT_FAILURE; // should never get here
    }
}

// vim: set ts=4 sw=4 et:
