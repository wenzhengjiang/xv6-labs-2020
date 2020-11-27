#include "kernel/types.h"
#include "user/user.h"

const uint uint8_mask = (1 << 8) - 1;

uint btoi(uint8 buf[4]) {
  return (uint)((buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3]);
}

void itob(uint i, uint8 buf[4]) {
  buf[0] = (uint8)(i >> 24);
  buf[1] = (uint8)((i >> 16) & uint8_mask);
  buf[2] = (uint8)((i >> 8) & uint8_mask);
  buf[3] = (uint8)(i & uint8_mask);
}

void run_slave(int left_pipe[2]) {
  uint8 buf[4];
  close(left_pipe[1]);
  if (read(left_pipe[0], buf, 4) != 4) {
    return;
  }
  uint prime = btoi(buf);
  printf("prime %d\n", prime);

  int right_pipe[2];
  if (pipe(right_pipe) < 0) {
    fprintf(2, "pipe");
    exit(1);
  }
  if (fork() > 0) {
    // Current slave filters numbers and send them to the right slave.
    close(right_pipe[0]);
    while (read(left_pipe[0], buf, 4) == 4) {
      uint new_number = btoi(buf);
      if (new_number % prime != 0) {
        if (write(right_pipe[1], buf, 4) < 4) {
          fprintf(2, "failed to pass %d to the right slave", new_number);
          exit(1);
        }
      }
    }
    close(left_pipe[0]);
    close(right_pipe[1]);
  } else {
    close(left_pipe[0]);
    // Run the right slave.
    run_slave(right_pipe);
  }
}

void run_master(int right_pipe[2]) {
  uint8 buf[4];
  close(right_pipe[0]);
  for (int i = 2; i <= 35; i++) {
    itob(i, buf);
    if (write(right_pipe[1], buf, 4) < 4) {
      fprintf(2, "failed to distribute %d\n", i);
      exit(1);
    }
  }
  close(right_pipe[1]);
}

int main(int argc, char *argv[]) {
  int p[2];
  if (pipe(p) < 0) {
    fprintf(2, "pipe");
    exit(1);
  }

  if (fork() == 0) {
    run_slave(p);
  } else {
    run_master(p);
  }
  while (wait(0) != -1)
    ;
  exit(0);
}
