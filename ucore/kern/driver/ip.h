#ifndef _KERN_DRIVE_IP_H_
#define _KERN_DRIVE_IP_H_

#include <ethernet.h>

extern int IP_ADDR[4];
extern int REMOTE_IP_ADDR[4];
#define ETHERNET_TYPE_IP 0x0800

#define IP_VERSION 0
#define IP_VERSION_VAL (0x45) // version and header len

#define IP_TOTAL_LEN 2
#define IP_IDENTIF 4
#define IP_FLAGS 6
#define IP_TTL 8
#define IP_PROTOCAL 9

#define IP_SRC 12
#define IP_DST 16

#define IP_HDR_LEN 20

void ip_handle();
void ip_send(int proto, int length);

#endif
