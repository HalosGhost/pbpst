#include "main.h"
#include "pb.h"
#include "pbpst_db.h"
#include "callback.h"

CURLcode
pb_paste (const struct pbpst_state * s) {

    CURLcode status = CURLE_OK;
    CURL * handle = curl_easy_init();

    if ( !handle ) {
        pbpst_err(_("Failed to get CURL handle"));
        return CURLE_FAILED_INIT;
    }

    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wdisabled-macro-expansion"
    curl_easy_setopt(handle, CURLOPT_VERBOSE, s->verb >= 2);
    #pragma clang diagnostic pop

    struct curl_httppost * post = NULL, * last = NULL;
    size_t tlen = strlen(s->provider) + (
                  s->vanity                ? strlen(s->vanity) + 2 :
                  s->cmd == UPD && s->uuid ? strlen(s->uuid)   + 1 : 2);

    struct curl_slist * list = NULL;
    list = curl_slist_append(list, "Accept: application/json");
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wdisabled-macro-expansion"
    curl_easy_setopt(handle, CURLOPT_HTTPHEADER, list);
    #pragma clang diagnostic pop

    struct CurlResponse * response_data = malloc(sizeof(struct CurlResponse));
    char * target = malloc(tlen);

    if ( !response_data ) { status = CURLE_OUT_OF_MEMORY; goto cleanup; }
    response_data->mem = 0;
    if ( !target ) { status = CURLE_OUT_OF_MEMORY; goto cleanup; }

    CURLFORMcode fc;
    if ( s->cmd == SNC ) {
        if ( s->vanity ) {
            snprintf(target, tlen, "%s~%s", s->provider, s->vanity);
        } else {
            snprintf(target, tlen, "%s", s->provider);
        }

        if ( s->priv ) {
            fc = curl_formadd(&post,                 &last,
                              CURLFORM_COPYNAME,     "p",
                              CURLFORM_COPYCONTENTS, "1",
                              CURLFORM_END);
            if ( fc ) { status = CURLE_HTTP_POST_ERROR; goto cleanup; }
        }
    } else if ( s->cmd == UPD && s->uuid ) {
        snprintf(target, tlen, "%s%s", s->provider, s->uuid);
        #pragma clang diagnostic push
        #pragma clang diagnostic ignored "-Wdisabled-macro-expansion"
        curl_easy_setopt(handle, CURLOPT_CUSTOMREQUEST, "PUT");
        #pragma clang diagnostic pop
    }

    fc = curl_formadd(&post,                &last,
                      CURLFORM_COPYNAME,    "c",
                      CURLFORM_FILE,        s->path ? s->path : "-",
                      CURLFORM_CONTENTTYPE, "application/octet-stream",
                      CURLFORM_END);
    if ( fc ) { status = CURLE_HTTP_POST_ERROR; goto cleanup; }

    if ( s->secs ) {
        fc = curl_formadd(&post,                 &last,
                          CURLFORM_COPYNAME,     "s",
                          CURLFORM_COPYCONTENTS, s->secs,
                          CURLFORM_END);
        if ( fc ) { status = CURLE_HTTP_POST_ERROR; goto cleanup; }
    }

    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wdisabled-macro-expansion"
    curl_easy_setopt(handle, CURLOPT_HTTPPOST, post);
    curl_easy_setopt(handle, CURLOPT_FAILONERROR, 1L);
    curl_easy_setopt(handle, CURLOPT_URL, target);
    curl_easy_setopt(handle, CURLOPT_XFERINFOFUNCTION, &pb_progress_cb);
    curl_easy_setopt(handle, CURLOPT_NOPROGRESS, (long )false);
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, &pb_write_cb);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, response_data);
    #pragma clang diagnostic pop

    status = curl_easy_perform(handle);
    if ( status != CURLE_OK ) {
        #pragma clang diagnostic push
        #pragma clang diagnostic ignored "-Wdisabled-macro-expansion"
        fprintf(stderr, "pbpst: Pasting failed: %s\n",
                curl_easy_strerror(status)); goto cleanup;
        #pragma clang diagnostic pop
    }

    status = db_add_entry(s, response_data->mem) == EXIT_SUCCESS
           ? EXIT_SUCCESS : EXIT_FAILURE;
    if ( status == EXIT_FAILURE ) { goto cleanup; }

    status = print_url(s, response_data->mem);

    cleanup:
        if ( list ) { curl_slist_free_all(list); }
        curl_easy_cleanup(handle);
        curl_formfree(post);
        if ( response_data ) {
            if ( response_data->mem ) { free(response_data->mem); }
            free(response_data);
        }
        free(target);
        return status;
}

CURLcode
pb_shorten (const char * provider, const char * url, const uint16_t verb) {

    CURLcode status = CURLE_OK;
    CURL * handle = curl_easy_init();

    if ( !handle ) {
        pbpst_err(_("Failed to get CURL handle"));
        return CURLE_FAILED_INIT;
    }

    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wdisabled-macro-expansion"
    curl_easy_setopt(handle, CURLOPT_VERBOSE, verb >= 2);
    #pragma clang diagnostic pop

    struct curl_httppost * post = NULL, * last = NULL;
    size_t tlen = strlen(provider) + 6;

    struct curl_slist * list = NULL;
    list = curl_slist_append(list, "Accept: application/json");
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wdisabled-macro-expansion"
    curl_easy_setopt(handle, CURLOPT_HTTPHEADER, list);
    #pragma clang diagnostic pop

    struct CurlResponse * response_data = malloc(sizeof(struct CurlResponse));
    char * target = malloc(tlen);

    if ( !response_data ) { status = CURLE_OUT_OF_MEMORY; goto cleanup; }
    response_data->mem = 0;
    if ( !target ) { status = CURLE_OUT_OF_MEMORY; goto cleanup; }

    CURLFORMcode fc;
    fc = curl_formadd(&post,                &last,
                      CURLFORM_COPYNAME,    "c",
                      CURLFORM_PTRCONTENTS, url,
                      CURLFORM_END);
    if ( fc ) { status = CURLE_HTTP_POST_ERROR; goto cleanup; }

    snprintf(target, tlen, "%s%c", provider, 'u');
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wdisabled-macro-expansion"
    curl_easy_setopt(handle, CURLOPT_HTTPPOST, post);
    curl_easy_setopt(handle, CURLOPT_URL, target);
    curl_easy_setopt(handle, CURLOPT_XFERINFOFUNCTION, &pb_progress_cb);
    curl_easy_setopt(handle, CURLOPT_NOPROGRESS, (long )false);
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, &pb_write_cb);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, response_data);
    #pragma clang diagnostic pop

    status = curl_easy_perform(handle);
    if ( status != CURLE_OK ) {
        #pragma clang diagnostic push
        #pragma clang diagnostic ignored "-Wdisabled-macro-expansion"
        fprintf(stderr, "pbpst: Shortening failed: %s\n",
                curl_easy_strerror(status)); goto cleanup;
        #pragma clang diagnostic pop
    }

    json_error_t err;
    json_t * json = json_loads(response_data->mem, 0, &err);
    if ( !json ) {
        #pragma clang diagnostic push
        #pragma clang diagnostic ignored "-Wdisabled-macro-expansion"
        fprintf(stderr, "pbpst: %s at %d:%d\n", err.text, err.line, err.column);
        #pragma clang diagnostic pop
        return EXIT_FAILURE;
    }

    json_t * value;
    const char * key;
    if ( verb ) {
        json_object_foreach(json, key, value) {
            printf("%s: %s\n", key, json_string_value(value));
        }
    } else {
        json_t * str = json_object_get(json, "url");
        if ( str ) { printf("%s\n", json_string_value(str)); }
    }

    cleanup:
        if ( list ) { curl_slist_free_all(list); }
        curl_easy_cleanup(handle);
        curl_formfree(post);
        if ( response_data ) {
            if ( response_data->mem ) { free(response_data->mem); }
            free(response_data);
        }
        free(target);
        return status;
}

