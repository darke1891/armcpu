#ifndef _KERN_DRIVE_TCP_H_
#define _KERN_DRIVE_TCP_H_

#define TCP_CLOSED 1
#define TCP_SYNC_RECVED 2
#define TCP_LISTEN 3
#define TCP_ESTABLISHED 4
#define TCP_FIN_SENT 5
#define TCP_SYNC_SENT 6
#define TCP_SYNACK_RECVED 7
#define TCP_FIN_RECV 8

#define IP_PROTOCAL_TCP 0x06

#define TCP_SRC_PORT 0
#define TCP_DST_PORT 2
#define TCP_SEQ 4
#define TCP_ACK 8
#define TCP_DATA_OFFSET 12
#define TCP_FLAGS 13
#define TCP_WINDOW 14
#define TCP_CHECKSUM 16
#define TCP_URGEN 18

#define TCP_DATA 20
#define TCP_HDR_LEN 20

#define TCP_FLAG_CWR 0x80
#define TCP_FLAG_ECE 0x40
#define TCP_FLAG_URG 0x20
#define TCP_FLAG_ACK 0x10
#define TCP_FLAG_PSH 0x08
#define TCP_FLAG_RST 0x04
#define TCP_FLAG_SYN 0x02
#define TCP_FLAG_FIN 0x01

#define TCP_SYS_SOCKET 0
#define TCP_SYS_BIND 1
#define TCP_SYS_LISTEN 2
#define TCP_SYS_CONNECT 3
#define TCP_SYS_SEND 4
#define TCP_SYS_RECV 5
#define TCP_SYS_CLOSE 6

void tcp_handle(int *dataHead, int length);
void tcp_send_packet(int flags, int * data, int length);
void tcp_send_queue();

void tcp_handshake(int *src_addr, int *dst_addr);

int tcp_socket();
int tcp_bind(int sockfd, int* ip, int port);
int tcp_connect(int sockfd, int* ip, int port);
int tcp_listen(int sockfd);
int tcp_send(int sockfd, char* data, int len);
int tcp_recv(int sockfd, char* data, int len);
int tcp_close(int sockfd);

#endif
