#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"
#include "kernel/param.h"

void
find(char *path, char *target, char *cmd[], int ncmd)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if ((fd = open(path, O_RDONLY)) < 0) {
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  if (fstat(fd, &st) < 0) {
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  if (strcmp(path + strlen(path) - strlen(target), target) == 0) {
    char *name = path + strlen(path);
    while (name > path && name[-1] != '/')
      name--;

    if (strcmp(name, target) == 0) {
      if (ncmd == 0) {
        printf("%s\n", path);
      } else {
        char *args[MAXARG];
        int i;

        if (ncmd + 1 >= MAXARG) {
          fprintf(2, "find: too many arguments\n");
          close(fd);
          return;
        }

        for (i = 0; i < ncmd; i++)
          args[i] = cmd[i];

        args[ncmd] = path;
        args[ncmd + 1] = 0;

        int pid = fork();

        if (pid < 0) {
          fprintf(2, "find: fork failed\n");
        } else if (pid == 0) {
          exec(args[0], args);
          fprintf(2, "find: exec %s failed\n", args[0]);
          exit(1);
        } else {
          wait(0);
        }
      }
    }
  }

  if (st.type != T_DIR) {
    close(fd);
    return;
  }

  if (strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf)) {
    printf("find: path too long\n");
    close(fd);
    return;
  }

  strcpy(buf, path);
  p = buf + strlen(buf);
  *p++ = '/';

  while (read(fd, &de, sizeof(de)) == sizeof(de)) {
    if (de.inum == 0)
      continue;

    memmove(p, de.name, DIRSIZ);
    p[DIRSIZ] = 0;

    if (strcmp(de.name, ".") == 0 ||
        strcmp(de.name, "..") == 0)
      continue;

    find(buf, target, cmd, ncmd);
  }

  close(fd);
}

int
main(int argc, char *argv[])
{
  char *cmd[MAXARG];
  int ncmd = 0;

  if (argc < 3) {
    fprintf(2, "Usage: find path name [-exec command [args...]]\n");
    exit(1);
  }

  if (argc > 3) {
    if (strcmp(argv[3], "-exec") != 0 || argc < 5) {
      fprintf(2, "Usage: find path name [-exec command [args...]]\n");
      exit(1);
    }

    ncmd = argc - 4;

    if (ncmd + 1 >= MAXARG) {
      fprintf(2, "find: too many arguments\n");
      exit(1);
    }

    for (int i = 0; i < ncmd; i++)
      cmd[i] = argv[i + 4];
  }

  find(argv[1], argv[2], cmd, ncmd);
  exit(0);
}
