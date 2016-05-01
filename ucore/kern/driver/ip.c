#include <ip.h>
#include <ethernet.h>
#include <icmp.h>
#include <eth_utils.h>
#include <defs.h>
#include <tcp.h>

int IP_ADDR[4] = {192, 168, 2, 2};
int REMOTE_IP_ADDR[4] = {192, 168, 2, 1};

void ip_handle() {
//    kprintf("handle ip\n");
    int * data = ethernet_rx_data + ETHERNET_HDR_LEN;
    // not IPv4 or header is longer than 20bit
    if(data[IP_VERSION] != IP_VERSION_VAL)
        return;

    int length = (data[IP_TOTAL_LEN] << 8) | data[IP_TOTAL_LEN + 1];
    length -= 20; // ip header

    if(data[IP_PROTOCAL] == IP_PROTOCAL_ICMP)
        icmp_handle(length);
    if(data[IP_PROTOCAL] == IP_PROTOCAL_TCP)
        tcp_handle(length);
}

void ip_send(int proto, int length) {
    length += IP_HDR_LEN; // ip header
    ethernet_set_tx(ethernet_rx_src, ETHERNET_TYPE_IP);
    int * data = ethernet_tx_data + ETHERNET_HDR_LEN;
    data[IP_VERSION] = IP_VERSION_VAL;
    data[IP_TOTAL_LEN] = MSB(length);
    data[IP_TOTAL_LEN + 1] = LSB(length);
    data[IP_FLAGS] = 0;
    data[IP_FLAGS + 1] = 0;
    data[IP_TTL] = 64;
    data[IP_PROTOCAL] = proto;
    eth_memcpy(data + IP_SRC, IP_ADDR, 4);
    eth_memcpy(data + IP_DST,
        ethernet_rx_data + ETHERNET_HDR_LEN + IP_SRC, 4);
    ethernet_tx_len = ETHERNET_HDR_LEN + length;
    ethernet_send();
}

