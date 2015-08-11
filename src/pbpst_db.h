#pragma once

#include "main.h"

static const char swp_db_err [] =
    "pbpst: Could not create the swap db: %s\n"
    "pbpst: If no other instances of pbpst are running and all paste data "
    "has been saved to the database, you can manually remove: %s/%s\n";

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
