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
  char message[800];
  int message_len = 702;
  int ip[4] = {192, 168, 2, 1};
  int i,j;
  int fd1 = -1;
  char temp;

  bind(0, ip, 8891);
  listen(0);
  cprintf("return from listen\n");
//  connect(0, ip, 8889);
  for (i=0;i<6;i++)
    recv(0, message+i*(message_len/6), message_len/6);
  cprintf("get message\n");
  for (i=0;i<26;i++)
    for (j=0;j<13;j++) {
      temp = message[i*27+j];
      message[i*27+j] = message[i*27+25-j];
      message[i*27+25-j] = temp;
    }
  for (i=0;i<6;i++)
    send(0, message+i*(message_len/6), message_len/6);

  if (argc>1)
    fd1 = open(argv[1], O_RDWR | O_CREAT);
  else
    fd1 = open(PROG_FILE_NAME, O_RDWR | O_CREAT);

  write(fd1, message, message_len);
  close(fd1);
  return 0;
}
