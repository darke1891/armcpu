#ifndef _KERN_DRIVE_ARP_H_
#define _KERN_DRIVE_ARP_H_

#include <defs.h>
#include <ethernet.h>

#define ETHERNET_TYPE_ARP 0x0806

#define ARP_TYPE 7
#define ARP_TYPE_REQUEST 0x1
#define ARP_TYPE_REPLY 0x2

#define ARP_SENDER_MAC 8
#define ARP_SENDER_IP (ARP_SENDER_MAC+6)
#define ARP_TARGET_MAC (ARP_SENDER_IP+4)
#define ARP_TARGET_IP (ARP_TARGET_MAC+6)

#define ARP_BODY_LEN (ARP_TARGET_IP + 4)

// handle arp
void arp_handle(int *dataHead, int length);

#endif