CURLcode
pb_remove (const char * provider, const char * uuid, const uint16_t verb) {

    CURLcode status = CURLE_OK;
    CURL * handle = curl_easy_init();

    if ( !handle ) {
        pbpst_err(_("Failed to get CURL handle"));
        return CURLE_FAILED_INIT;
    }

    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wdisabled-macro-expansion"
    curl_easy_setopt(handle, CURLOPT_VERBOSE, verb >= 2);
    #pragma clang diagnostic pop

    struct curl_slist * list = NULL;
    list = curl_slist_append(list, "Accept: application/json");

    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wdisabled-macro-expansion"
    curl_easy_setopt(handle, CURLOPT_HTTPHEADER, list);
    #pragma clang diagnostic pop

    struct CurlResponse * response_data = malloc(sizeof(struct CurlResponse));
    size_t target_len = strlen(provider) + strlen(uuid) + 1;
    char * target = malloc(target_len);

    if ( !response_data ) { status = CURLE_OUT_OF_MEMORY; goto cleanup; }
    response_data->mem = 0;
    if ( !target ) { status = CURLE_OUT_OF_MEMORY; goto cleanup; }

    snprintf(target, target_len, "%s%s", provider, uuid);

    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wdisabled-macro-expansion"
    curl_easy_setopt(handle, CURLOPT_CUSTOMREQUEST, "DELETE");
    curl_easy_setopt(handle, CURLOPT_URL, target);
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, &pb_write_cb);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, response_data);
    #pragma clang diagnostic pop

    status = curl_easy_perform(handle);
    if ( status == CURLE_OK ) {
        long resp_code = 0;

        #pragma clang diagnostic push
        #pragma clang diagnostic ignored "-Wdisabled-macro-expansion"
        curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, &resp_code);
        #pragma clang diagnostic pop

        if ( resp_code == 200 ) {
            pbpst_err(_("Paste deleted"));
            db_remove_entry(provider, uuid);
        } else {
            #pragma clang diagnostic push
            #pragma clang diagnostic ignored "-Wdisabled-macro-expansion"
            fprintf(stderr, "pbpst: Failed to delete paste: %d\n", resp_code);
            #pragma clang diagnostic pop
        }
    }

    cleanup:
        if ( list ) { curl_slist_free_all(list); }
        curl_easy_cleanup(handle);
        if ( response_data ) {
            if ( response_data->mem ) { free(response_data->mem); }
            free(response_data);
        }
        free(target);
        return status;
}

CURLcode
pb_list (const struct pbpst_state * s) {

    CURLcode status = CURLE_OK;
    CURL * handle = curl_easy_init();

    if ( !handle ) {
        pbpst_err(_("Failed to get CURL handle"));
        return CURLE_FAILED_INIT;
    }

    struct curl_slist * list = NULL;
    list = curl_slist_append(list, "Accept: application/json");

    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wdisabled-macro-expansion"
    curl_easy_setopt(handle, CURLOPT_HTTPHEADER, list);
    curl_easy_setopt(handle, CURLOPT_VERBOSE, s->verb >= 2);
    #pragma clang diagnostic pop

    struct CurlResponse * response_data = malloc(sizeof(struct CurlResponse));
    size_t target_len = strlen(s->provider) + 3;
    char * target = malloc(target_len);

    if ( !response_data ) { status = CURLE_OUT_OF_MEMORY; goto cleanup; }
    response_data->mem = 0;
    if ( !target ) { status = CURLE_OUT_OF_MEMORY; goto cleanup; }

    const char route [] = { 'l', s->lthm ? 's' : s->lfrm ? 'f' : 0, 0 };
    snprintf(target, target_len, "%s%s", s->provider, route);

    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wdisabled-macro-expansion"
    curl_easy_setopt(handle, CURLOPT_URL, target);
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, &pb_write_cb);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, response_data);
    #pragma clang diagnostic pop

    status = curl_easy_perform(handle);
    if ( status != CURLE_OK ) {
        #pragma clang diagnostic push
        #pragma clang diagnostic ignored "-Wdisabled-macro-expansion"
        fprintf(stderr, "pbpst: Listing failed: %s\n",
                curl_easy_strerror(status)); goto cleanup;
        #pragma clang diagnostic pop
    }

    size_t idx;
    json_error_t err;
    json_t * value, * json = json_loads(response_data->mem, 0, &err);
    if ( !json ) {
        #pragma clang diagnostic push
        #pragma clang diagnostic ignored "-Wdisabled-macro-expansion"
        fprintf(stderr, "pbpst: %s at %d:%d\n", err.text, err.line, err.column);
        #pragma clang diagnostic pop
        return EXIT_FAILURE;
    }

    json_array_foreach (json, idx, value) {
        if ( json_is_array(value) ) {
            size_t inner_idx;
            json_t * inner_value;
            json_array_foreach(value, inner_idx, inner_value) {
                printf("%s ", json_string_value(inner_value));
            } putchar('\n');
        } else if ( json_is_string(value) ) {
            puts(json_string_value(value));
        }
    }

    cleanup:
        if ( list ) { curl_slist_free_all(list); }
        curl_easy_cleanup(handle);
        if ( response_data ) {
            if ( response_data->mem ) { free(response_data->mem); }
            free(response_data);
        }
        free(target);
        return status;
}

