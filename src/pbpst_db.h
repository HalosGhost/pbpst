#pragma once

#include "main.h"

static const char db_err [] =
    "pbpst: Error opening %s: %s\n";

static const char def_db [] =
    "{\n"
    "   \"default_provider\": \"https://ptpb.pw\",\n"
    "   \"pastes\": {\n"
    "   }\n"
    "}\n";

char *
db_locate (const struct pbpst_state *);

signed
db_swp_init (const char *);

// vim: set ts=4 sw=4 et:
