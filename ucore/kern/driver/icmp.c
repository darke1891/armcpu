/* 
* @Author: BlahGeek
* @Date:   2014-06-05
* @Last Modified by:   BlahGeek
* @Last Modified time: 2014-06-05
*/

#include <icmp.h>
#include <ip.h>
#include <ethernet.h>
#include <eth_utils.h>
#include <defs.h>

void icmp_handle(int length) {
//    kprintf("handle icmp\n");
    int * data = ethernet_rx_data + ETHERNET_HDR_LEN + IP_HDR_LEN;
    if(data[ICMP_TYPE] != ICMP_TYPE_ECHO_REQUEST)
        return;
    int * buf = ethernet_tx_data + ETHERNET_HDR_LEN + IP_HDR_LEN;
    buf[ICMP_TYPE] = ICMP_TYPE_ECHO_REPLY;
    buf[ICMP_CODE] = 0;
    eth_memcpy(buf + ICMP_ID_SEQ,
           data + ICMP_ID_SEQ,
           length - 4);
    icmp_checksum(buf, length);
    ip_send(IP_PROTOCAL_ICMP, length);
}

void icmp_checksum(int * data, int length) {
    data[ICMP_CHECKSUM] = 0;
    data[ICMP_CHECKSUM + 1] = 0;
    int sum = 0, i;
    for(i = 0 ; i < length ; i += 2) {
        int val = (LSB(data[i]) << 8);
        if(i+1 != length) val |= LSB(data[i+1]);
        sum += val;
    }
    sum = (sum >> 16) + (sum & 0xffff);
    sum = (sum >> 16) + (sum & 0xffff);
    sum = ~sum;
    data[ICMP_CHECKSUM] = MSB(sum);
    data[ICMP_CHECKSUM + 1] = LSB(sum);
}
