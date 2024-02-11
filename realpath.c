/* This program is my realpath() implementation. 
 * This program gets a path from argv and pass it to the function. 
 * You can pass multiple arguments to the program. */

#define _POSIX_SOURCE
#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <libgen.h>

typedef enum {
    DEFAULT,
    DIRECTORY,
    REG,
    LNK
} FILETYPE;

char *myRealpath(const char *path, char *resolved_path)
{
    if (path == NULL) {
        errno = EINVAL;
        return NULL;
    }

    if (path[0] == '/') {
        strncpy(resolved_path, path, PATH_MAX);
        return resolved_path;
    }

    if (resolved_path == NULL) {
        resolved_path = malloc(PATH_MAX);
        if (resolved_path == NULL) {
            perror("myRealpath.malloc");
            exit(EXIT_FAILURE);
        }
    }

    struct stat info;
    FILETYPE ft = DEFAULT;

    if (lstat(path, &info) == -1) {
        return NULL;
    }
    
    if (S_ISDIR(info.st_mode)) {
        ft = DIRECTORY;
    } else if (S_ISREG(info.st_mode)) {
        ft = REG;
    } else if (S_ISLNK(info.st_mode)) {
        ft = LNK;
    }

    char cwd[PATH_MAX];
    char current[PATH_MAX];
    char t1[PATH_MAX];
    char t2[PATH_MAX];
    char *dname;
    char *fname;
    switch(ft) {
        case DIRECTORY:
            /* Save current directory to return here */
            if (getcwd(current, PATH_MAX) == NULL) {
                return NULL;
            }
            if (chdir(path) == -1) {
                return NULL;
            }
            if (getcwd(cwd, PATH_MAX) == NULL) {
                perror("getcwd");
                return NULL;
            }
            chdir(current);
            strncpy(resolved_path, cwd, PATH_MAX);

            return resolved_path;
        case REG:
            strncpy(t1, path, PATH_MAX);
            strncpy(t2, path, PATH_MAX);
            dname = dirname(t1);
            fname = basename(t2);
            if (!strncmp(dname, ".", 1)) {
                if (getcwd(current, PATH_MAX) == NULL) {
                    return NULL;
                }
                if (snprintf(resolved_path, PATH_MAX, "%s/%s", current, fname) >= PATH_MAX)
                    return NULL;
            } else {
                if (getcwd(current, PATH_MAX) == NULL) {
                    return NULL;
                }
                if (chdir(dname) == -1) { 
                    return NULL;
                }
                if (getcwd(cwd, PATH_MAX) == NULL) {
                    return NULL;
                }
                chdir (current);
                if (snprintf(resolved_path, PATH_MAX, "%s/%s", cwd, fname) >= PATH_MAX)
                    return NULL;
            }
            return resolved_path;
        case LNK:
            char buf[PATH_MAX];
            if (readlink(path, &buf, PATH_MAX) == -1) {
                return NULL;
            }
            strncpy(t1, buf, PATH_MAX);
            strncpy(t2, buf, PATH_MAX);
            dname = dirname(t1);
            fname = basename(t2);
            if (getcwd(current, PATH_MAX) == NULL) {
                return NULL;
            }
            if (chdir(dname) == -1) {
                return NULL;
            }
            if (getcwd(cwd, PATH_MAX) == NULL) {
                return NULL;
            }
            if (!strncmp(dname, ".", 1))
                snprintf(resolved_path, PATH_MAX, "%s", cwd);
            else
                snprintf(resolved_path, PATH_MAX, "%s/%s", cwd, fname);

            return resolved_path;
        default:
            /* Should not reach here */
            return NULL;
    }
    /* Should not reach here */
    return NULL;     
}

int main(int argc, char **argv)
{
    char buf[PATH_MAX];

    for (int i = 1; i < argc; i++) {
        if (myRealpath(argv[i], buf) == NULL) {
            perror("myRealpath");
            exit(EXIT_FAILURE);
        }
        fprintf(stdout, "%s\n", buf);
    }

    exit(EXIT_SUCCESS);
}
