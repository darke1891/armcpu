#include <new_tcp.h>
#include <syscall.h>

int socket() {
	return sys_socket();
}

int bind(int sockfd, int *ip, int port) {
	return sys_bind(sockfd, ip, port);
}

int connect(int sockfd, int* ip, int port) {
	return sys_connect(sockfd, ip, port);
}

int listen(int sockfd) {
	return sys_listen(sockfd);
}

int send(int sockfd, char* data, int len) {
	return sys_send(sockfd, data, len);
}

int recv(int sockfd, char* data, int len) {
	return sys_recv(sockfd, data, len);
}

int tcp_close(int sockfd) {
	return sys_tcp_close(sockfd);
}