#include "kernel/types.h"
#include "kernel/fs.h"
#include "kernel/stat.h"
#include "user/user.h"

void find(const char *dirname, const char *filename);
#define strcat(dst, src) strcpy((dst) + strlen(dst), (src))

int main(int argc, char *argv[]) {
  if (argc != 3) {
    fprintf(2, "The number of arugment(s) passed is invaild \n"
               "Usage: find [path] filename \n");
    exit(-1);
  }
  find(argv[1], argv[2]);
  exit(0);
}
void find(const char *dirname, const char *filename) {
  if (dirname == (void *)0) {
    fprintf(2, "NULL name\n");
    goto fail;
  }
  int dir_fd = open(dirname, 0);
  if (dir_fd < 0) {
    fprintf(2, "Failed to open %s: %d\n", dirname, dir_fd);
    return;
  }
  struct stat sdata;
  int rtn = fstat(dir_fd, &sdata);
  if (rtn) {
    fprintf(2, "Failed to stat %s: %d\n", dirname, rtn);
    goto fail;
  }
  if (sdata.type != T_DIR) {
    fprintf(2, "%s is not a DIR\n", dirname);
    goto fail;
  }
  uint64 ssize = 0;
  while (ssize < sdata.size) {
    struct dirent this_dirdata;
    rtn = read(dir_fd, &this_dirdata, sizeof(struct dirent));
    if (rtn < 0) {
      fprintf(2, "Failed to read %s: %d\n", dirname, rtn);
      goto fail;
    }
    if (!strlen(this_dirdata.name))
      goto next;
    if (!strcmp(this_dirdata.name, ".") || !strcmp(this_dirdata.name, ".."))
      goto next;
    struct stat this_sdata;
    if (strlen(this_dirdata.name) + 1 + strlen(dirname) + 1 > 128) {
      fprintf(2, "Path too long\n");
      exit(0);
    }
    if (this_dirdata.name == (void *)0) {
      fprintf(2, "NULL entry name\n");
      exit(0);
    }
    char this_name[128] = {0};
    strcpy(this_name, dirname);
    strcat(this_name, "/");
    strcat(this_name, this_dirdata.name);
    int this_fd = open(this_name, 0);
    if (this_fd < 0) {
      fprintf(2, "Failed to open %s: %d\n", this_name, this_fd);
      goto fail;
    }
    rtn = fstat(this_fd, &this_sdata);
    if (rtn) {
      fprintf(2, "Failed to stat %s: %d\n", this_name, rtn);
      goto fail;
    }
    if (!strcmp(this_dirdata.name, filename)) {
      fprintf(1, "%s\n", this_name);
      goto next;
    }
    if (this_sdata.type == T_DIR)
      find(this_name, filename);
  next:
    close(this_fd);
    ssize += sizeof(this_dirdata);
  }
fail:
  close(dir_fd);
  return;
}