CURLcode
print_url (const struct pbpst_state * s, const char * userdata) {

    json_error_t err;
    json_t * json = json_loads(userdata, 0, &err);
    if ( !json ) {
        #pragma clang diagnostic push
        #pragma clang diagnostic ignored "-Wdisabled-macro-expansion"
        fprintf(stderr, "pbpst: %s at %d:%d\n", err.text, err.line, err.column);
        #pragma clang diagnostic pop
        return EXIT_FAILURE;
    }

    CURLcode status = EXIT_SUCCESS;
    json_t * uuid_j = 0, * lid_j = 0, * label_j = 0;

    char * hdln = 0, * lexr = 0, * form = 0, * them = 0, * extn = 0,
         * sunset = 0;

    uuid_j  = json_object_get(json, "uuid");
    lid_j   = json_object_get(json, "long");
    label_j = json_object_get(json, "label");
    json_incref(uuid_j);
    json_incref(lid_j);
    json_incref(label_j);

    const char * lid   = json_string_value(lid_j),
               * label = json_string_value(label_j);

    if ( s->verb ) {
        json_t * value;
        const char * key;
        json_object_foreach(json, key, value) {
            printf("%s: %s\n", key, json_string_value(value));
        } printf("murl: ");
    }

    const char * rndr = s->rend ? "r/" :
                        s->term ? "t/" : "",
               * idnt = label_j ? label : s->priv ? lid : lid + 24,
               * mod_fmts [] = { "#L-", "/", "?style=", ".", "/" };

    char * state_mod = 0, ** mod_var = 0,
         * mod_names [] = { "line", "lexer", "theme", "extension", "format" };

    for ( uint8_t i = 0; i < 5; i ++ ) {
        switch ( mod_names[i][1] ) {
            case 'i': mod_var = &hdln; state_mod = s->ln;     break;
            case 'e': mod_var = &lexr; state_mod = s->lexer;  break;
            case 'h': mod_var = &them; state_mod = s->theme;  break;
            case 'x': mod_var = &extn; state_mod = s->ext;    break;
            case 'o': mod_var = &form; state_mod = s->format; break;
        }

        if ( state_mod && *state_mod ) {
            size_t tlen = strlen(state_mod) + strlen(mod_fmts[i]);
            *mod_var = malloc(tlen + 2);
            if ( !*mod_var ) {
                #pragma clang diagnostic push
                #pragma clang diagnostic ignored "-Wdisabled-macro-expansion"
                fprintf(stderr, "pbpst: Could not modify %s: Out of Memory\n",
                                mod_names[i]);
                #pragma clang diagnostic pop
                goto cleanup;
            } snprintf(*mod_var, tlen + 1, "%s%s", mod_fmts[i], state_mod);
        } else if ( mod_var ) {
            *mod_var = "";
        } else {
            pbpst_err(_("Could not setup URL for printing"));
            status = EXIT_FAILURE; goto cleanup;
        }
    } printf("%s%s%s%s%s%s%s%s\n", s->provider, rndr, idnt, extn, lexr, form,
             them, hdln);

    cleanup:
        if ( s->ln )     { free(hdln);   }
        if ( s->lexer )  { free(lexr);   }
        if ( s->format ) { free(form);   }
        if ( s->theme )  { free(them);   }
        if ( s->ext )    { free(extn);   }
        if ( s->secs )   { free(sunset); }
        json_decref(json);
        json_decref(uuid_j);
        json_decref(lid_j);
        json_decref(label_j);
        return status;
}

signed
pb_prune (const struct pbpst_state * s) {

    pastes = json_object_get(mem_db, "pastes");
    json_incref(pastes);

    if ( !pastes ) { return EXIT_FAILURE; }
    prov_pastes = json_object_get(pastes, s->provider);
    json_incref(prov_pastes);

    if ( !prov_pastes ) {
        #pragma clang diagnostic push
        #pragma clang diagnostic ignored "-Wdisabled-macro-expansion"
        fprintf(stderr, "pbpst: No pastes found for: %s\n", s->provider);
        #pragma clang diagnostic pop
        return EXIT_FAILURE;
    }

    int64_t curtime = (int64_t )time(NULL);

    const char * key;
    json_t * val;
    json_object_foreach (prov_pastes, key, val) {
        json_t * res = json_object_get(val, "sunset");
        int64_t stime = 0;
        const char * stc = json_string_value(res);
        size_t stclen = json_string_length(res);

        if ( stclen && sscanf(stc, "%" SCNd64, &stime) == EOF ) {
            perror("pbpst: Failed to scan offset");
            return EXIT_FAILURE;
        }

        if ( stime > 0 && curtime > stime ) {
            if ( s->cmd == RMV ) {
                pb_remove(s->provider, key, s->verb);
            } else {
                db_remove_entry(s->provider, key);
            }
        }
    } return EXIT_SUCCESS;
}

// vim: set ts=4 sw=4 et:
