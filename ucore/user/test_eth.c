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
  char *message = "test message";
  int message_len = 12;
  int ip[4] = {192, 168, 2, 2};
  connect(0, ip, 8888);
  send(0, message, message_len);
  return 0;
}