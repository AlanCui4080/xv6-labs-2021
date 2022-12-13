#include "kernel/param.h"
#include "kernel/types.h"
#include "user/user.h"

#define MAX_ARG_LEN 128

#define stderr 2

int static lstrcpy(char *dst, const char *src) {
  for (unsigned long i = 0; i < strlen(src) + 1; i++)
    dst[i] = src[i];
  return 0;
}
int main(int argc, char *argv[]) {
  static char __buf_exec_argv[MAXARG * MAX_ARG_LEN];
  static char *exec_argv[MAXARG];
  for (unsigned long i = 0; i < MAXARG; i++){
    exec_argv[i] = __buf_exec_argv + i*MAX_ARG_LEN;
  }
    // exec_argv[i] = malloc(128);

  if (argc < 2 || argc > MAXARG) {
    fprintf(2, "The number of arugment(s) passed is invaild \n"
               "Usage: xargs command \n");
    exit(-1);
  }

  int remain_argc = MAXARG - 1;
  unsigned long i = 0;
  for (i = 0; i < (argc - 1); i++) {
    lstrcpy(exec_argv[i], argv[i + 1]);
  }
  for (; i < remain_argc - 1; i++) {
    char * gets_rtn = gets(exec_argv[i], MAX_ARG_LEN-1);
    unsigned long len = strlen(gets_rtn);
    gets_rtn[len - 1] = '\0';
    if(!len)
      break;
  }
  exec_argv[i] = (void*) 0;
  // for(unsigned long q = 0; q < MAXARG; q++){
  //   fprintf(2, "argv[%d] =%s#\n", q, exec_argv[q]);
  // }
  int fork_rtn = fork();
  switch (fork_rtn) {
  case -1:
    fprintf(stderr, "Failed to fork(): %d\n", fork_rtn);
    break;
  case 0:
    int sub_rtn;
    wait(&sub_rtn);
    break;
  default:
    int rtn_exec = exec(argv[1], exec_argv);
    fprintf(stderr, "Failed to exec() %d\n", rtn_exec);
    exit(-1);
  }
  exit(0);
}