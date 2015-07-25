#include <stdio.h>    // fprintf(), FILE
#include <fcntl.h>    // open()
#include <unistd.h>   // chdir()
#include <stdlib.h>   // exit()
#include "pbpst_db.h"

signed
db_lockfile_init (void) {
    open("/tmp/ptpst.lock", O_RDWR | O_CREAT, 0200);
    return 0;
}

signed
db_lockfile_cleanup (void) {
    remove("/tmp/ptpst.lock");
    return 0;
}

// vim: set ts=4 sw=4 et:
