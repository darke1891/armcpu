/*
 * $File: test_eth.c
 * $Author: Hao Sun
 */

#include <network.h>
#include <ulib.h>
#include <stdio.h>
#include <string.h>
#include <dir.h>
#include <file.h>
#include <stat.h>
#include <dirent.h>
#include <unistd.h>
#include <syscall.h>
#include <utils.h>

int
main(int argc, char **argv) {
  char message[702];
  int message_len = 702;
  int ip[4] = {192, 168, 2, 2};
  int i,j;
  char temp;
  bind(0, ip, 8890);
  listen(0);
  cprintf("return from listen\n");
//  connect(0, ip, 8889);
  recv(0, message, message_len);
  cprintf("get message\n");
  for (i=0;i<26;i++)
    for (j=0;j<13;j++) {
      temp = message[i*27+j];
      message[i*27+j] = message[i*27+25-j];
      message[i*27+25-j] = temp;
    }
  send(0, message, message_len);
  return 0;
}