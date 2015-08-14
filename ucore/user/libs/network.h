/*
 * $File: network.h
 * $Author: liangdun
 */


void server(int timeout);
void tcp_send(char *cmd, int (*callback)(int *, int), int timeout);
void set_eth_int(int open);

extern int tcp_verbose;
