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

    struct CurlResponse * response_data = malloc(sizeof(struct CurlResponse));
    char * target = malloc(tlen);
    if ( !response_data || !target ) {
        status = CURLE_OUT_OF_MEMORY;
        goto cleanup;
    }

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
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, response_data);

    status = curl_easy_perform(handle);
    if ( status == EXIT_FAILURE ) { goto cleanup; }

    if ( s->url ) {
        printf("%s", response_data->mem);
    } else {
        status = print_url(s, response_data->mem);
    }

    cleanup:
        if ( list ) { curl_slist_free_all(list); }
        curl_easy_cleanup(handle);
        curl_formfree(post);
        if ( response_data->mem ) { free(response_data->mem); }
        if ( response_data ) { free(response_data); }
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

    curl_easy_setopt(handle, CURLOPT_VERBOSE, s->verb >= 2);

    struct curl_slist * list = NULL;
    list = curl_slist_append(list, "Accept: application/json");
    curl_easy_setopt(handle, CURLOPT_HTTPHEADER, list);

    const char * provider = def_provider ? def_provider : s->provider;

    struct CurlResponse * response_data = malloc(sizeof(struct CurlResponse));
    size_t target_len = strlen(provider) + strlen(s->uuid) + 1;
    char * target = malloc(target_len);
    if ( !target ) { status = CURLE_OUT_OF_MEMORY; goto cleanup; }

    snprintf(target, target_len, "%s%s", provider, s->uuid);

    curl_easy_setopt(handle, CURLOPT_CUSTOMREQUEST, "DELETE");
    curl_easy_setopt(handle, CURLOPT_URL, target);
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, &pb_write_cb);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, response_data);

    status = curl_easy_perform(handle);
    if ( status == EXIT_FAILURE ) { goto cleanup; }

    status = print_url(s, response_data->mem);

    cleanup:
        if ( list ) { curl_slist_free_all(list); }
        curl_easy_cleanup(handle);
        if ( response_data->mem ) { free(response_data->mem); }
        if ( response_data ) { free(response_data); }
        free(target);
        return status;
}

