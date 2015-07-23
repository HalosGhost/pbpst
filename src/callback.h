#pragma once

int
pb_progress_cb (void *client,
                curl_off_t dltotal, curl_off_t dlnow,
                curl_off_t ultotal, curl_off_t ulnow);
