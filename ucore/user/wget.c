/*
 * $File: wget.c
 * $Author: liangdun
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

int fd1 = -1;
#define MAX_HTTP_REQUEST_LEN (500/4)
char http_request[MAX_HTTP_REQUEST_LEN];
int http_msg_len = 0;

int tcp_get(int *data, int len) {
    //cprintf("get len %d\n", len);
    int i=0;
    for (i=0; i<len; i++) {
        char t = (char)(data[i]);
        cprintf("%c", t);
        write(fd1, &t, 1);
    }
}

int
main(int argc, char **argv) {
//    tcp_send("GET /\r\n\r\n", tcp_get, 100000);
    if (argc != 3 && argc != 2) {
        cprintf("Usage: %s local_dst_file [remote_src_file]\n", argv[0]);
        return 0;
    }
    fd1 = open(argv[1], O_CREAT|O_RDWR);
    if (fd1 < 0) {
        cprintf("open file %s error", argv[1]);
        return -1;
    }
//    memset(http_request, 0, MAX_HTTP_REQUEST_LEN);
    char *prefix = "GET /";
    char *suffix = "\r\n\r\n";
    http_msg_len = 0;
    int i=0;
    #define data_push_back(ss) \
        if (ss != 0) {\
            for (i=0; i+http_msg_len < MAX_HTTP_REQUEST_LEN && \
                ss[i] != '\0'; i++) \
                http_request[http_msg_len+i] = ss[i]; \
            http_msg_len += i; \
        }
    data_push_back(prefix);
    if (argc==3)
        data_push_back(argv[2]);
    data_push_back(suffix);
    http_request[http_msg_len] = '\0';

    cprintf("msglen: %d, msg: %s\n", http_msg_len, http_request);

    tcp_send(http_request, tcp_get, 100000);

    return 0;
}
