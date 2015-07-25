#include <stdio.h>    // fprintf(), FILE
#include <fcntl.h>    // open()
#include <unistd.h>   // chdir()
#include <stdlib.h>   // exit()
#include "pbpst_db.h"

#define LOCKFILE "/tmp/ptpst.lock"

signed
db_lockfile_init (void) {
    int fd; // Integer for file descriptor
    if (fd = open(LOCKFILE, O_RDONLY | O_CREAT | O_EXCL, 0200) == -1) {
        fprintf(stderr, "Cannot open /tmp/ptpst.lock.\n");
        exit(1);
    }
    return 0;
}

signed
db_lockfile_cleanup (void) {
    remove(LOCKFILE);
    return 0;
}

// vim: set ts=4 sw=4 et:
