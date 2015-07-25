#include <stdio.h>    // fputs(), remove()
#include <fcntl.h>    // open()
#include <unistd.h>   // close()
#include <sys/stat.h> // S_IRUSR
#include <stdlib.h>   // EXIT_FAILURE, EXIT_SUCCESS
#include "pbpst_db.h"

signed
db_lockfile_init (void) {

    signed fd;
    if ( (fd = open("/tmp/pbpst.lck", O_CREAT | O_EXCL, NULL)) == -1 ) {
        fputs("Cannot open /tmp/ptpst.lck.\n", stderr);
        return EXIT_FAILURE;
    }

    close(fd);
    return EXIT_SUCCESS;
}

signed
db_lockfile_cleanup (void) {

    remove("/tmp/pbpst.lck");
    return EXIT_SUCCESS;
}

// vim: set ts=4 sw=4 et:
