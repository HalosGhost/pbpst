#include "main.h"
#include "pb.h"

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
