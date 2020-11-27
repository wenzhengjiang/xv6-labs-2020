#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
  // A pipe that transfers bytes from child to parent
  int c2p[2];
  // A pipe that transfers bytes from parent to child 
  int p2c[2];
  static char buf[1];
  if (pipe(c2p) < 0) {
    fprintf(2, "pipe");
    exit(1);
  }
  if (pipe(p2c) < 0) {
    fprintf(2, "pipe");
    exit(1);
  }

  if (fork() == 0)
  {
    close(p2c[1]);
    close(c2p[0]);
    if (read(p2c[0], buf, 1) == 1) {
      fprintf(1, "%d: received ping\n", getpid());
    } else {
      fprintf(2, "%d: Failed to read from pipe\n", getpid());
      exit(1);
    }
    if (write(c2p[1], buf, 1) != 1) {
      fprintf(2, "%d: Failed to write to pipe\n", getpid());
      exit(1);
    }
  } else {
    close(p2c[0]);
    close(c2p[1]);
    buf[0] = ' ';
    if (write(p2c[1], buf, 1) != 1) {
      fprintf(2, "%d: Failed to write to pipe\n", getpid());
      exit(1);
    }
    if (read(c2p[0], buf, 1) == 1) {
      fprintf(1, "%d: received pong\n", getpid());
    } else {
      fprintf(2, "%d: Failed to read from pipe\n", getpid());
      exit(1);
    }
    wait(0);
  }

  exit(0);
}
