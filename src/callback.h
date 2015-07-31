#pragma once

signed
pb_progress_cb (void *, curl_off_t, curl_off_t, curl_off_t, curl_off_t);

size_t
pb_write_cb (char *ptr, size_t size, size_t nmemb, void *userdata);

// vim: set ts=4 sw=4 et:
