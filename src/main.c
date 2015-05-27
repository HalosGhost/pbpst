/*****************************\
* Manage pastes to pb         *
* Author: Sam Stuewe (C) 2015 *
* License: GPLv3              *
\*****************************/

#include "main.h"

signed
main (signed argc, char * argv []) {

    if ( argc <= 1 ) {
        fprintf(stderr, "%s%s%s", cmds_help, gen_help, more_info);
        return EXIT_FAILURE;
    }

    signed exit_status = EXIT_SUCCESS;

    struct ptpst_state state = { .path = 0, .url = 0, .lexer = 0, .vanity = 0,
                                 .uuid = 0, .provider = 0, .cmd = NON, .ln = 0,
                                 .help = false, .priv = false, .rend = false,
                                 .verb = false };

    const char vos [] = "SRUP:hv:s:f:l:L:pru:";
    for ( signed oi = 0, c = getopt_long(argc, argv, vos, os, &oi);
          c != -1; c = getopt_long(argc, argv, vos, os, &oi) ) {

        size_t l = optarg ? strlen(optarg) : 0;
        char ** state_var = (c == 's' ? &state.url      :
                             c == 'f' ? &state.path     :
                             c == 'l' ? &state.lexer    :
                             c == 'v' ? &state.vanity   :
                             c == 'u' ? &state.uuid     :
                             c == 'P' ? &state.provider : 0);

        switch ( c ) {
            case 'S': case 'R': case 'U':
                if ( state.cmd ) {
                    fputs("Error: you can only run one operation at a time\n",
                          stderr);
                    exit_status = EXIT_FAILURE; goto cleanup;
                } state.cmd = (enum pb_cmd )c; break;

            case 's': case 'f': case 'l': case 'v': case 'u': case 'P':
                *state_var = (char * )malloc(l + 1);
                strncpy(*state_var, optarg, l);
                break;

            case 'L': sscanf(optarg, "%" SCNu32, &state.ln); break;
            case 'r': state.rend = true; break;
            case 'p': state.priv = true; break;
            case 'h': state.help = true; break;
            case 256: state.verb = true; break;
            case 257: printf(version_str); goto cleanup;
        }
    }

    switch ( state.cmd ) {
        case SNC:
            if ( !state.url && !state.path ) {
                printf("%s%s",   sync_help, gen_help);
                goto cleanup;
            }

            if ( (state.url && (state.path || state.lexer  || state.rend ||
                                state.ln   || state.priv)) || state.uuid ) {
                fprintf(stderr, "Error: erroneous option. See `%s -Sh`\n",
                        argv[0]); goto cleanup;
            } break;

        case RMV:
            if ( state.path || state.url  || state.lexer || state.vanity ||
                 state.ln   || state.priv || state.rend ) {
                fprintf(stderr, "Error: erroneous option. See `%s -Rh`\n",
                        argv[0]); goto cleanup;
            } break;

        case UPD:
            if ( (state.path && state.url) || state.priv ) {
                fprintf(stderr, "Error: erroneous option. See `%s -Uh`\n",
                        argv[0]); goto cleanup;
            } break;

        case NON:
            printf("%s%s%s", cmds_help, gen_help, more_info);
            goto cleanup;
    }

    if ( state.help ) {
        switch ( state.cmd ) {
            case SNC: printf("%s%s",   sync_help, gen_help);            break;
            case RMV: printf("%s%s",   rem_help,  gen_help);            break;
            case UPD: printf("%s%s",   upd_help,  gen_help);            break;
            case NON: printf("%s%s%s", cmds_help, gen_help, more_info); break;
        } goto cleanup;
    }

    // Takes care of all the interactions with pb
    exit_status = state.cmd == SNC ? pb_paste(&state) : pb_remove(&state);

    cleanup:
        if ( state.url )      { free(state.url);      }
        if ( state.path )     { free(state.path);     }
        if ( state.lexer )    { free(state.lexer);    }
        if ( state.vanity )   { free(state.vanity);   }
        if ( state.uuid )     { free(state.uuid);     }
        if ( state.provider ) { free(state.provider); }
        return exit_status;
}

/**
 * TODO
 **
 * Add support for vanity URLs, specifying the lexer, making
 * a short URL, and specifying the line number.
 */
CURLcode
pb_paste (const struct ptpst_state * state) {

    CURLcode status = CURLE_OK;
    CURL * handle = curl_easy_init();

    if ( !handle ) {
        fputs("Failed to get CURL handle", stderr);
        return CURLE_FAILED_INIT;
    }

    struct curl_httppost * post = NULL;
    struct curl_httppost * last = NULL;

    char * url = state->provider;
    bool default_provider = false;

    if ( !state->provider ) {
        size_t len = strlen("https://ptpb.pw/") + 1;

        url = malloc(len);
        if ( !url ) {
            status = CURLE_OUT_OF_MEMORY;
            goto cleanup;
        }

        snprintf(url, len, "https://ptpb.pw/");
        default_provider = true;
    }

    if ( state->cmd == SNC ) {
        CURLFORMcode s;
        s = curl_formadd(&post,                &last,
                         CURLFORM_COPYNAME,    "c",
                         CURLFORM_FILE,        state->path,
                         CURLFORM_CONTENTTYPE, "application/octet-stream",
                         CURLFORM_END);
        if ( s != 0 ) {
            status = CURLE_HTTP_POST_ERROR;
            goto cleanup;
        }
    }

    if ( state->verb ) { curl_easy_setopt(handle, CURLOPT_VERBOSE, 1L); }

    curl_easy_setopt(handle, CURLOPT_HTTPPOST, post);
    curl_easy_setopt(handle, CURLOPT_URL, url);
    status = curl_easy_perform(handle);

    cleanup:
        curl_easy_cleanup(handle);
        if ( default_provider ) { free(url); }
        if ( post )             { curl_formfree(post); }
        return status;
}

/**
 * TODO
 **
 * Actually implement paste removal
 */
CURLcode
pb_remove (const struct ptpst_state * state) {

    CURLcode status = CURLE_OK;

    return status;
}

// vim: set ts=4 sw=4 et:
