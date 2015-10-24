#include "main.h"
#include "callback.h"

signed
pb_progress_cb (void * client,
                curl_off_t dltotal, curl_off_t dlnow,
                curl_off_t ultotal, curl_off_t ulnow) {

    static curl_off_t last_progress;
    curl_off_t progress = ultotal ? ulnow * 100 / ultotal : 0,
               hashlen  = 73, hash = progress * hashlen / 100;

    if ( progress == last_progress ) { return 0; }

    fputs("\x1b[?25l[", stderr);
    for ( curl_off_t i = hashlen; i; -- i ) {
        fputc(i > hashlen - hash ? '#' : '-', stderr);
    } fprintf(stderr, "] %3" CURL_FORMAT_CURL_OFF_T "%%%s", progress,
                      progress == 100 ? "\x1b[?25h\n" : "\r");

    last_progress = progress;
    return 0;
}

size_t
pb_write_cb (char * ptr, size_t size, size_t nmemb, void * userdata) {

    if ( !ptr ) { return 0; }

    size_t rsize = size * nmemb;
    struct CurlResponse * udata = (struct CurlResponse * )userdata;

    char * mem = realloc(udata->mem, rsize + 2);
    if ( !mem ) {
        fputs("pbpst: Failed to allocate memory for response\n", stderr);
        return 0;
    } udata->mem = mem;

    if ( snprintf(udata->mem, rsize + 1, "%s", ptr) == -1 ) {
        fputs("pbpst: Failed writing response to memory\n", stderr);
        return 0;
    } return rsize;
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

// vim: set ts=4 sw=4 et:
