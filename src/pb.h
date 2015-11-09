#pragma once

CURLcode
pb_paste (const struct pbpst_state *);

CURLcode
pb_remove (const char *, const char *, const uint16_t);

CURLcode
print_url (const struct pbpst_state *, const char *);

CURLcode
pb_list (const struct pbpst_state *);

signed
pb_prune (const struct pbpst_state * s);

// vim: set ts=4 sw=4 et:
