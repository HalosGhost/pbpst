#include "main.h"
#include "callback.h"

signed
pb_progress_cb (void * client,
                curl_off_t dltotal, curl_off_t dlnow,
                curl_off_t ultotal, curl_off_t ulnow) {

    (void )client; (void )dltotal; (void )dlnow; // unused

    static curl_off_t last_progress;
    curl_off_t progress = ultotal ? ulnow * 100 / ultotal : 0,
               hashlen  = 73, hash = progress * hashlen / 100;

    if ( progress == last_progress ) { return 0; }

    if ( state.prog ) {
        fputs("\x1b[?25l[", stderr);
        for ( curl_off_t i = hashlen; i; -- i ) {
            fputc(i > hashlen - hash ? '#' : '-', stderr);
        }

        #pragma clang diagnostic push
        #pragma clang diagnostic ignored "-Wdisabled-macro-expansion"
        fprintf(stderr, "] %3" CURL_FORMAT_CURL_OFF_T "%%%s", progress,
                        progress == 100 ? "\x1b[?25h\n" : "\r");
        #pragma clang diagnostic pop
    }

    point_of_no_return = progress == 100;
    last_progress = progress;
    return 0;
}

size_t
pb_write_cb (char * restrict ptr, size_t size, size_t nmemb, void * userdata) {

    if ( !ptr ) { return 0; }

    size_t rsize = size * nmemb;
    struct CurlResponse * udata = (struct CurlResponse * )userdata;

    char * mem = realloc(udata->mem, rsize + 2);
    if ( !mem ) {
        pbpst_err(_("Could not allocate memory for response"));
        return 0;
    } udata->mem = mem;

    if ( snprintf(udata->mem, rsize + 1, "%s", ptr) == -1 ) {
        pbpst_err(_("Could not write response to memory"));
        return 0;
    } return rsize;
}

// vim: set ts=4 sw=4 et:
