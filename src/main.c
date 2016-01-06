/*****************************\
* Manage pastes to pb         *
* Author: Sam Stuewe (C) 2015 *
* License: GPLv3              *
\*****************************/

#include "main.h"
#include "pb.h"
#include "pbpst_db.h"

json_t * mem_db, * pastes = 0, * prov_pastes = 0;
static json_t * def_prov = 0;
struct pbpst_state state;

const char * def_provider = 0;
bool point_of_no_return = false;

signed
main (signed argc, char * argv []) {

    if ( argc <= 1 ) {
        #pragma clang diagnostic push
        #pragma clang diagnostic ignored "-Wdisabled-macro-expansion"
        fprintf(stderr, "%s%s%s", cmds_help, gen_help, more_info);
        #pragma clang diagnostic pop
        return EXIT_FAILURE;
    }

    signal(2, signal_handler);

    signed exit_status = EXIT_SUCCESS;

    const char * vos = opts_for[NON];
    for ( signed oi = 0, c = getopt_long(argc, argv, vos, os, &oi);
          c != -1; c = getopt_long(argc, argv, vos, os, &oi) ) {

        size_t l = optarg ? strlen(optarg) + 1 : 0;
        char ** state_var = 0;

        switch ( c ) {
            case 'S': case 'R': case 'U': case 'D':
                if ( !state.cmd ) {
                    vos = opts_for[(state.cmd = (enum pb_cmd )c)];
                    optind = 1;
                } else if ( state.cmd != c ) {
                    fputs("pbpst: You can only run one operation at a time\n",
                          stderr);
                    exit_status = EXIT_FAILURE; goto cleanup;
                } break;

            case 's':
                if ( !state.cmd ) {
                    vos = opts_for[(state.cmd = (enum pb_cmd )c)];
                    optind = 1;
                } else {
                    state_var = &state.url; goto svcase;
                } break;

            case 'f': state_var = &state.path;     goto svcase;
            case 'l': state_var = &state.lexer;    goto svcase;
            case 'T': state_var = &state.theme;    goto svcase;
            case 'e': state_var = &state.ext;      goto svcase;
            case 'v': state_var = &state.vanity;   goto svcase;
            case 'u': state_var = &state.uuid;     goto svcase;
            case 'm': state_var = &state.msg;      goto svcase;
            case 'q': state_var = &state.query;    goto svcase;
            case 'd': state_var = &state.del;      goto svcase;
            case 'P': state_var = &state.provider; goto svcase;
            case 'b': state_var = &state.dbfile;   goto svcase;
            case 'L': state_var = &state.ln;       goto svcase;
            case 'x': state_var = &state.secs;     goto svcase;
            svcase:
                *state_var = (char * )malloc(l);
                if ( !*state_var ) {
                    fputs("pbpst: Could not store arguments: Out of Memory\n",
                          stderr); goto cleanup;
                } snprintf(*state_var, l, "%s", optarg); break;

            case '#': state.prog = true; break;
            case 'r': state.rend = true; break;
            case 't': state.term = true; break;
            case 'i': state.init = true; break;
            case 'y': state.prun = true; break;
            case 'p': state.priv = true; break;
            case 'h': state.help = true; break;
            case 'V': state.verb += 1;   break;
            case 256: printf(version_str); goto cleanup;
            case 257: state.llex = true; break;
            case 258: state.lthm = true; break;
            case 259: state.lfrm = true; break;
            default:  exit_status = EXIT_FAILURE; goto cleanup;
        }
    }

    if ( state.help ) {
        switch ( state.cmd ) {
            case SNC: printf("%s%s",   sync_help, gen_help);            break;
            case RMV: printf("%s%s",   rem_help,  gen_help);            break;
            case UPD: printf("%s%s",   upd_help,  gen_help);            break;
            case DBS: printf("%s%s",   dbs_help,  gen_help);            break;
            case SHR:
            case NON: printf("%s%s%s", cmds_help, gen_help, more_info); break;
        } goto cleanup;
    }

    if ( !(state.llex || state.lthm || state.lfrm) &&
         !pbpst_test_options(&state) ) {
        exit_status = EXIT_FAILURE; goto cleanup;
    }

    if ( state.path ) {
        struct stat st;

        errno = 0;
        if ( stat(state.path, &st) ) {
            exit_status = errno;
            #pragma clang diagnostic push
            #pragma clang diagnostic ignored "-Wdisabled-macro-expansion"
            fprintf(stderr, "pbpst: Error checking file: %s\n",
                    strerror(exit_status));
            #pragma clang diagnostic pop
            goto cleanup;
        }

        if ( st.st_size > PB_FILE_MAX ) {
            fputs("pbpst: File too large\n", stderr);
            exit_status = EXIT_FAILURE; goto cleanup;
        }
    }

    db_loc = db_locate(&state);
    if ( !db_loc ) {
        exit_status = EXIT_FAILURE; goto cleanup;
    }

    if ( !(swp_db_loc = db_swp_init(db_loc)) ) {
        exit_status = EXIT_FAILURE; goto cleanup;
    }

    if ( !(mem_db = db_read(db_loc)) ) {
        exit_status = EXIT_FAILURE; goto cleanup;
    }

    if ( (def_prov = json_object_get(mem_db, "default_provider")) ) {
        def_provider = json_string_value(def_prov);
    } else if ( !state.provider ) {
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

    if ( db_swp_flush(mem_db, swp_db_loc) == -1 ) {
        exit_status = EXIT_FAILURE; goto cleanup;
    }

    if ( exit_status != EXIT_SUCCESS ) { goto cleanup; }

    if ( db_swp_cleanup(db_loc, swp_db_loc) == -1 ) {
        exit_status = EXIT_FAILURE; goto cleanup;
    }

    cleanup:
        pbpst_cleanup();
        return exit_status;
}

bool
pbpst_test_options (const struct pbpst_state * s) {

    char cl = 0;
    switch ( s->cmd ) {
        case SNC: cl = !s->path ? 'S' : cl; break;
        case SHR: break;
        case RMV: cl = !s->uuid && !s->prun ? 'R' : cl; break;
        case UPD: cl = !s->uuid ? 'U' : cl; break;
        case DBS: cl = !s->init && !s->query && !s->del && !s->prun
                     ? 'D' : cl; break;
        case NON: cl = 'N'; break;
    }

    if ( cl == 'N' ) {
        #pragma clang diagnostic push
        #pragma clang diagnostic ignored "-Wdisabled-macro-expansion"
        fprintf(stderr, "%s%s%s", cmds_help, gen_help, more_info);
        #pragma clang diagnostic pop
    } else if ( cl != 0 ) {
        #pragma clang diagnostic push
        #pragma clang diagnostic ignored "-Wdisabled-macro-expansion"
        fprintf(stderr, "pbpst: erroneous option(s). See `pbpst -%ch`\n", cl);
        #pragma clang diagnostic pop
    } return cl == 0;
}

signed
pbpst_dispatch (const struct pbpst_state * s) {

    if ( s->llex || s->lthm || s->lfrm ) { return pb_list(s); }

    const char * provider = s->provider ? s->provider : def_provider,
               * uuid     = s->uuid     ? s->uuid     : s->del;

    switch ( s->cmd ) {
        case SNC:
        case UPD: return pb_paste(s);
        case SHR: return pb_shorten(provider, s->url, s->verb);
        case RMV: return (s->prun ? pb_prune(s)
                                  : pb_remove(provider, uuid, s->verb));
        case DBS: return pbpst_db(s);
        case NON: return EXIT_FAILURE; // should never get here
    }
}

void
signal_handler (signed signum) {

    if ( signum < 1 || signum > 31 ) { return; }

    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wdisabled-macro-expansion"
    fprintf(stderr, signal_err, sys_siglist[signum]);
    #pragma clang diagnostic pop
    if ( point_of_no_return ) {
        fputs("pbpst: You need to manually check your swap db\n", stderr);
    } pbpst_cleanup(); exit(EXIT_FAILURE);
}

void
pbpst_cleanup (void) {

    free(state.url);
    free(state.path);
    free(state.msg);
    free(state.lexer);
    free(state.theme);
    free(state.ext);
    free(state.vanity);
    free(state.uuid);
    free(state.query);
    free(state.del);
    free(state.provider);
    free(state.ln);
    free(state.secs);
    json_decref(mem_db);
    json_decref(pastes);
    json_decref(prov_pastes);
    json_decref(def_prov);

    if ( swp_db_loc ) {
        struct stat st;
        if ( stat(swp_db_loc, &st) == 0 && st.st_size == 0 ) {
            remove(swp_db_loc);
            if ( point_of_no_return ) {
                fputs("pbpst: removed empty swap (contingency)\n", stderr);
            }
        } free(swp_db_loc);
    }

    if ( db_loc != state.dbfile ) { free(db_loc); }
    free(state.dbfile);
}

// vim: set ts=4 sw=4 et:
