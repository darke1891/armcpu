/*
* @Author: BlahGeek
* @Date:   2014-06-05
* @Last Modified by:   BlahGeek
* @Last Modified time: 2014-06-09
*/
#include <stdio.h>
#include <tcp.h>
#include <ethernet.h>
#include <ip.h>
#include <defs.h>
#include <eth_utils.h>
#include <intr.h>

#define WINDOW_SIZE 1000
#define INIT_SEQ 1001
#define TIMEOUT 30

int tcp_inited = 0;

#define BUF_LENGTH 1000
int MYDATA[BUF_LENGTH];
#define BUF MYDATA

int recv_pos = 0;
int recv_len = 0;
int recv_len_target = 0;
int recv_waiting = 0;

int tcp_src_port, tcp_dst_port;
int tcp_target_port;
int tcp_src_addr[4], tcp_dst_addr[4];
int tcp_ack = 0, tcp_seq = INIT_SEQ;
int tcp_state = TCP_CLOSED;
int tcp_my_seq;

void tcp_handshake(int *src_addr, int *dst_addr) {
  eth_memcpy(ethernet_rx_src, R_MAC_ADDR, 6);
  eth_memcpy(ethernet_rx_data + ETHERNET_HDR_LEN + IP_SRC, REMOTE_IP_ADDR, 4);
  if(tcp_inited == 0)
  {
    tcp_inited = 1;
    tcp_seq = (rand() & 0xfff);
  }
  if (tcp_state != TCP_CLOSED)
    return;
  kprintf("TCP handshake initiated\n");
  tcp_src_port = 50000+(eth_rand() & 0xfff);
  eth_memcpy(tcp_src_addr, src_addr, 4);
  eth_memcpy(tcp_dst_addr, dst_addr, 4);
  // send SYN
  tcp_send_packet(TCP_FLAG_SYN, 0, 0);
  tcp_state = TCP_SYNC_SENT;
}

void tcp_handle(int length) {
//  kprintf("handle tcp\n");
  if(tcp_inited == 0)
  {
    tcp_inited = 1;
  }
  int * data = ethernet_rx_data + ETHERNET_HDR_LEN + IP_HDR_LEN;

  if((data[TCP_FLAGS] & TCP_FLAG_SYN) &&
    (tcp_state == TCP_LISTEN || tcp_state == 0)) {
    tcp_src_port = mem2int(data + TCP_DST_PORT, 2);
    tcp_dst_port = mem2int(data + TCP_SRC_PORT, 2);
    if (tcp_src_port != tcp_target_port)
      return;
//    kprintf("dst_port : %d target_port : %d\n", tcp_src_port, tcp_target_port);
    eth_memcpy(tcp_src_addr, data - IP_HDR_LEN + IP_DST, 4);
    eth_memcpy(tcp_dst_addr, data - IP_HDR_LEN + IP_SRC, 4);
    tcp_ack = mem2int(data + TCP_SEQ, 4) + 1;
    tcp_seq = (rand() & 0xfff);
    tcp_state = TCP_SYNC_RECVED;
//    kprintf("recv seq %d in listening\n", tcp_ack-1);
    tcp_send_packet(TCP_FLAG_SYN | TCP_FLAG_ACK,
            0, 0);
    return;
  }

  // not closed, check port & addr
  if(tcp_src_port != mem2int(data + TCP_DST_PORT, 2)){
//    kprintf("wrong port: %d, %d\n", tcp_src_port, mem2int(data + TCP_DST_PORT, 2));
    return;
  }
  tcp_dst_port == mem2int(data + TCP_SRC_PORT, 2);
  if(tcp_state == TCP_FIN_RECV) {
    tcp_state = TCP_CLOSED;
    kprintf("TCP_CLOSED\n");
  }

  if ((data[TCP_FLAGS] & TCP_FLAG_SYN) &&
  (data[TCP_FLAGS] & TCP_FLAG_ACK) && (tcp_state == TCP_SYNC_SENT)) {
    tcp_seq = mem2int(data+TCP_ACK, 4);
    tcp_ack = mem2int(data+TCP_SEQ, 4) + 1;
    // send out ACK
    tcp_send_packet(TCP_FLAG_ACK, 0, 0);
    tcp_state = TCP_ESTABLISHED;
    kprintf("TCP handshake complete\n");

    wakeup_ethernet();
    return;
  }
  if(data[TCP_FLAGS] & TCP_FLAG_RST) {
    tcp_state = TCP_CLOSED;
    kprintf("TCP_CLOSED\n");
    return;
  }
  if(tcp_state == TCP_FIN_SENT) {
    tcp_seq = mem2int(data + TCP_ACK, 4);
    tcp_send_packet(TCP_FLAG_RST, 0, 0);
    tcp_state = TCP_CLOSED;
    kprintf("TCP_CLOSED\n");
    return;
  }
  if(tcp_state == TCP_SYNC_RECVED &&
    (data[TCP_FLAGS] & TCP_FLAG_ACK)) {
    tcp_seq = mem2int(data + TCP_ACK, 4);
    tcp_my_seq = tcp_seq;
    tcp_ack = mem2int(data + TCP_SEQ, 4) + 1;
    tcp_state = TCP_ESTABLISHED;
    kprintf("TCP_ESTABLISHED\n");
    wakeup_ethernet();
    return;
  }

/*
  if ((tcp_state == TCP_LISTEN) && (data[TCP_FLAGS] & TCP_FLAG_SYN) {
    kprintf("listening\n");
    tcp_seq = (rand() & 0xfff);
    tcp_ack = mem2int(data + TCP_SEQ, 4);
    tcp_send_packet(TCP_FLAGS_SYN | TCP_FLAG_ACK, 0, 0);
  }
*/

  if ((data[TCP_FLAGS] & TCP_FLAG_FIN) && !(data[TCP_FLAGS] & TCP_FLAG_PSH)) {
    tcp_ack +=  1;
    tcp_send_packet(TCP_FLAG_FIN | TCP_FLAG_ACK, 0, 0);
    kprintf("TCP_CLOSED\n");
    tcp_state = TCP_CLOSED;
  }
  if(tcp_state == TCP_ESTABLISHED) {
      int tcphdrlen = 4*(int)(data[TCP_DATA_OFFSET]>>4);
      int datalen = length - tcphdrlen;
      char print_buf[100];
      int *print_data;
      int i;
      // tcp_seq = mem2int(data + TCP_ACK, 4);

//      kprintf("tcp_ack: %d, recv_seq: %d\n", tcp_ack, mem2int(data + TCP_SEQ, 4));
//      kprintf("recv_ack: %d, flags: %d\n", mem2int(data + TCP_ACK, 4), data[TCP_FLAGS]);

//      if (tcp_ack != mem2int(data + TCP_SEQ, 4)) {
        //kprintf("tcp_handle: sequence incorrect\n");
        // out of order pkt, re ACK
//        tcp_send_packet(TCP_FLAG_ACK, 0, 0);
//        return;
//      }
      // in order pkt
//      tcp_ack += datalen;
//      if ((recv_len & 0xffff) == 0)
//      kprintf("recved total length: %d bytes\n", recv_len);
//      kprintf("tcp: datalen: %d, tcphdrlen: %d\n", datalen, tcphdrlen);
//        kprintf("recv_pos: %d datalen : %d\n", recv_pos, datalen);
        if (recv_pos + datalen > BUF_LENGTH) {
        } else
        if (datalen>0) {
          eth_memcpy(BUF+recv_pos,data+tcphdrlen, datalen);
          recv_pos += datalen;
          tcp_ack = mem2int(data + TCP_SEQ, 4) + datalen;
          tcp_seq = tcp_my_seq;
          tcp_send_packet(TCP_FLAG_ACK, 0, 0);
          if (recv_waiting && (recv_pos >= recv_len_target)) {
            wakeup_ethernet();
          }
        }

        if (data[TCP_FLAGS] & TCP_FLAG_FIN) {
          tcp_ack +=  1;
          tcp_send_packet(TCP_FLAG_ACK | TCP_FLAG_FIN, 0, 0);
          tcp_state = TCP_FIN_RECV;
        } else if (data[TCP_FLAGS] & TCP_FLAG_ACK) {
          tcp_my_seq = mem2int(data + TCP_ACK, 4);
        }
      return;
    }
}

// length is the len(data)
void tcp_send_packet(int flags, int * data, int length) {
  int * packet = ethernet_tx_data + ETHERNET_HDR_LEN + IP_HDR_LEN;
  int2mem(packet + TCP_SRC_PORT, 2, tcp_src_port);
  int2mem(packet + TCP_DST_PORT, 2, tcp_dst_port);
  int2mem(packet + TCP_SEQ, 4, tcp_seq);
  int2mem(packet + TCP_ACK, 4, tcp_ack);
  packet[TCP_DATA_OFFSET] = 0x50;
  packet[TCP_FLAGS] = flags;
  packet[TCP_URGEN] = 0;
  packet[TCP_URGEN + 1] = 0;
  packet[TCP_CHECKSUM] = 0;
  packet[TCP_CHECKSUM + 1] = 0;
  int2mem(packet + TCP_WINDOW, 2, WINDOW_SIZE);
  eth_memcpy(packet + TCP_DATA, data, length);
  // calc checksum
  int sum = 0;
  sum += mem2int(tcp_src_addr, 2) + mem2int(tcp_src_addr + 2, 2);
  sum += mem2int(tcp_dst_addr, 2) + mem2int(tcp_dst_addr + 2, 2);
  sum += IP_PROTOCAL_TCP;
  length += TCP_HDR_LEN;
  sum += length;
    int i;
  for(i = 0 ; i < length ; i += 2) {
    int val = (packet[i] << 8);
    if(i + 1 != length) val |= packet[i+1];
    sum += val;
  }
  sum = (sum >> 16) + (sum & 0xffff);
  sum = (sum >> 16) + (sum & 0xffff);
  sum = ~sum;
  packet[TCP_CHECKSUM] = MSB(sum);
  packet[TCP_CHECKSUM + 1] = LSB(sum);
  ip_send(IP_PROTOCAL_TCP, length);
  tcp_my_seq += length;
}

int tcp_socket() {
  return 0;
}

int tcp_bind(int sockfd, int *ip, int port) {
  tcp_target_port = port;
  return 0;
}

int tcp_connect(int sockfd, int *ip, int port) {
  tcp_dst_port = port;
  tcp_handshake(IP_ADDR, REMOTE_IP_ADDR);
  wait_ethernet_int();
  return 0;
}

int tcp_listen(int sockfd) {
  tcp_state = TCP_LISTEN;
  wait_ethernet_int();
  return 0;
}

int tcp_send(int sockfd, char* data, int len) {
  int data_in[1000];
  int i;
  bool intr_flag;
  kprintf("tcp_handle send len: %d\n", len);
  local_intr_save(intr_flag);
  for (i=0;i<len;i++)
    data_in[i] = data[i];
  data_in[len] = '\0';
  tcp_seq = tcp_my_seq;
  tcp_send_packet(TCP_FLAG_PSH|TCP_FLAG_ACK, data_in, len);
  local_intr_restore(intr_flag);
  return 0;
}

int tcp_recv(int sockfd, char* data, int len) {
  int i;
  bool intr_flag;
  kprintf("tcp_handle recving len: %d\n", len);
  local_intr_save(intr_flag);
  if (recv_pos < len) {
    recv_waiting = 1;
    recv_len_target = len;
    local_intr_restore(intr_flag);
    wait_ethernet_int();
    local_intr_save(intr_flag);
    recv_waiting = 0;
  }
    for (i = 0; i < len;i++)
      data[i] = (char)BUF[i];
    data[len] = '\0';
  local_intr_restore(intr_flag);
  return 0;
}