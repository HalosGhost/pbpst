#include "pbpst_db.h"

signed
db_lockfile_init (void) {

    signed fd;
    if ( (fd = open("/tmp/pbpst.lck", O_CREAT | O_EXCL, NULL)) == -1 ) {
        signed errsv = errno;
        fprintf(stderr, lockfile_err, strerror(errsv));
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

signed
db_locate (void) {

    return 0;
}

// vim: set ts=4 sw=4 et:
