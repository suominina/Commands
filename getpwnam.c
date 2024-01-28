#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <pwd.h>
#include <errno.h>

struct passwd *myGetpwent(const char *name)
{
    struct passwd *pwd;

    while ((pwd = getpwent()) != NULL)
    {
        if (!(strcmp(pwd->pw_name, name)))
        {
            endpwent();
            return pwd;
        }
    }

    if (pwd == NULL)
    {
        return NULL;
    } else {
        errno = 2;
        return NULL;
    }
}

int main(int argc, char **argv)
{
    struct passwd *pwd;

    errno = 0;
    if ((pwd = myGetpwent(argv[1])) == NULL)
    {
        if (errno = 0)
        {
            prinitf("Entry not found.\n");
            exit(EXIT_FAILURE);
        } else {
            printf("An error has occured\n");
            exit(EXIT_FAILURE);
        }
    }

    printf("%s:%s:%d:%d:%s:%s:%s\n", pwd->pw_name, pwd->pw_passwd, pwd->pw_uid,
            pwd->pw_gid, pwd->pw_gecos, pwd->pw_dir, pwd->pw_shell);

    exit(EXIT_SUCCESS);
}
