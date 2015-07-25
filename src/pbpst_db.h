#pragma once

#include <stdio.h>    // fputs(), remove()
#include <fcntl.h>    // open()
#include <unistd.h>   // close()
#include <sys/stat.h> // S_IRUSR
#include <stdlib.h>   // EXIT_FAILURE, EXIT_SUCCESS
#include <errno.h>    // errno
#include <string.h>   // strerror()

static const char lockfile_err [] =
    "Could not lock the database: %s\n"
    "If no other instances of pbpst are running, "
    "you can delete /tmp/pbpst.lck\n";

signed
db_lockfile_init (void);

signed
db_lockfile_cleanup (void);

// vim: set ts=4 sw=4 et:
