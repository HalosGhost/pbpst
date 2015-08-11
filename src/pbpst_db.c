#include "pbpst_db.h"
#include <linux/limits.h>

char *
db_locate (const struct pbpst_state * s) {

    char * db_loc, * db = 0;
    enum {
        FLE = 0, USR = 1, XDG = 2, HME = 3
    } which_brnch = (db_loc = s->dbfile)                 ? USR
                  : (db_loc = getenv("XDG_CONFIG_HOME")) ? XDG
                  : (db_loc = getenv("HOME"))            ? HME : FLE;

    if ( which_brnch == FLE ) {
        fputs("pbpst: No valid location for database\n", stderr);
        return 0;
    }

    signed errsv;
    size_t db_len;
    if ( which_brnch != USR ) {
        char cwd [PATH_MAX] = { '\0' };
        errno = 0;
        if ( !getcwd(cwd, PATH_MAX - 1) ) {
            errsv = errno;
            fprintf(stderr, "pbpst: Could not save cwd: %s\n", strerror(errsv));
            return 0;
        }

        errno = 0;
        if ( chdir(db_loc) == -1 ) {
            errsv = errno;
            fprintf(stderr, "pbpst: Could not cd to %s: %s\n", db_loc,
                    strerror(errsv)); return 0;
        }

        for ( uint8_t i = 0; i < 2; i ++ ) {
            i += which_brnch == XDG;
            char * str = !i ? ".config" : "pbpst";

            errno = 0;
            if ( chdir(str) == -1 ) {
                errsv = errno;
                if ( errsv == ENOENT ) {
                    errno = 0;
                    if ( mkdir(str, 0777) == -1 ) {
                        errsv = errno;
                        fprintf(stderr, "pbpst: Could not create %s/%s: %s\n",
                                db_loc, str, strerror(errsv)); return 0;
                    }
                } else {
                    fprintf(stderr, "pbpst: Could not cd to %s/%s: %s\n",
                            db_loc, str, strerror(errsv)); return 0;
                }
            }
        }

        errno = 0;
        if ( chdir(cwd) == -1 ) {
            errsv = errno;
            fprintf(stderr, "pbpst: Could not return to %s: %s\n",
                    cwd, strerror(errsv)); return 0;
        }

        db_len = strlen(db_loc) + 23;
        db = (char * )malloc(db_len);
        if ( !db ) {
            fprintf(stderr, "pbpst: Could not save db path: Out of Memory\n");
            return 0;
        }

        snprintf(db, db_len, "%s%s/pbpst/db.json", db_loc,
                 which_brnch == XDG ? "" : "/.config");
    }

    char * fdb = which_brnch == USR ? db_loc : db;

    signed fd;
    errno = 0;
    if ( (fd = open(fdb, O_CREAT | O_EXCL, 0666)) == -1 ) {
        errsv = errno;
        if ( errsv == EEXIST ) {
            return fdb;
        } else {
            fprintf(stderr, db_err, fdb, strerror(errsv));
            return 0;
        }
    } close(fd); return fdb;
}

signed
db_swp_init (const char * db_loc) {

    return 0;
}

// vim: set ts=4 sw=4 et:
