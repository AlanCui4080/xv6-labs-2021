#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(2, "The number of arugment(s) passed is invaild \n"
               "Usage: sleep [ticks] \n");
    exit(-1);
  }
  exit(sleep(atoi(argv[1])));
}