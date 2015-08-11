#include "pbpst_db.h"
#include <linux/limits.h>
#include <libgen.h>

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
            fprintf(stderr, "pbpst: Failed to open %s: %s\n", fdb,
                    strerror(errsv)); return 0;
        }
    }

    errno = 0;
    if ( close(fd) == -1 ) {
        errsv = errno;
        fprintf(stderr, "pbpst: Failed to close %s: %s\n", fdb,
                strerror(errsv)); return 0;
    } return fdb;
}

signed
db_swp_init (const char * db_loc) {

    size_t len = strlen(db_loc) + 1;
    char * parent = 0, * file = 0, * fc = 0, * swp_db_name = 0,
         * pc = (char * )malloc(len);

    signed fd = 0;

    if ( !pc ) {
        fprintf(stderr, "pbpst: Could not store db dirname: Out of Memory\n");
        fd = -1; goto cleanup;
    }

    fc = (char * )malloc(len);
    if ( !fc ) {
        fprintf(stderr, "pbpst: Could not store db basename: Out of Memory\n");
        fd = -1; goto cleanup;
    }

    snprintf(pc, len, "%s", db_loc);
    snprintf(fc, len, "%s", db_loc);

    parent = dirname(pc);
    file = basename(fc);

    char cwd [PATH_MAX] = { '\0' };

    signed errsv = 0;
    errno = 0;
    if ( !getcwd(cwd, PATH_MAX - 1) ) {
        errsv = errno;
        fprintf(stderr, "pbpst: Could not save cwd: %s\n", strerror(errsv));
        fd = -1; goto cleanup;
    }

    errno = 0;
    if ( chdir(parent) == -1 ) {
        errsv = errno;
        fprintf(stderr, "pbpst: Could not cd to db path: %s\n",
                strerror(errsv));
        fd = -1; goto cleanup;
    }

    len = strlen(file) + 6;
    swp_db_name = (char * )malloc(len);
    if ( !swp_db_name ) {
        fprintf(stderr, "pbpst: Could not save swap db name: Out of Memory\n");
        fd = -1; goto cleanup;
    }

    snprintf(swp_db_name, len, ".%s.swp", file);

    errno = 0;
    if ( (fd = open(swp_db_name, O_CREAT | O_EXCL, 0666)) == -1 ) {
        errsv = errno;
        fprintf(stderr, swp_db_err, strerror(errsv), parent, swp_db_name);
        fd = -1; goto cleanup;
    }

    errno = 0;
    if ( chdir(cwd) == -1 ) {
        errsv = errno;
        fprintf(stderr, "pbpst: Could not return to %s: %s\n",
                cwd, strerror(errsv));

        errno = 0;
        if ( close(fd) == -1 ) {
            errsv = errno;
            fprintf(stderr, "pbpst: Failed to close %s: %s\n", swp_db_name,
                    strerror(errsv)); fd = -1; goto cleanup;
        }
    }

    cleanup:
        free(pc);
        free(fc);
        free(swp_db_name);
        return fd;
}

signed
db_swp_cleanup (const char * db_loc, signed swp_fd) {

    size_t len = strlen(db_loc) + 1;
    char * parent = 0, * file = 0, * fc = 0, * swp_db_path = 0,
         * pc = (char * )malloc(len);

    signed errsv = 0, ret = 0;

    if ( !pc ) {
        fprintf(stderr, "pbpst: Could not store db dirname: Out of Memory\n");
        ret = -1; goto cleanup;
    }

    fc = (char * )malloc(len);
    if ( !fc ) {
        fprintf(stderr, "pbpst: Could not store db basename: Out of Memory\n");
        ret = -1; goto cleanup;
    }

    snprintf(pc, len, "%s", db_loc);
    snprintf(fc, len, "%s", db_loc);

    parent = dirname(pc);
    file = basename(fc);

    len = strlen(db_loc) + 6;
    swp_db_path = (char * )malloc(len);
    if ( !swp_db_path ) {
        fprintf(stderr, "pbpst: Could not save swap db name: Out of Memory\n");
        ret = -1; goto cleanup;
    }

    snprintf(swp_db_path, len, "%s/.%s.swp", parent, file);

    errno = 0;
    if ( close(swp_fd) == -1 ) {
        errsv = errno;
        fprintf(stderr, "pbpst: Could not close %s: %s\n", swp_db_path,
                strerror(errsv)); ret = -1; goto cleanup;
    }

    errno = 0;
    if ( rename(swp_db_path, db_loc) == -1 ) {
        errsv = errno;
        fprintf(stderr, "pbpst: Failed to save %s to %s: %s\n", swp_db_path,
                db_loc, strerror(errsv)); ret = -1; goto cleanup;
    }

    cleanup:
        free(pc);
        free(fc);
        free(swp_db_path);
        return ret;
}

// vim: set ts=4 sw=4 et:
