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

#define PROG_FILE_NAME "two.txt"

int
main(int argc, char **argv) {
  char message[800];
  int message_len = 702;
  int file_name_len;
  char len_buffer[4];
  int ip[4] = {192, 168, 2, 1};
  int i,l;
  int fd1 = -1;
  char temp;

  if (argc < 2){
    cprintf("Need a file name.\n");
    return 0;
  }

  bind(0, ip, 8900);
  listen(0);
  cprintf("return from connect 0\n");
  cprintf("return from open file\n");
  file_name_len = strlen(argv[1]);
  send(0, argv[1], file_name_len);
  recv(0, (char*)(&message_len), 4);
  if (message_len == 0) {
    cprintf("Can't find this file\n");
  }
  else {
    cprintf("Get %s, len : %d\n", argv[1], message_len);
    fd1 = open(argv[1], O_RDWR | O_CREAT);
    while (message_len > 0) {
      if (message_len > 500)
        l = 500;
      else
        l = message_len;
      recv(0, message, l);
      write(fd1, message, l);
      send(0, "OK", 2);
      message_len = message_len - l;
//      cprintf("%d %d\n", message_len, l);
    }
    close(fd1);
  }


  return 0;
}
