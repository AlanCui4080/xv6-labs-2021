#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
  int ping_fd[2];
  int pong_fd[2];
  char drop;

  int pipe_rtn = pipe(ping_fd);
  if (pipe_rtn) {
    fprintf(2, "Failed to create ping_pipe: %d\n", pipe_rtn);
    goto fail;
  }
  pipe_rtn = pipe(pong_fd);
  if (pipe_rtn) {
    fprintf(2, "Failed to create pong_pipe: %d\n", pipe_rtn);
    goto fail;
  }

  write(ping_fd[0], "A", 1);
  close(ping_fd[0]);
  
  int fork_rtn = fork();
  switch (fork_rtn) {
  case -1:
    fprintf(2, "Failed to fork(): %d\n", fork_rtn);
  fail:
    close(ping_fd[1]);
    close(pong_fd[0]);
    close(pong_fd[1]);
    break;
  case 0:
    read(ping_fd[1], &drop, 1);
    fprintf(2, "%d: received pong \n", getpid());
    write(pong_fd[1], "B", 1);

    close(ping_fd[1]);
    close(pong_fd[1]);
    break;
  default:
    read(pong_fd[0], &drop, 1);
    fprintf(2, "%d: received ping \n", getpid());
    close(pong_fd[0]);
    break;
  }
  exit(0);
}
