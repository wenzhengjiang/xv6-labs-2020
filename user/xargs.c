#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"

#define MAXLINE 256

int isspace(char c) { return c == ' ' || c == '\r' || c == '\n'; }

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(2, "usage: xargs command [initial arguments]\n");
    exit(1);
  }

  char *xargv[MAXARG];  // command arg1 arg2 ...
  memcpy(xargv, &argv[1], (argc - 1) * sizeof(argv[1]));

  char line[MAXLINE + 1];
  while (strlen(gets(line, MAXLINE + 1)) > 0) {
    if (strlen(line) > MAXLINE) {
      fprintf(2, "Line length excceeds %d", MAXLINE);
      exit(1);
    }
    uint xargc = argc - 1;
    // Append xargv with args from line.
    int i = 0;
    uint line_len = strlen(line);
    while (i < line_len) {
      if (isspace(line[i])) {
        i++;
        continue;
      }
      // find the start of an argument
      xargv[xargc++] = &line[i];
      // Iterate to the end of the argument
      while (i < line_len && !isspace(line[i])) i++;
      line[i++] = '\0';
    }
    xargv[xargc] = 0;
    if (fork() == 0) {
      exec(xargv[0], xargv);
      exit(1);
    } else {
      wait(0);
    }
  }
  exit(0);
}
