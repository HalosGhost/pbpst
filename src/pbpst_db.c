#include "pbpst_db.h"

char *
db_locate (const struct pbpst_state * s) {

    char * dbl, * db = 0;
    enum {
        FLE = 0, USR = 1, XDG = 2, HME = 3
    } which_brnch = (dbl = s->dbfile)                 ? USR
                  : (dbl = getenv("XDG_CONFIG_HOME")) ? XDG
                  : (dbl = getenv("HOME"))            ? HME : FLE;

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
        if ( chdir(dbl) == -1 ) {
            errsv = errno;
            fprintf(stderr, "pbpst: Could not cd to %s: %s\n", dbl,
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
                                dbl, str, strerror(errsv)); return 0;
                    }
                } else {
                    fprintf(stderr, "pbpst: Could not cd to %s/%s: %s\n",
                            dbl, str, strerror(errsv)); return 0;
                }
            }
        }

        errno = 0;
        if ( chdir(cwd) == -1 ) {
            errsv = errno;
            fprintf(stderr, "pbpst: Could not return to %s: %s\n",
                    cwd, strerror(errsv)); return 0;
        }

        db_len = strlen(dbl) + 23;
        db = (char * )malloc(db_len);
        if ( !db ) {
            fprintf(stderr, "pbpst: Could not save db path: Out of Memory\n");
            return 0;
        }

        snprintf(db, db_len, "%s%s/pbpst/db.json", dbl,
                 which_brnch == XDG ? "" : "/.config");
    }

    char * fdb = which_brnch == USR ? dbl : db;

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

char *
db_swp_init (const char * dbl) {

    size_t len = strlen(dbl) + 1;
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

    snprintf(pc, len, "%s", dbl);
    snprintf(fc, len, "%s", dbl);

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
                strerror(errsv)); fd = -1; goto cleanup;
    }

    len = strlen(dbl) + 7;
    swp_db_name = (char * )malloc(len);
    if ( !swp_db_name ) {
        fprintf(stderr, "pbpst: Could not save swap db name: Out of Memory\n");
        fd = -1; goto cleanup;
    }

    snprintf(swp_db_name, len, "%s/.%s.swp", parent, file);

    errno = 0;
    if ( (fd = open(swp_db_name, O_CREAT | O_EXCL, 0666)) == -1 ) {
        errsv = errno;
        fprintf(stderr, swp_db_err, strerror(errsv), parent, swp_db_name);
        fd = -1; goto cleanup;
    }

    errno = 0;
    if ( close(fd) == -1 ) {
        errsv = errno;
        fprintf(stderr, "pbpst: Could not close %s: %s\n", swp_db_name,
                strerror(errsv)); fd = -1; goto cleanup;
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
        if ( fd == -1 ) {
            free(swp_db_name);
            return 0;
        } else {
            return swp_db_name;
        }
}

signed
db_swp_cleanup (const char * dbl, const char * s_dbl) {

    errno = 0;
    if ( rename(s_dbl, dbl) == -1 ) {
        signed errsv = errno;
        fprintf(stderr, "pbpst: Failed to save %s to %s: %s\n", s_dbl,
                dbl, strerror(errsv)); return -1;
    } return 0;
}

json_t *
db_read (const char * dbl) {

    FILE * f;
    signed errsv;
    errno = 0;
    if ( !(f = fopen(dbl, "r")) ) {
        errsv = errno;
        fprintf(stderr, "pbpst: Could not open %s for reading: %s\n", dbl,
                strerror(errsv)); return 0;
    }

    json_error_t err;
    json_t * mdb = json_loadf(f, 0, &err);
    if ( !mdb ) {
        errno = 0;
        if ( (fseek(f, 0, SEEK_END)) == -1 ) {
            errsv = errno;
            fprintf(stderr, "pbpst: Failed to seek to end of %s: %s\n", dbl,
                    strerror(errsv)); goto cleanup;
        }

        signed long size = 0;
        errno = 0;
        if ( (size = ftell(f)) == -1 ) {
            errsv = errno;
            fprintf(stderr, "pbpst: failed to check position in %s: %s\n",
                    dbl, strerror(errsv)); goto cleanup;
        }

        if ( size == 0 ) {
            mdb = DEF_DB();
            goto cleanup;
        }

        fprintf(stderr, "pbpst: Failed reading %s: %s\n", dbl, err.text);
        goto cleanup;
    }

    cleanup:
        errno = 0;
        if ( fclose(f) == -1 ) {
            errsv = errno;
            fprintf(stderr, "pbpst: Could not close %s: %s\n", dbl,
                    strerror(errsv)); json_decref(mdb); mdb = 0;
        } return mdb;
}

signed
db_swp_flush (const json_t * mdb, const char * s_dbl) {

    FILE * swp_db;
    signed errsv;
    errno = 0;
    if ( !(swp_db = fopen(s_dbl, "w")) ) {
        errsv = errno;
        fprintf(stderr, "pbpst: Could not open swap db: %s\n",
                strerror(errsv)); return -1;
    }

    signed ret = 0;
    if ( json_dumpf(mdb, swp_db, JSON_PRESERVE_ORDER | JSON_INDENT(2)) == -1 ) {
        ret = -1;
    }

    errno = 0;
    if ( fflush(swp_db) == EOF ) {
        errsv = errno;
        fprintf(stderr, "pbpst: Could not flush memory to swap db: %s\n",
                strerror(errsv)); ret = -1;
    }

    errno = 0;
    if ( fclose(swp_db) == -1 ) {
        errsv = errno;
        fprintf(stderr, "pbpst: Could not close swap db: %s\n",
                strerror(errsv)); ret = -1;
    } return ret;
}

// vim: set ts=4 sw=4 et:
