#include "kernel/types.h"
#include "kernel/fs.h"
#include "kernel/stat.h"
#include "user/user.h"

#define NULL (void *)0

void find(char *path, char *target) {
  int fd;
  if ((fd = open(path, 0)) < 0) {
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  struct stat st;
  if (fstat(fd, &st) < 0) {
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch (st.type) {
    case T_FILE:
      break;

    case T_DIR:;
      char buf[512];
      if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf) {
        printf("ls: path too long\n");
        break;
      }
      strcpy(buf, path);
      char *p = buf + strlen(buf);
      *p++ = '/';
      struct dirent de;
      while (read(fd, &de, sizeof(de)) == sizeof(de)) {
        if (de.inum == 0) continue;
        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = 0;
        if (strcmp(target, de.name) == 0) {
          printf("%s\n", buf);
        }
        // Recursively look up child directories.
        if (strcmp(de.name, ".") != 0 && strcmp(de.name, "..") != 0) {
          find(buf, target);
        }
      }
      break;
  }
  close(fd);
}

int main(int argc, char *argv[]) {
  int i;

  if (argc < 2) {
    find(".", NULL);
  } else if (argc < 3) {
    find(argv[1], NULL);
  } else {
    for (i = 2; i < argc; i++) find(argv[1], argv[i]);
  }
  exit(0);
}
