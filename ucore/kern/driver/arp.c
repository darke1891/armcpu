/*
* @Author: BlahGeek
* @Date:   2014-06-05
* @Last Modified by:   BlahGeek
* @Last Modified time: 2014-06-05
*/

#include <arp.h>
#include <eth_utils.h>
#include <ip.h>

int ARP_FIX_HDR[] = {
    0x00, 0x01, // ethernet
    0x08, 0x00, // IP
    0x06, 0x04, // mac/IP size
    0x00,       // high bit of type (hack)
};

void arp_handle(int *dataHead, int length) {
//    kprintf("handle arp\n");
    int * data = dataHead + ETHERNET_HDR_LEN;
    if(data[ARP_TYPE] == ARP_TYPE_REQUEST) {
        if(eth_memcmp(data + ARP_TARGET_IP, IP_ADDR, 4) != 0)
            return;
        ethernet_tx_len = ETHERNET_HDR_LEN + ARP_BODY_LEN;
        ethernet_set_tx(ETHERNET_TYPE_ARP);

        int * buf = ethernet_tx_data + ETHERNET_HDR_LEN;
        eth_memcpy(buf, ARP_FIX_HDR, 6 + 1);
        buf[ARP_TYPE] = ARP_TYPE_REPLY;
        eth_memcpy(buf + ARP_SENDER_MAC, MAC_ADDR, 6);
        eth_memcpy(buf + ARP_SENDER_IP, IP_ADDR, 4);
        eth_memcpy(buf + ARP_TARGET_MAC,
               data + ARP_SENDER_MAC, 6);
        eth_memcpy(buf + ARP_TARGET_IP,
               data + ARP_SENDER_IP, 4);
        ethernet_send();
    }
}
