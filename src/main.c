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
struct pbpst_state state = {
    .path = 0, .url = 0, .lexer = 0, .vanity = 0,
    .uuid = 0, .provider = 0, .dbfile = 0,
    .query = 0, .del = 0, .msg = 0, .theme = 0, .ext = 0,
    .cmd = NON, .ln = 0,
    .help = false, .priv = false, .rend = false,
    .init = false, .verb = 0, .prog = false
};

const char * def_provider = 0;

signed
main (signed argc, char * argv []) {

    if ( argc <= 1 ) {
        fprintf(stderr, "%s%s%s", cmds_help, gen_help, more_info);
        return EXIT_FAILURE;
    }

    signal(2, signal_handler);

    signed exit_status = EXIT_SUCCESS;

    const char vos [] = "SRUDP:hv:s:f:l:t:e:L:pru:b:q:d:im:#";
    for ( signed oi = 0, c = getopt_long(argc, argv, vos, os, &oi);
          c != -1; c = getopt_long(argc, argv, vos, os, &oi) ) {

        size_t l = optarg ? strlen(optarg) + 1 : 0;
        char ** state_var = 0;

        switch ( c ) {
            case 'S': case 'R': case 'U': case 'D':
                if ( state.cmd ) {
                    fputs("pbpst: You can only run one operation at a time\n",
                          stderr);
                    exit_status = EXIT_FAILURE; goto cleanup;
                } state.cmd = (enum pb_cmd )c; break;

            case 's': state_var = &state.url;      goto svcase;
            case 'f': state_var = &state.path;     goto svcase;
            case 'l': state_var = &state.lexer;    goto svcase;
            case 't': state_var = &state.theme;    goto svcase;
            case 'e': state_var = &state.ext;      goto svcase;
            case 'v': state_var = &state.vanity;   goto svcase;
            case 'u': state_var = &state.uuid;     goto svcase;
            case 'm': state_var = &state.msg;      goto svcase;
            case 'q': state_var = &state.query;    goto svcase;
            case 'd': state_var = &state.del;      goto svcase;
            case 'P': state_var = &state.provider; goto svcase;
            case 'b': state_var = &state.dbfile;   goto svcase;
            svcase:
                *state_var = (char * )malloc(l);
                snprintf(*state_var, l, "%s", optarg);
                break;

            case 'L': sscanf(optarg, "%" SCNu32, &state.ln); break;
            case '#': state.prog = true; break;
            case 'r': state.rend = true; break;
            case 'i': state.init = true; break;
            case 'p': state.priv = true; break;
            case 'h': state.help = true; break;
            case 256: state.verb += 1;   break;
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

    if ( !pbpst_test_options(&state) ) {
        exit_status = EXIT_FAILURE; goto cleanup;
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

    exit_status = !state.init ? pbpst_dispatch(&state) : EXIT_SUCCESS;

    if ( db_swp_flush(mem_db, swp_db_loc) == -1 ) {
        exit_status = EXIT_FAILURE; goto cleanup;
    }

    if ( exit_status != EXIT_SUCCESS ) { goto cleanup; }

    if ( db_swp_cleanup(db_loc, swp_db_loc) == -1 ) {
        exit_status = EXIT_FAILURE; goto cleanup;
    }

    cleanup:
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
        json_decref(mem_db);
        json_decref(pastes);
        json_decref(prov_pastes);
        json_decref(def_prov);
        if ( swp_db_loc ) { free(swp_db_loc); }
        if ( db_loc == state.dbfile ) {
            free(state.dbfile);
        } else {
            free(db_loc);
            free(state.dbfile);
        } return exit_status;
}

bool
pbpst_test_options (const struct pbpst_state * s) {

    const char option_err [] = "pbpst: erroneous option(s). See `pbpst -%ch`\n";
    char cl = 0;
    switch ( s->cmd ) {
        case SNC: cl = (s->url && (s->path || s->lexer || s->rend || s->init
               || s->ln || s->vanity )) || s->uuid ? 'S' : cl; break;

        case RMV: cl = s->path || s->url  || s->lexer || s->vanity || s->init
               || s->ln || s->priv || s->rend || !s->uuid || s->prog || s->theme
               || s->ext ? 'R' : cl; break;

        case UPD: cl = !s->uuid || s->priv || s->query || s->init
               || s->del || s->url ? 'U' : cl; break;

        case DBS: cl = (s->query && s->del) || s->url || s->theme || s->ext
               || s->path || s->lexer || s->vanity || s->ln || s->priv
               || s->rend || s->uuid || s->prog ? 'D' : cl; break;

        case NON: cl = 'N'; break;
    }

    if ( cl == 'N' ) {
        fprintf(stderr, "%s%s%s", cmds_help, gen_help, more_info);
    } else if ( cl != 0 ) {
        fprintf(stderr, option_err, cl);
    } return cl == 0;
}

signed
pbpst_dispatch (const struct pbpst_state * s) {

    switch ( s->cmd ) {
        case SNC:
        case UPD: return pb_paste(s);
        case RMV: return pb_remove(s);
        case DBS: return EXIT_FAILURE; // replace later
        case NON: return EXIT_FAILURE; // should never get here
    }
}

void
signal_handler (signed signum) {

    if ( signum < 1 || signum > 31 ) { return; }

    fprintf(stderr, "pbpst: Received %s\n", sys_siglist[signum]);
    db_swp_cleanup(db_loc, swp_db_loc);
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
    json_decref(mem_db);
    json_decref(pastes);
    json_decref(prov_pastes);
    json_decref(def_prov);
    if ( swp_db_loc ) { free(swp_db_loc); }
    if ( db_loc == state.dbfile ) {
        free(state.dbfile);
    } else {
        free(db_loc);
        free(state.dbfile);
    } exit(EXIT_FAILURE);
}

// vim: set ts=4 sw=4 et:
