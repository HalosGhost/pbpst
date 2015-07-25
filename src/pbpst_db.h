#pragma once

#include "main.h"

static const char lockfile_err [] =
    "pbpst: Could not lock the database: %s\n"
    "pbpst: If no other instances of pbpst are running, "
    "you can delete /tmp/pbpst.lck\n";

static const char def_db [] =
    "{\n"
    "   \"default_provider\": \"https://ptpb.pw\",\n"
    "   \"pastes\": {\n"
    "   }\n"
    "}\n";

signed
db_lockfile_init (void);

signed
db_lockfile_cleanup (void);

signed
db_locate (void);

// vim: set ts=4 sw=4 et:
