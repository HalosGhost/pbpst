#include <stdio.h>    // fprintf(), FILE
#include <fcntl.h>    // open()
#include <unistd.h>   // chdir()
#include <stdlib.h>   // exit()
#include "pbpst_db.h"

signed
db_lockfile_init (void) {
    signed fd; // Integer for file descriptor
    if ( (fd = open("/tmp/pbpst.lck", O_RDONLY | O_CREAT | O_EXCL, S_IRUSR)) == -1 ) {
        fprintf(stderr, "Cannot open /tmp/ptpst.lck.\n");
        return EXIT_FAILURE;
    }
    close(fd);
    return EXIT_SUCCESS;
}

signed
db_lockfile_cleanup (void) {
    remove("/tmp/ptpst.lck");
    return EXIT_SUCCESS;
}

// vim: set ts=4 sw=4 et:
