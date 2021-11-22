#define _XOPEN_SOURCE 600
#include <ftw.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <dirent.h>

char *ENTRY = "├── ";
char *LAST = "└── ";
int n_files = 0, n_dirs = 0, prefix = 0;
char *perms[] = {"---", "--x", "-w-", "-wx", "r--", "r-x", "rw-", "rwx"};

static int list(const char *name, const struct stat *status, int type, struct FTW *ftwb)
{
    if (name[ftwb->base] == '.')
        return 0;

    for (int i = prefix; i < strlen(name) - 1; ++i)
    {
        if (name[i] == '/' && name[i + 1] == '.')
        {
            return 0;
        }
    }
    if (type == FTW_NS)
        return 0;

    if (type == FTW_F)
    {
        for (int i = 1; i < ftwb->level; ++i)
        {
            printf("│   ");
        }
        struct passwd *pwd;
        struct group *grp;
        pwd = getpwuid(status->st_uid);
        grp = getgrgid(status->st_gid);
        char a[4];
        sprintf(a, "%3o", status->st_mode & 0777);
        ++n_files;
        printf("%s[-%s%s%s %s %s %*ld]  %s\n", ENTRY, perms[a[0] - '0'], perms[a[1] - '0'], perms[a[2] - '0'], pwd->pw_name, grp->gr_name, 11, status->st_size, name + ftwb->base);
    }

    if (type == FTW_D && strcmp(".", name) != 0 && strcmp("", name) != 0 && strcmp("..", name) != 0)
    {
        for (int i = 1; i < ftwb->level; ++i)
        {
            printf("│   ");
        }
        struct passwd *pwd;
        struct group *grp;
        pwd = getpwuid(status->st_uid);
        grp = getgrgid(status->st_gid);
        char a[4];
        sprintf(a, "%3o", status->st_mode & 0777);
        ++n_dirs;
        printf("%s[d%s%s%s %s %s %*ld]  %s\n", ENTRY, perms[a[0] - '0'], perms[a[1] - '0'], perms[a[2] - '0'], pwd->pw_name, grp->gr_name, 11, status->st_size, name + ftwb->base);
    }

    return 0;
}

int main(int argc, char *argv[])
{

    if (argc == 1)
    {
        nftw(".", list, 1, FTW_CHDIR);
    }
    else
    {
        prefix = strlen(argv[1]);
        nftw(argv[1], list, 1, FTW_CHDIR);
    }
    printf("\n\n%d directories, %d files\n", n_dirs, n_files);
    return 0;
}
