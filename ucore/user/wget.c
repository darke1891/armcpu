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
char url[MAX_HTTP_REQUEST_LEN];
int http_msg_len = 0;
int print_info = 0;
#define TCP_BUF_LEN 1010
char tcp_buf[TCP_BUF_LEN];

int call_recv_len = 0;

int tcp_get(int *data, int len) {
//    cprintf("tcp_get len %d\n", len);
    int i=0;
    memset(tcp_buf, 0, TCP_BUF_LEN);
    for (i=0; i<len; i++) {
        tcp_buf[i] = (char)(data[i]);
    }
//    tcp_buf[len] = '\0';
    cprintf("%s", tcp_buf);
//    write(fd1, tcp_buf, len);
    call_recv_len += len;
}

int
main(int argc, char **argv) {
    set_eth_int(0);
//    tcp_send("GET /\r\n\r\n", tcp_get, 100000);
    if (argc != 3 && argc != 2) {
        cprintf("Usage: %s local_dst_file [remote_src_file]\n", argv[0]);
        return 0;
    }
    print_info = (argc==2);
//    fd1 = open(argv[1], O_CREAT|O_RDWR);
//    if (fd1 < 0) {
//        cprintf("open file %s error", argv[1]);
//        return -1;
//    }
//    memset(http_request, 0, MAX_HTTP_REQUEST_LEN);
    char *prefix = "GET /";
    char *suffix = "\r\n\r\n";
    http_msg_len = 0;
    int i=0;
    #define data_push_back(ss) \
        if (ss != 0) {\
            for (i=0; i+http_msg_len < MAX_HTTP_REQUEST_LEN && \
                ss[i] != '\0'; i++) \
                url[http_msg_len+i] = ss[i]; \
            http_msg_len += i; \
        }
    data_push_back(prefix);
    if (argc==3)
        data_push_back(argv[2]);
    data_push_back(suffix);
    url[http_msg_len] = '\0';

    cprintf("msglen: %d, msg: %s\n", http_msg_len, url);

    tcp_send(url, tcp_get, 100000);
    cprintf("call_recv_len: %d\n", call_recv_len);
//    close(fd1);

    return 0;
}
