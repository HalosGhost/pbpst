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
main (signed argc, char *const argv []) {

    if ( !setlocale(LC_MESSAGES, "") ||
         !bindtextdomain("pbpst", PREFIX "/share/locale") ||
         !textdomain("pbpst") ||
         !bind_textdomain_codeset("pbpst", "utf-8") ) {
        pbpst_err("Failed to initialize internationalization");
        return EXIT_FAILURE;
    }

    if ( argc <= 1 ) {
        print_usage(NON, EXIT_FAILURE);
        return EXIT_FAILURE;
    }

    signal(SIGINT, signal_handler);

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
                    pbpst_err(_("You can only run one operation at a time"));
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
            case 'F': state_var = &state.format;   goto svcase;
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
                    fprintf(stderr, "pbpst: %s: %s\n",
                            _("Could not store argument"), _("Out of Memory"));
                    goto cleanup;
                } snprintf(*state_var, l, "%s", optarg); break;

            case '#': state.prog  = true;         break;
            case 'r': state.rend  = true;         break;
            case 't': state.term  = true;         break;
            case 'i': state.init  = true;         break;
            case 'y': state.prun  = true;         break;
            case 'p': state.priv  = true;         break;
            case 'h': state.help  = true;         break;
            case 'H': state.lspv  = true;         break;
            case 'V': state.verb += 1;            break;
            case 256: printf(version_str);        goto cleanup;
            case 257: state.llex  = true;         break;
            case 258: state.lthm  = true;         break;
            case 259: state.lfrm  = true;         break;
            case 260: state.dfpv  = true;         break;
            default:  exit_status = EXIT_FAILURE; goto cleanup;
        }
    }

    if ( state.help ) {
        print_usage(state.cmd, exit_status);
        goto cleanup;
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
            pbpst_err(_("Error checking file"));
            goto cleanup;
        }

        if ( !S_ISREG(st.st_mode) ) {
            fprintf(stderr, "pbpst: %s: %s\n", _("File is not regular"),
                    state.path);
            exit_status = EXIT_FAILURE; goto cleanup;
        }

        if ( st.st_size > PB_FILE_MAX ) {
            pbpst_err(_("File too Large"));
            exit_status = EXIT_FAILURE; goto cleanup;
        }
    }

    if ( state.secs ) {
        double count = 0;
        char unit = 0;
        sscanf(state.secs, "%lf%c", &count, &unit);

        #pragma clang diagnostic push
        #pragma clang diagnostic ignored "-Wconversion"
        #pragma clang diagnostic ignored "-Wdouble-promotion"
        if ( !isfinite(count) && count > 0 ) {
        #pragma clang diagnostic pop
            pbpst_err(_("Sunset must be a finite, positive value"));
            exit_status = EXIT_FAILURE; goto cleanup;
        }

        unsigned mult = 0;
        switch ( unit ) {
            case 'd': mult = 86400; break;
            case 'h': mult = 3600;  break;
            case 'm': mult = 60;    break;
            default:  mult = 1;     break;
        }

        char * tmp = realloc(state.secs, 22);
        if ( !tmp ) {
            exit_status = EXIT_FAILURE; goto cleanup;
        } else {
            state.secs = tmp;
        }

        snprintf(state.secs, 22, "%u", (unsigned )(count * mult));
    }

    if ( !(db_loc = db_locate(&state)) || !(swp_db_loc = db_swp_init(db_loc)) ||
         !(mem_db = db_read(db_loc)) ) {

        exit_status = EXIT_FAILURE; goto cleanup;
    }

    if ( (def_prov = json_object_get(mem_db, "default_provider")) ) {
        json_incref(def_prov);
        def_provider = json_string_value(def_prov);
    }

    if ( !state.provider ) {
        bool def = (bool )def_provider;
        size_t len = strlen(def ? def_provider : FALLBACK_PROVIDER) + 1;
        state.provider = malloc(len);
        if ( !state.provider ) {
            pbpst_err(_("Could not store provider"));
            exit_status = EXIT_FAILURE; goto cleanup;
        } memcpy(state.provider, (def ? def_provider : FALLBACK_PROVIDER), len);
    }

    size_t len = strlen(state.provider);
    if ( state.provider[len - 1] != '/' ) {
        state.provider = realloc(state.provider, len + 2);
        state.provider[len] = '/'; state.provider[len + 1] = '\0';
    }

    if ( !strstr(state.provider, "https://") ) {
        pbpst_err(_("Only HTTPS providers are supported"));
        exit_status = EXIT_FAILURE; goto cleanup;
    }

    exit_status = pbpst_dispatch(&state);
    if ( exit_status == CURLE_HTTP_RETURNED_ERROR ) { goto cleanup; }

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
        case SNC: case SHR: break;
        case RMV: cl = !s->uuid && !s->prun ? 'R' : cl; break;
        case UPD: cl = !s->uuid ? 'U' : cl; break;
        case DBS: cl = !s->init && !s->lspv && !s->query && !s->del && !s->prun
                    && !(s->dfpv && s->provider) ? 'D' : cl; break;
        case NON: cl = 'N'; break;
    }

    if ( cl == 'N' ) {
        print_usage(NON, EXIT_FAILURE);
    } else if ( cl != 0 ) {
        pbpst_err(_("Passed erroneous option(s)"));
    } return cl == 0;
}

signed
pbpst_dispatch (const struct pbpst_state * s) {

    if ( s->llex || s->lthm || s->lfrm ) { return pb_list(s); }

    const char * uuid = s->uuid ? s->uuid : s->del;

    switch ( s->cmd ) {
        case SNC:
        case UPD: return pb_paste(s);
        case SHR: return pb_shorten(s->provider, s->url, s->verb);
        case RMV: return (s->prun ? pb_prune(s)
                                  : pb_remove(s->provider, uuid, s->verb));
        case DBS: return pbpst_db(s);
        case NON: return EXIT_FAILURE; // should never get here
    }
}

noreturn void
signal_handler (signed signum) {

    const char * const siglist [] = {
        [SIGINT] = _("Caught Interrupt")
    };

    fprintf(stderr, "pbpst: %s\x1b[?25h\n", siglist[signum]);
    pbpst_cleanup();
    exit(EXIT_FAILURE);
}

void
pbpst_cleanup (void) {

    free(state.url);
    free(state.path);
    free(state.msg);
    free(state.lexer);
    free(state.theme);
    free(state.format);
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
            pbpst_err( !point_of_no_return && !remove(swp_db_loc)
                     ? _("Removed empty swap file (contingency)")
                     : _("You need to manually check your swap db"));
        } free(swp_db_loc);
    }

    if ( db_loc != state.dbfile ) { free(db_loc); }
    free(state.dbfile);
}

signed
pbpst_err (const char * explanation) {

    signed ret = fprintf(stderr, "pbpst: %s\n", explanation);
    return ret;
}

// vim: set ts=4 sw=4 et:
