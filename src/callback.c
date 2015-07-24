#include <stdint.h>    // uint8_t
#include <stdio.h>     // fputs(), fputc(), fprintf(), fflush()
#include <inttypes.h>  // PRIu8
#include <curl/curl.h> // curl_off_t
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

    fill_progress((uint8_t )progress, 40);
    last_progress = progress;
    return 0;
}

// vim: set ts=4 sw=4 et:
