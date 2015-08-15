#include "main.h"
#include "pb.h"
#include "callback.h"

CURLcode
pb_paste (const struct pbpst_state * s) {

    CURLcode status = CURLE_OK;
    CURL * handle = curl_easy_init();

    if ( !handle ) {
        fputs("Failed to get CURL handle", stderr);
        return CURLE_FAILED_INIT;
    }

	curl_easy_setopt(handle, CURLOPT_VERBOSE, s->verb == 2);

    struct curl_httppost * post = NULL, * last = NULL;
    size_t tlen = strlen(s->provider) + (
                  s->vanity     ? strlen(s->vanity) + 2 :
                  s->cmd == UPD ? strlen(s->uuid) + 1   : 2);

    struct curl_slist * list = NULL;
    list = curl_slist_append(list, "Accept: application/json");
    curl_easy_setopt(handle, CURLOPT_HTTPHEADER, list);

    char * target = malloc(tlen);
    if ( !target ) { status = CURLE_OUT_OF_MEMORY; goto cleanup; }

    CURLFORMcode fc;
    if ( s->cmd == SNC ) {
        if ( s->url ) {
            snprintf(target, tlen, "%s%c", s->provider, 'u');
        } else if ( s->vanity ) {
            snprintf(target, tlen, "%s~%s", s->provider, s->vanity);
        } else {
            snprintf(target, tlen, "%s", s->provider);
        }

        fc = s->url
           ? curl_formadd(&post,                &last,
                          CURLFORM_COPYNAME,    "c",
                          CURLFORM_PTRCONTENTS, s->url,
                          CURLFORM_END)
           : curl_formadd(&post,                &last,
                          CURLFORM_COPYNAME,    "c",
                          CURLFORM_FILE,        s->path ? s->path : "-",
                          CURLFORM_CONTENTTYPE, "application/octet-stream",
                          CURLFORM_END);

        if ( fc ) { status = CURLE_HTTP_POST_ERROR; goto cleanup; }
        if ( s->priv ) {
            fc = curl_formadd(&post,                 &last,
                              CURLFORM_COPYNAME,     "p",
                              CURLFORM_COPYCONTENTS, "1",
                              CURLFORM_END);
            if ( fc ) { status = CURLE_HTTP_POST_ERROR; goto cleanup; }
        } curl_easy_setopt(handle, CURLOPT_HTTPPOST, post);
    } else if ( s->cmd == UPD ) {
        fc = curl_formadd(&post,                &last,
                          CURLFORM_COPYNAME,    "c",
                          CURLFORM_FILE,        s->path ? s->path : "-",
                          CURLFORM_CONTENTTYPE, "application/octet-stream",
                          CURLFORM_END);

        if ( fc ) { status = CURLE_HTTP_POST_ERROR; goto cleanup; }
        snprintf(target, tlen, "%s%s", s->provider, s->uuid);
        curl_easy_setopt(handle, CURLOPT_HTTPPOST, post);
        curl_easy_setopt(handle, CURLOPT_CUSTOMREQUEST, "PUT");
    }

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
        fputs("Failed to get CURL handle", stderr);
        return CURLE_FAILED_INIT;
    }

	curl_easy_setopt(handle, CURLOPT_VERBOSE, s->verb == 2);

    struct curl_slist * list = NULL;
    list = curl_slist_append(list, "Accept: application/json");
    curl_easy_setopt(handle, CURLOPT_HTTPHEADER, list);

    size_t target_len = strlen(s->provider) + strlen(s->uuid) + 1;
    char * target = malloc(target_len);
    if ( !target ) { status = CURLE_OUT_OF_MEMORY; goto cleanup; }

    snprintf(target, target_len, "%s%s", s->provider, s->uuid);

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