CURLcode
print_url (const struct pbpst_state * s, const char * userdata) {

    json_error_t err;
    json_t * json = json_loads(userdata, 0, &err);
    if ( !json ) {
        fprintf(stderr, "pbpst: %s at %d:%d\n", err.text, err.line, err.column);
        return EXIT_FAILURE;
    }

    CURLcode status = EXIT_SUCCESS;
    pastes = json_object_get(mem_db, "pastes");
    json_t * prov_obj = 0, * uuid_j = 0, * lid_j = 0,
           * label_j = 0, * status_j = 0, * sunset_j = 0, * new_paste = 0;

    char * hdln = 0, * lexr = 0, * them = 0, * extn = 0, * sunset = 0;

    const char * provider = def_provider ? def_provider : s->provider;

    if ( !pastes ) { status = EXIT_FAILURE; goto cleanup; }
    prov_pastes = json_object_get(pastes, provider);
    if ( !prov_pastes ) {
        prov_obj = json_pack("{s:{}}", provider);
        json_object_update(pastes, prov_obj);
        json_decref(prov_obj);
        prov_pastes = json_object_get(pastes, provider);
    }

    uuid_j   = json_object_get(json, "uuid");
    lid_j    = json_object_get(json, "long");
    label_j  = json_object_get(json, "label");
    status_j = json_object_get(json, "status");
    sunset_j = json_object_get(json, "sunset");

    if ( !status_j ) { status = EXIT_FAILURE; goto cleanup; }
    const char stat = json_string_value(status_j)[0];
    if ( stat == 'a' ) {
        fputs("pbpst: Paste already existed\n", stderr);
        goto cleanup;
    } else if ( stat == 'd' ) {
        json_object_del(prov_pastes, s->uuid);
        if ( s->verb ) {
            json_t * value;
            const char * key;
            json_object_foreach(json, key, value) {
                printf("%s: %s\n", key, json_string_value(value));
            }
        } goto cleanup;
    }

    if ( sunset_j ) {
        time_t curtime = time(NULL), offset = 0;
        if ( sscanf(s->secs, "%ld", &offset) == EOF ) {
            signed errsv = errno;
            fprintf(stderr, "pbpst: Failed to scan offset: %s\n",
                    strerror(errsv)); status = EXIT_FAILURE; goto cleanup;
        }

        if ( !(sunset = malloc(12)) ) {
            fprintf(stderr, "pbpst: Failed to store sunset epoch: "
                    "Out of Memory\n"); status = EXIT_FAILURE; goto cleanup;
        } snprintf(sunset, 11, "%ld", curtime + offset);
    }

    if ( (!uuid_j && !s->uuid) || !lid_j ) { status = EXIT_FAILURE; goto cleanup; }

    const char * uuid  = uuid_j ? json_string_value(uuid_j) : s->uuid,
               * lid   = json_string_value(lid_j),
               * label = json_string_value(label_j),
               * msg   =  s->msg               ? s->msg
                       : !s->msg && s->path ? s->path : "-";

    if ( label_j && s->secs ) {
        new_paste = json_pack("{s:s,s:s,s:s,s:s}", "long", lid, "msg", msg,
                              "label", label, "sunset", sunset);
    } else if ( label_j && !s->secs ) {
        new_paste = json_pack("{s:s,s:s,s:s,s:n}", "long", lid, "msg", msg,
                              "label", label, "sunset");
    } else if ( !label_j && s->secs ) {
        new_paste = json_pack("{s:s,s:s,s:n,s:s}", "long", lid, "msg", msg,
                              "label", "sunset", sunset);
    } else {
        new_paste = json_pack("{s:s,s:s,s:n,s:n}", "long", lid, "msg", msg,
                              "label", "sunset");
    }

    if ( json_object_set(prov_pastes, uuid, new_paste) == -1 ) {
        fputs("pbpst: Failed to create new paste object\n", stderr);
        status = EXIT_FAILURE; goto cleanup;
    }

    if ( s->verb ) {
        json_t * value;
        const char * key;
        json_object_foreach(json, key, value) {
            printf("%s: %s\n", key, json_string_value(value));
        } printf("murl: ");
    }

    const char * rndr = s->rend ? "r/" : "",
               * idnt = label_j ? label : s->priv ? lid : lid + 24,
               * mod_fmts [] = { "#L-", "/", "?style=", "." };

    char * state_mod = 0, ** mod_var = 0,
         * mod_names [] = { "line", "lexer", "theme", "extension" };

    for ( uint8_t i = 0; i < 4; i ++ ) {
        switch ( mod_names[i][1] ) {
            case 'i': mod_var = &hdln; state_mod = s->ln;    break;
            case 'e': mod_var = &lexr; state_mod = s->lexer; break;
            case 'h': mod_var = &them; state_mod = s->theme; break;
            case 'x': mod_var = &extn; state_mod = s->ext;   break;
        }

        if ( state_mod ) {
            size_t tlen = strlen(state_mod) + strlen(mod_fmts[i]);
            *mod_var = malloc(tlen + 2);
            if ( !mod_var ) {
                fprintf(stderr, "pbpst: Could not modify %s: Out of Memory\n",
                                mod_names[i]); goto cleanup;
            } snprintf(*mod_var, tlen + 1, "%s%s", mod_fmts[i], state_mod);
        } else { *mod_var = ""; }
    }

    printf("%s%s%s%s%s%s%s\n", provider, rndr, idnt, extn, lexr, them, hdln);

    cleanup:
        if ( s->ln )    { free(hdln);   }
        if ( s->lexer ) { free(lexr);   }
        if ( s->theme ) { free(them);   }
        if ( s->ext )   { free(extn);   }
        if ( s->secs )  { free(sunset); }
        json_decref(json);
        json_decref(uuid_j);
        json_decref(lid_j);
        json_decref(label_j);
        json_decref(status_j);
        json_decref(new_paste);
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
