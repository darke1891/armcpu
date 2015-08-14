/*
 * $File: network.h
 * $Author: liangdun
 */


void server(int timeout);
void tcp_send(char *cmd, int (*callback)(int *, int), int timeout);
void set_eth_int(int open);
void set_remote_mac(int r_mac_addr[6]);


extern int tcp_verbose;
