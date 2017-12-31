#pragma once

#include "main.h"

#ifdef __APPLE__
#include <sys/syslimits.h>
#else
#include <linux/limits.h>
#endif

#include <libgen.h>

#define DEF_DB() json_pack("{s:s,s:{}}", "default_provider",\
                           FALLBACK_PROVIDER, "pastes")

signed
print_err2 (const char *, const char *);

signed
print_err3 (const char *, const char *, const char *);

char *
db_locate (const struct pbpst_state *);

char *
db_swp_init (const char *);

signed
db_swp_cleanup (const char *, const char *);

json_t *
db_read (const char *);

signed
db_swp_flush (const json_t *, const char *);

signed
db_set_default (const char *);

signed
pbpst_db (const struct pbpst_state *);

signed
db_add_entry (const struct pbpst_state *, const char *);

signed
db_remove_entry (const char *, const char *);

signed
db_query (const struct pbpst_state *);

signed
db_list_providers (void);

// vim: set ts=4 sw=4 et:
