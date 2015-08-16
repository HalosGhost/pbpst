#include "main.h"
#include "callback.h"

/* adapted from pacman */
static void
fill_progress (const uint8_t bar_percent, const uint8_t proglen) {

    /* 8 = 1 space + 1 [ + 1 ] + 5 for percent */
    const uint8_t hashlen = proglen > 8 ? proglen - 8 : 0,
                  hash    = bar_percent * hashlen / 100;

    if ( hashlen > 0 ) {
        fputs(" [", stderr);
        for ( uint8_t i = hashlen; i; --i ) {
            fputc(i > (hashlen - hash) ? '#' : '-', stderr);
        } fputc(']', stderr);
    }

    /* print display percent after progress bar */
    if ( proglen >= 5 ) { fprintf(stderr, " %3" PRIu8 "%%", bar_percent); }

    fputc(bar_percent == 100 ? '\n' : '\r', stderr);
    fflush(stderr);
}

signed
pb_progress_cb (void * client,
                curl_off_t dltotal, curl_off_t dlnow,
                curl_off_t ultotal, curl_off_t ulnow) {

    static curl_off_t last_progress;
    curl_off_t progress = ultotal ? ulnow * 100 / ultotal : 0;

    if ( progress == last_progress ) { return 0; }

    fill_progress((uint8_t )progress, 80);
    last_progress = progress;
    return 0;
}

size_t
pb_write_cb (char * ptr, size_t size, size_t nmemb, void * userdata) {

    if ( !ptr || !userdata ) { return 0; }

    size_t rsize = size * nmemb;
    *(ptr + rsize) = '\0';

    if ( state.url ) { printf("%s", ptr); return rsize; }

    json_t * json = json_loads(ptr, 0, NULL);
    if ( !json ) { return 0; }

    pastes = json_object_get(mem_db, "pastes");
    json_t * prov_obj = 0, * uuid_j = 0, * lid_j = 0,
           * label_j = 0, * status_j = 0, * new_paste = 0;

    char * hdln = 0, * lexr = 0, * them = 0, * extn = 0;

    const char * provider = def_provider ? def_provider : state.provider;

    if ( !pastes ) { goto cleanup; }
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

    if ( !status_j ) { goto cleanup; }
    const char stat = json_string_value(status_j)[0];
    if ( stat == 'a' ) {
        fputs("pbpst: Paste already existed\n", stderr);
        goto cleanup;
    } else if ( stat == 'd' ) {
        json_object_del(prov_pastes, state.uuid);
        if ( state.verb ) {
            json_t * value;
            const char * key;
            json_object_foreach(json, key, value) {
                printf("%s: %s\n", key, json_string_value(value));
            }
        } goto cleanup;
    }

    if ( (!uuid_j && !state.uuid) || !lid_j ) { goto cleanup; }
    const char * uuid  = uuid_j ? json_string_value(uuid_j) : state.uuid,
               * lid   = json_string_value(lid_j),
               * label = json_string_value(label_j);

    const char * msg =  state.msg               ? state.msg
                     : !state.msg && state.path ? state.path : "-";
    new_paste = json_pack(label_j ? "{s:s,s:s,s:s}" : "{s:s,s:s,s:n}", "long", lid,
                                    "msg", msg, "label", label);

    if ( json_object_set(prov_pastes, uuid, new_paste) == -1 ) {
        fputs("pbpst: Failed to create new paste object\n", stderr);
        goto cleanup;
    }

    if ( state.verb ) {
        json_t * value;
        const char * key;
        json_object_foreach(json, key, value) {
            printf("%s: %s\n", key, json_string_value(value));
        } printf("murl: ");
    }

    const char * rndr = state.rend ? "r/" : "",
               * idnt = label_j ? label : state.priv ? lid : lid + 24;

    if ( state.ln ) {
        hdln = malloc(14);
        if ( !hdln ) {
            fputs("pbpst: Could not store line modifier: Out of Memory\n",
                  stderr); goto cleanup;
        } snprintf(hdln, 13, "#L-%" PRIu32, state.ln);
    } else { hdln = ""; }

    if ( state.lexer ) {
        size_t tlen = strlen(state.lexer);
        lexr = malloc(tlen + 3);
        if ( !lexr ) {
            fputs("pbpst: Could not store lexer modifier: Out of Memory\n",
                  stderr); goto cleanup;
        } snprintf(lexr, tlen + 2, "/%s", state.lexer);
    } else { lexr = ""; }

    if ( state.theme ) {
        size_t tlen = strlen(state.theme);
        them = malloc(tlen + 9);
        if ( !them ) {
            fputs("pbpst: Could not store theme modifier: Out of Memory\n",
                  stderr); goto cleanup;
        } snprintf(them, tlen + 8, "?style=%s", state.theme);
    } else { them = ""; }

    if ( state.ext ) {
        size_t tlen = strlen(state.ext);
        extn = malloc(tlen + 3);
        if ( !extn ) {
            fputs("pbpst: Could not store extension modifier: Out of Memory\n",
                  stderr); goto cleanup;
        } snprintf(extn, tlen + 2, ".%s", state.ext);
    } else { extn = ""; }

    printf("%s%s%s%s%s%s%s\n", provider, rndr, idnt, extn, lexr, them, hdln);

    cleanup:
        if ( state.ln ) { free(hdln); }
        if ( state.lexer ) { free(lexr); }
        if ( state.theme ) { free(them); }
        if ( state.ext ) { free(extn); }
        json_decref(json);
        json_decref(uuid_j);
        json_decref(lid_j);
        json_decref(label_j);
        json_decref(status_j);
        json_decref(new_paste);
        return rsize;
}

// vim: set ts=4 sw=4 et:
