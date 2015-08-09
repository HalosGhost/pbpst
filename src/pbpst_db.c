#include "pbpst_db.h"
#include <linux/limits.h>

signed
db_locate (const struct pbpst_state * s) {


    mode_t dirmode = S_IRUSR | S_IWUSR | S_IXUSR
                   | S_IRGRP           | S_IXGRP
                   | S_IROTH           | S_IXOTH;

    mode_t dbmode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

    signed fd, errsv;
    if ( s->dbfile ) {
        errno = 0;
        if ( (fd = open(s->dbfile, O_CREAT | O_EXCL, dbmode)) == -1 ) {
            errsv = errno;
            if ( errsv == EEXIST ) {
                return 1;
            } else {
                fprintf(stderr, db_err, s->dbfile, strerror(errsv));
                return 0;
            }
        } close(fd); return 1;
    }

    char cwd [PATH_MAX] = { '\0' };
    errno = 0;
    if ( !getcwd(cwd, PATH_MAX - 1) ) {
        errsv = errno;
        fprintf(stderr, "pbpst: Could not save cwd: %s\n", strerror(errsv));
        return 0;
    }

    char * xch_loc = getenv("XDG_CONFIG_HOME");
    if ( xch_loc ) {
        errno = 0;
        if ( chdir(xch_loc) == -1 ) {
            errsv = errno;
            fprintf(stderr, "pbpst: Could not cd to %s: %s\n", xch_loc,
                    strerror(errsv));
            return 0;
        }

        errno = 0;
        if ( chdir("pbpst") == -1 ) {
            errsv = errno;
            if ( errsv == ENOENT ) {
                errno = 0;
                if ( mkdir("pbpst", dirmode) == -1 ) {
                    errsv = errno;
                    fprintf(stderr, "pbpst: Could not create %s/pbpst: %s\n",
                            xch_loc, strerror(errsv));
                    return 0;
                }
            } else {
                fprintf(stderr, "pbpst: Could not cd to %s/pbpst: %s\n",
                        xch_loc, strerror(errsv));
                return 0;
            }
        }

        errno = 0;
        if ( chdir(cwd) == -1 ) {
            errsv = errno;
            fprintf(stderr, "pbpst: Could not return to %s: %s\n",
                    cwd, strerror(errsv));
            return 0;
        }

        char * xch_db = (char * )malloc(strlen(xch_loc) + 15);
        if ( !xch_db ) {
            fprintf(stderr, "pbpst: Out of Memory\n");
            return 0;
        }

        strncpy(xch_db, xch_loc, strlen(xch_loc));
        strncat(xch_db, "/pbpst/db.json", 14);

        errno = 0;
        if ( (fd = open(xch_db, O_CREAT | O_EXCL, dbmode)) == -1 ) {
            errsv = errno;
            if ( errsv == EEXIST ) {
                free(xch_db);
                return 2;
            }
        } close(fd); free(xch_db); return 2;
    }

    char * h_loc = getenv("HOME");
    if ( !h_loc ) {
        fprintf(stderr, "pbpst: Error locating HOME directory\n");
        return 0;
    }

    errno = 0;
    if ( chdir(".config") == -1 ) {
        errsv = errno;
        if ( errsv == ENOENT ) {
            errno = 0;
            if ( mkdir(".config", dirmode) == -1 ) {
                errsv = errno;
                fprintf(stderr, "pbpst: Could not create %s/.config: %s\n",
                        h_loc, strerror(errsv));
                return 0;
            }
        } else {
            fprintf(stderr, "pbpst: Could not cd to %s/.config: %s\n",
                    h_loc, strerror(errsv));
            return 0;
        }
    }

    errno = 0;
    if ( chdir("pbpst") == -1 ) {
        errsv = errno;
        if ( errsv == ENOENT ) {
            errno = 0;
            if ( mkdir("pbpst", dirmode) == -1 ) {
                errsv = errno;
                fprintf(stderr, "pbpst: Could not create %s/%s: %s\n",
                        h_loc, ".config/pbpst", strerror(errsv));
                return 0;
            }
        } else {
            fprintf(stderr, "pbpst: Could not cd to %s/%s: %s\n",
                    h_loc, ".config/pbpst", strerror(errsv));
            return 0;
        }
    }

    errno = 0;
    if ( chdir(cwd) == -1 ) {
        errsv = errno;
        fprintf(stderr, "pbpst: Could not return to %s: %s\n",
                cwd, strerror(errsv));
        return 0;
    }

    errno = 0;
    char * hdb = (char * )malloc(strlen(h_loc) + 23);
    if ( !hdb ) {
        fprintf(stderr, "pbpst: Out of Memory\n");
        return 0;
    }

    strncpy(hdb, h_loc, strlen(h_loc));
    strncat(hdb, "/.config/pbpst/db.json", 22);

    errno = 0;
    if ( (fd = open(hdb, O_CREAT | O_EXCL, dbmode)) == -1 ) {
        errsv = errno;
        if ( errsv == EEXIST ) {
            free(hdb);
            return 3;
        } else {
            free(hdb);
            fprintf(stderr, db_err, hdb, strerror(errsv));
            return 0;
        }
    } close(fd); free(hdb); return 3;
}

// vim: set ts=4 sw=4 et:
