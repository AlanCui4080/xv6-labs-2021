#include "kernel/types.h"
#include "user/user.h"

#define PIPE_READ (0)
#define PIPE_WRITE (1)

#define TRUE (1)
#define FALSE (0)

#define TERMINATION (0xf00f00ff)

void do_sub(void);
void do_output(int num);

static int current[2];
static int current_next;

int main(int argc, char *argv[]) {
  current_next = FALSE;

  int pipe_rtn = pipe(current);
  if (pipe_rtn) {
    fprintf(2, "Failed to create pipe: %d\n", pipe_rtn);
    close(current[PIPE_WRITE]);
    exit(-1);
  }

  for (int i = 2; i <= 35; i++)
    do_output(i);
  do_output(TERMINATION);
  close(current[PIPE_WRITE]);
  wait(0);
  exit(0);
}
void do_output(int num) {
  write(current[PIPE_WRITE], &num, sizeof(num));

  if (!current_next) {
    int fork_rtn = fork();
    switch (fork_rtn) {
    case -1:
      fprintf(2, "Failed to fork: %d\n", fork_rtn);
      exit(-1);
      break;
    case 0:
      close(current[PIPE_WRITE]);
      current_next = FALSE;
      do_sub();
      break;
    default:
      close(current[PIPE_READ]);
      current_next = TRUE;
      return;
    }
  }
  return;
}
void do_sub(void) {
  int read_fd = 0;
  int write_fd = 0;
  int n1, n2;

  read_fd = current[PIPE_READ];
  int pipe_rtn = pipe(current);
  if (pipe_rtn) {
    fprintf(2, "Failed to create pipe: %d\n", pipe_rtn);
    goto fail;
  }
  write_fd = current[PIPE_WRITE];

  pipe_rtn = read(read_fd, &n1, sizeof(n1));
  if (pipe_rtn < 0) {
    fprintf(2, "Failed to read pipe: %d\n", pipe_rtn);
    goto fail;
  }

  fprintf(1, "prime %d\n", n1);

  while(1) {
    pipe_rtn = read(read_fd, &n2, sizeof(n2));
    if (pipe_rtn < 0) {
      fprintf(2, "Failed to read pipe: %d\n", pipe_rtn);
      goto fail;
    }
    if (n2 == TERMINATION)
      break;
    if (n2 % n1)
      do_output(n2);
  };

  if (current_next)
    do_output(n2);
  wait(0);

fail:
  close(read_fd);
  close(write_fd);
  exit(0);
}
