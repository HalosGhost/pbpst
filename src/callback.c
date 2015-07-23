#include <stdio.h>
#include <curl/curl.h>

#include "callback.h"

/* adapted from pacman */
static
void fill_progress (const int bar_percent, const int disp_percent,
                    const int proglen)
{
    /* 8 = 1 space + 1 [ + 1 ] + 5 for percent */
    const int hashlen = proglen > 8 ? proglen - 8 : 0;
    const int hash = bar_percent * hashlen / 100;
    static int lasthash = 0, mouth = 0;
    int i;

    if(bar_percent == 0) {
        lasthash = 0;
        mouth = 0;
    }

    if(hashlen > 0) {
        fputs(" [", stdout);
        for (i = hashlen; i > 0; --i) {
            if (i > hashlen - hash)
                putchar('#');
            else
                putchar('-');
        }
        putchar(']');
    }

    /* print display percent after progress bar */
    if(proglen >= 5)
        printf(" %3d%%", disp_percent);

    if(bar_percent == 100)
        putchar('\n');
    else
        putchar('\r');

    fflush(stdout);
}

int
pb_progress_cb (void *client,
                curl_off_t dltotal, curl_off_t dlnow,
                curl_off_t ultotal, curl_off_t ulnow) {

    long progress;
    static long last_progress;

    progress = ultotal ? ulnow * 100 / ultotal : 0;

    if (progress == last_progress)
        return 0;

    fill_progress((int)progress, (int)progress, 40);
    last_progress = progress;

    return 0;
}
