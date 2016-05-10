#include <ulib.h>
#include <stdio.h>
#include <string.h>
#include <dir.h>
#include <file.h>
#include <stat.h>
#include <dirent.h>
#include <unistd.h>
#include <utils.h>
#include <new_tcp.h>

#define PROG_FILE_NAME "test"

int
main(int argc, char **argv) {
  char *message = "bluemoon\n";
  int fd1;
  int message_len = 9;
  if (argc>1)
    fd1 = open(argv[1], O_RDWR | O_CREAT);
  else
    fd1 = open(PROG_FILE_NAME, O_RDWR | O_CREAT);

  if (fd1 >= 0) {
    write(fd1, message, message_len);
    close(fd1);
  }
  else
    cprintf("get file descriptor %d\n", fd1);
  return 0;
}
