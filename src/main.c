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

    struct pbpst_state state = { .path = 0, .url = 0, .lexer = 0, .vanity = 0,
                                 .uuid = 0, .provider = 0, .cmd = NON, .ln = 0,
                                 .help = false, .priv = false, .rend = false,
                                 .verb = false };

    const char vos [] = "SRUP:hv:s:f:l:L:pru:";
    for ( signed oi = 0, c = getopt_long(argc, argv, vos, os, &oi);
          c != -1; c = getopt_long(argc, argv, vos, os, &oi) ) {

        size_t l = optarg ? strlen(optarg) : 0;
        char ** state_var = 0;

        switch ( c ) {
            case 'S': case 'R': case 'U':
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
            case 'P': state_var = &state.provider; goto svcase;
            svcase:
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

    if ( state.help ) {
        switch ( state.cmd ) {
            case SNC: printf("%s%s",   sync_help, gen_help);            break;
            case RMV: printf("%s%s",   rem_help,  gen_help);            break;
            case UPD: printf("%s%s",   upd_help,  gen_help);            break;
            case NON: printf("%s%s%s", cmds_help, gen_help, more_info); break;
        } goto cleanup;
    }

    switch ( state.cmd ) {
        case SNC:
            if ( !state.url && !state.path ) {
                printf("%s%s", sync_help, gen_help);
                goto cleanup;
            }

            if ( (state.url && (state.path || state.lexer  || state.rend ||
                                state.ln   || state.vanity )) || state.uuid ) {
                fprintf(stderr, "Error: erroneous option. See `%s -Sh`\n",
                        argv[0]); goto cleanup;
            } break;

        case RMV:
            if ( state.path || state.url  || state.lexer || state.vanity ||
                 state.ln   || state.priv || state.rend ) {
                fprintf(stderr, "Error: erroneous option. See `%s -Rh`\n",
                        argv[0]); goto cleanup;
            } else if ( !state.uuid ) {
                fprintf(stderr,
                        "Error: please specify UUID to remove. See `%s -Rh`\n",
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

    // Takes care of all the interactions with pb
    exit_status = state.cmd == RMV ? pb_remove(&state) : pb_paste(&state);

    cleanup:
        free(state.url);
        free(state.path);
        free(state.lexer);
        free(state.vanity);
        free(state.uuid);
        free(state.provider);
        return exit_status;
}

/**
 * TODO
 **
 * Add support for specifying the lexer and specifying the line number.
 */
CURLcode
pb_paste (const struct pbpst_state * state) {

    CURLcode status = CURLE_OK;
    CURL * handle = curl_easy_init();

    if ( !handle ) {
        fputs("Failed to get CURL handle", stderr);
        return CURLE_FAILED_INIT;
    }

    if ( state->verb ) { curl_easy_setopt(handle, CURLOPT_VERBOSE, 1L); }

    struct curl_httppost * post = NULL;
    struct curl_httppost * last = NULL;
    size_t tlen = strlen(state->provider) + (
                  state->vanity     ? strlen(state->vanity) + 2 :
                  state->cmd == UPD ? strlen(state->uuid) + 1   : 2);

    char * target = malloc(tlen);
    CURLFORMcode s;
    if ( !target ) { status = CURLE_OUT_OF_MEMORY; goto cleanup; }

    if ( state->cmd == SNC ) {
        if ( state->url ) {
            snprintf(target, tlen, "%s%c", state->provider, 'u');
        } else if ( state->vanity ) {
            snprintf(target, tlen, "%s~%s", state->provider, state->vanity);
        } else {
            snprintf(target, tlen, "%s", state->provider);
        }

        s = state->url
          ? curl_formadd(&post,                &last,
                         CURLFORM_COPYNAME,    "c",
                         CURLFORM_PTRCONTENTS, state->url,
                         CURLFORM_END)
          : curl_formadd(&post,                &last,
                         CURLFORM_COPYNAME,    "c",
                         CURLFORM_FILE,        state->path,
                         CURLFORM_CONTENTTYPE, "application/octet-stream",
                         CURLFORM_END);

        if ( s ) { status = CURLE_HTTP_POST_ERROR; goto cleanup; }
        if ( state->priv ) {
            s = curl_formadd(&post,                 &last,
                             CURLFORM_COPYNAME,     "p",
                             CURLFORM_COPYCONTENTS, "1",
                             CURLFORM_END);
            if ( s ) { status = CURLE_HTTP_POST_ERROR; goto cleanup; }
        } curl_easy_setopt(handle, CURLOPT_HTTPPOST, post);
    } else if ( state->cmd == UPD ) {
        s = curl_formadd(&post,                &last,
                         CURLFORM_COPYNAME,    "c",
                         CURLFORM_FILE,        state->path,
                         CURLFORM_CONTENTTYPE, "application/octet-stream",
                         CURLFORM_END);

        if ( s ) { status = CURLE_HTTP_POST_ERROR; goto cleanup; }
        snprintf(target, tlen, "%s%s", state->provider, state->uuid);
        curl_easy_setopt(handle, CURLOPT_HTTPPOST, post);
        curl_easy_setopt(handle, CURLOPT_CUSTOMREQUEST, "PUT");
    }

    curl_easy_setopt(handle, CURLOPT_URL, target);
    status = curl_easy_perform(handle);

    cleanup:
        curl_easy_cleanup(handle);
        curl_formfree(post);
        free(target);
        return status;
}

CURLcode
pb_remove (const struct pbpst_state * state) {

    CURLcode status = CURLE_OK;
    CURL * handle = curl_easy_init();

    if ( !handle ) {
        fputs("Failed to get CURL handle", stderr);
        return CURLE_FAILED_INIT;
    }

    if ( state->verb ) { curl_easy_setopt(handle, CURLOPT_VERBOSE, 1L); }

    size_t target_len = strlen(state->provider) + strlen(state->uuid) + 1;
    char * target = malloc(target_len);
    if ( !target ) { status = CURLE_OUT_OF_MEMORY; goto cleanup; }

    snprintf(target, target_len, "%s%s", state->provider, state->uuid);

    curl_easy_setopt(handle, CURLOPT_CUSTOMREQUEST, "DELETE");
    curl_easy_setopt(handle, CURLOPT_URL, target);
    status = curl_easy_perform(handle);

    cleanup:
        curl_easy_cleanup(handle);
        free(target);
        return status;
}

// vim: set ts=4 sw=4 et:
