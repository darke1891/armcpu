#ifndef __LIBS_NEW_TCP_H_
#define __LIBS_NEW_TCP_H_

#define TCP_SYS_SOCKET 0
#define TCP_SYS_BIND 1
#define TCP_SYS_LISTEN 2
#define TCP_SYS_CONNECT 3
#define TCP_SYS_SEND 4
#define TCP_SYS_RECV 5
#define TCP_SYS_CLOSE 6

int socket();
int bind(int sockfd, int* ip, int port);
int connect(int sockfd, int* ip, int port);
int listen(int sockfd);
int send(int sockfd, char* data, int len);
int recv(int sockfd, char* data, int len);


#endif
