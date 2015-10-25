#pragma once

struct CurlResponse {
    char * mem;
};

signed
pb_progress_cb (void *, curl_off_t, curl_off_t, curl_off_t, curl_off_t);

size_t
pb_write_cb (char *, size_t, size_t, void *);

// vim: set ts=4 sw=4 et:
