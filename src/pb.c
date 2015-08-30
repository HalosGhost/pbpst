#include "main.h"
#include "pb.h"
#include "callback.h"

CURLcode
pb_paste (const struct pbpst_state * s) {

    CURLcode status = CURLE_OK;
    CURL * handle = curl_easy_init();

    if ( !handle ) {
        fputs("pbpst: Failed to get CURL handle\n", stderr);
        return CURLE_FAILED_INIT;
    }

    curl_easy_setopt(handle, CURLOPT_VERBOSE, s->verb >= 2);

    const char * provider = def_provider ? def_provider : s->provider;

    struct curl_httppost * post = NULL, * last = NULL;
    size_t tlen = strlen(provider) + (
                  s->vanity     ? strlen(s->vanity) + 2 :
                  s->cmd == UPD ? strlen(s->uuid)   + 1 : 2);

    struct curl_slist * list = NULL;
    list = curl_slist_append(list, "Accept: application/json");
    curl_easy_setopt(handle, CURLOPT_HTTPHEADER, list);

    char * target = malloc(tlen);
    if ( !target ) { status = CURLE_OUT_OF_MEMORY; goto cleanup; }

    CURLFORMcode fc;
    if ( s->cmd == SNC ) {
        if ( s->url ) {
            snprintf(target, tlen, "%s%c", provider, 'u');
            fc = curl_formadd(&post,                &last,
                              CURLFORM_COPYNAME,    "c",
                              CURLFORM_PTRCONTENTS, s->url,
                              CURLFORM_END);
            if ( fc ) { status = CURLE_HTTP_POST_ERROR; goto cleanup; }
        } else if ( s->vanity ) {
            snprintf(target, tlen, "%s~%s", provider, s->vanity);
        } else {
            snprintf(target, tlen, "%s", provider);
        }

        if ( s->priv ) {
            fc = curl_formadd(&post,                 &last,
                              CURLFORM_COPYNAME,     "p",
                              CURLFORM_COPYCONTENTS, "1",
                              CURLFORM_END);
            if ( fc ) { status = CURLE_HTTP_POST_ERROR; goto cleanup; }
        }
    } else if ( s->cmd == UPD ) {
        snprintf(target, tlen, "%s%s", provider, s->uuid);
        curl_easy_setopt(handle, CURLOPT_CUSTOMREQUEST, "PUT");
    }

    if ( !(s->cmd == SNC && s->url) ) {
        fc = curl_formadd(&post,                &last,
                          CURLFORM_COPYNAME,    "c",
                          CURLFORM_FILE,        s->path ? s->path : "-",
                          CURLFORM_CONTENTTYPE, "application/octet-stream",
                          CURLFORM_END);
        if ( fc ) { status = CURLE_HTTP_POST_ERROR; goto cleanup; }
    }

    if ( s->secs ) {
        fc = curl_formadd(&post,                 &last,
                          CURLFORM_COPYNAME,     "s",
                          CURLFORM_COPYCONTENTS, s->secs,
                          CURLFORM_END);
        if ( fc ) { status = CURLE_HTTP_POST_ERROR; goto cleanup; }
    }

    curl_easy_setopt(handle, CURLOPT_HTTPPOST, post);
    curl_easy_setopt(handle, CURLOPT_URL, target);
    curl_easy_setopt(handle, CURLOPT_XFERINFOFUNCTION, &pb_progress_cb);
    curl_easy_setopt(handle, CURLOPT_NOPROGRESS, (long )!s->prog);
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, &pb_write_cb);

    status = curl_easy_perform(handle);

    cleanup:
        if ( list ) { curl_slist_free_all(list); }
        curl_easy_cleanup(handle);
        curl_formfree(post);
        free(target);
        return status;
}

CURLcode
pb_remove (const struct pbpst_state * s) {

    CURLcode status = CURLE_OK;
    CURL * handle = curl_easy_init();

    if ( !handle ) {
        fputs("pbpst: Failed to get CURL handle\n", stderr);
        return CURLE_FAILED_INIT;
    }

    curl_easy_setopt(handle, CURLOPT_VERBOSE, s->verb == 2);

    struct curl_slist * list = NULL;
    list = curl_slist_append(list, "Accept: application/json");
    curl_easy_setopt(handle, CURLOPT_HTTPHEADER, list);

    const char * provider = def_provider ? def_provider : s->provider;

    size_t target_len = strlen(provider) + strlen(s->uuid) + 1;
    char * target = malloc(target_len);
    if ( !target ) { status = CURLE_OUT_OF_MEMORY; goto cleanup; }

    snprintf(target, target_len, "%s%s", provider, s->uuid);

    curl_easy_setopt(handle, CURLOPT_CUSTOMREQUEST, "DELETE");
    curl_easy_setopt(handle, CURLOPT_URL, target);
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, &pb_write_cb);

    status = curl_easy_perform(handle);

    cleanup:
        if ( list ) { curl_slist_free_all(list); }
        curl_easy_cleanup(handle);
        free(target);
        return status;
}

CURLcode
pb_list (const struct pbpst_state * s) {

    CURLcode status = CURLE_OK;
    CURL * handle = curl_easy_init();

    if ( !handle ) {
        fputs("pbpst: Failed to get CURL handle\n", stderr);
        return CURLE_FAILED_INIT;
    }

    curl_easy_setopt(handle, CURLOPT_VERBOSE, s->verb >= 2);

    const char * provider = def_provider ? def_provider : s->provider;

    size_t target_len = strlen(provider) + 3;
    char * target = malloc(target_len);
    if ( !target ) { status = CURLE_OUT_OF_MEMORY; goto cleanup; }

    snprintf(target, target_len, "%s%s", provider, s->llex ? "l" : "ls");
    curl_easy_setopt(handle, CURLOPT_URL, target);

    status = curl_easy_perform(handle);

    cleanup:
        curl_easy_cleanup(handle);
        free(target);
        return status;
}
