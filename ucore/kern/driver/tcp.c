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

struct tcp_queue_s tcp_queue[TCP_QUEUE_NUM];

void tcp_queue_init(int i) {
    tcp_queue[i].send_pos = 0;
    tcp_queue[i].send_start = 0;
    tcp_queue[i].send_len = 0;
    tcp_queue[i].send_waiting = 0;
    tcp_queue[i].recv_pos = 0;
    tcp_queue[i].recv_start = 0;
    tcp_queue[i].recv_len = 0;
    tcp_queue[i].recv_len_target = 0;
    tcp_queue[i].recv_waiting = 0;
    tcp_queue[i].tcp_ack = 0;
    tcp_queue[i].tcp_seq = INIT_SEQ;
    tcp_queue[i].tcp_state = TCP_CLOSED;
    tcp_queue[i].tcp_dst_port = 0;
    tcp_queue[i].tcp_src_port = 0;
}

void tcp_init() {
  int i = 0;
  for (i=0;i<TCP_QUEUE_NUM;i++) {
    tcp_queue_init(i);
  }
}


void tcp_handshake(int sockfd, int *src_addr, int *dst_addr) {
  if (tcp_queue[sockfd].tcp_state != TCP_CLOSED)
    return;
  tcp_queue[sockfd].tcp_seq = (rand() & 0xfff);
//  kprintf("TCP handshake initiated\n");
//  kprintf("handshake seq : %d\n", tcp_queue[sockfd].tcp_seq);
  tcp_queue[sockfd].tcp_src_port = 50000+(eth_rand() & 0xfff);
  eth_memcpy(tcp_queue[sockfd].tcp_src_addr, src_addr, 4);
  eth_memcpy(tcp_queue[sockfd].tcp_dst_addr, dst_addr, 4);
  // send SYN
  tcp_send_packet(sockfd, TCP_FLAG_SYN, 0, 0);
  tcp_queue[sockfd].tcp_state = TCP_SYNC_SENT;
}

void tcp_handle(int *dataHead, int length) {
//  kprintf("handle tcp\n");
  int *data = dataHead + ETHERNET_HDR_LEN + IP_HDR_LEN;
  int tcphdrlen;
  int sockfd;
  int tcp_listen_port;
  int i;
  int recv_seq;
  int after_len;

  if (length < TCP_HDR_LEN)
    return;
  tcphdrlen = 4*(int)(data[TCP_DATA_OFFSET]>>4);


//  kprintf("recv_seq: %d, recv_ack: %d, flags: %d\n", mem2int(data + TCP_SEQ, 4), mem2int(data + TCP_ACK, 4), data[TCP_FLAGS]);
  for (i=0;i<TCP_QUEUE_NUM;i++)
    if((data[TCP_FLAGS] & TCP_FLAG_SYN) &&
      (tcp_queue[i].tcp_state == TCP_LISTEN)) {
      tcp_listen_port = mem2int(data + TCP_DST_PORT, 2);
      if (tcp_listen_port != tcp_queue[i].tcp_target_port)
        continue;
      sockfd = i;
      tcp_queue[sockfd].tcp_dst_port = mem2int(data + TCP_SRC_PORT, 2);
      tcp_queue[sockfd].tcp_src_port = mem2int(data + TCP_DST_PORT, 2);
  //    kprintf("dst_port : %d target_port : %d\n", tcp_src_port, tcp_target_port);
      eth_memcpy(tcp_queue[sockfd].tcp_src_addr, data - IP_HDR_LEN + IP_DST, 4);
      eth_memcpy(tcp_queue[sockfd].tcp_dst_addr, data - IP_HDR_LEN + IP_SRC, 4);
      tcp_queue[sockfd].tcp_ack = mem2int(data + TCP_SEQ, 4) + 1;
      tcp_queue[sockfd].tcp_remote_seq = tcp_queue[sockfd].tcp_ack;
      tcp_queue[sockfd].tcp_seq = (rand() & 0xfff);
      tcp_queue[sockfd].tcp_state = TCP_SYNC_RECVED;
  //    kprintf("recv seq %d in listening\n", tcp_ack-1);
      tcp_send_packet(sockfd, TCP_FLAG_SYN | TCP_FLAG_ACK, 0, 0);
      return;
    }

  sockfd = -1;
  for (i=0;i<TCP_QUEUE_NUM;i++) {
    if (tcp_queue[i].tcp_src_port == mem2int(data + TCP_DST_PORT, 2))
      sockfd = i;
  }

  if (sockfd == -1) {
//    kprintf("No connection matches\n");
    return;
  }

  // not closed, check port & addr
  if(tcp_queue[sockfd].tcp_dst_port != mem2int(data + TCP_SRC_PORT, 2)){
//    kprintf("wrong port: %d, %d\n", tcp_queue[sockfd].tcp_dst_port, mem2int(data + TCP_SRC_PORT, 2));
    return;
  }
  if (tcp_queue[sockfd].tcp_state == TCP_CLOSED) {
//    kprintf("This connection has been closed\n");
    return;
  }

  if(tcp_queue[sockfd].tcp_state == TCP_FIN_RECV) {
    tcp_queue[sockfd].tcp_state = TCP_CLOSED;
//    kprintf("TCP_CLOSED\n");
    return;
  }

  if ((data[TCP_FLAGS] & TCP_FLAG_SYN) &&
  (data[TCP_FLAGS] & TCP_FLAG_ACK) && (tcp_queue[sockfd].tcp_state == TCP_SYNC_SENT)) {
    tcp_queue[sockfd].tcp_seq = mem2int(data+TCP_ACK, 4);
    tcp_queue[sockfd].tcp_my_seq = tcp_queue[sockfd].tcp_seq;
    tcp_queue[sockfd].tcp_ack = mem2int(data+TCP_SEQ, 4) + 1;
    tcp_queue[sockfd].tcp_remote_seq = tcp_queue[sockfd].tcp_ack;
    // send out ACK
    tcp_send_packet(sockfd, TCP_FLAG_ACK, 0, 0);
    tcp_queue[sockfd].tcp_state = TCP_ESTABLISHED;
//    kprintf("TCP handshake complete\n");

    wakeup_ethernet();
    return;
  }
  if(data[TCP_FLAGS] & TCP_FLAG_RST) {
    tcp_queue[sockfd].tcp_state = TCP_CLOSED;
//    kprintf("TCP_CLOSED\n");
    return;
  }
  if(tcp_queue[sockfd].tcp_state == TCP_FIN_SENT) {
    tcp_queue[sockfd].tcp_seq = mem2int(data + TCP_ACK, 4);
    tcp_send_packet(sockfd, TCP_FLAG_RST, 0, 0);
    tcp_queue[sockfd].tcp_state = TCP_CLOSED;
//    kprintf("TCP_CLOSED\n");
    return;
  }
  if(tcp_queue[sockfd].tcp_state == TCP_SYNC_RECVED &&
    (data[TCP_FLAGS] & TCP_FLAG_ACK)) {
    tcp_queue[sockfd].tcp_seq = mem2int(data + TCP_ACK, 4);
    tcp_queue[sockfd].tcp_my_seq = tcp_queue[sockfd].tcp_seq;
    tcp_queue[sockfd].tcp_ack = mem2int(data + TCP_SEQ, 4);
    tcp_queue[sockfd].tcp_remote_seq = tcp_queue[sockfd].tcp_ack;
    tcp_queue[sockfd].tcp_state = TCP_ESTABLISHED;
//    kprintf("TCP_ESTABLISHED\n");
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
    tcp_queue[sockfd].tcp_ack +=  1;
    tcp_send_packet(sockfd, TCP_FLAG_FIN | TCP_FLAG_ACK, 0, 0);
//    kprintf("TCP_CLOSED\n");
    tcp_queue[sockfd].tcp_state = TCP_CLOSED;
    return;
  }
  if(tcp_queue[sockfd].tcp_state == TCP_ESTABLISHED) {
      int datalen = length - tcphdrlen;
      char print_buf[100];
      int *print_data;
      int i;
      // tcp_seq = mem2int(data + TCP_ACK, 4);

//      kprintf("datalen : %d\n", datalen);

//      if (tcp_ack != mem2int(data + TCP_SEQ, 4)) {
        //kprintf("tcp_handle: sequence incorrect\n");
        // out of order pkt, re ACK
//        tcp_send_packet(TCP_FLAG_ACK, 0, 0);
//        return;
//      }
      // in order pkt
//      tcp_ack += datalen;
//      kprintf("tcp: datalen: %d, tcphdrlen: %d\n", datalen, tcphdrlen);
//        kprintf("Before : Now we have %d bytes\n", tcp_queue[sockfd].recv_len);
        recv_seq = mem2int(data + TCP_SEQ, 4);
        after_len = tcp_queue[sockfd].recv_len + datalen;
//        kprintf("Before seq %d %d %d\n", tcp_queue[sockfd].tcp_remote_seq, recv_seq, after_len);
        if ((datalen>0) && (tcp_queue[sockfd].tcp_remote_seq == recv_seq) && (datalen + tcp_queue[sockfd].recv_len <= buf_length)) {
          for (i=0;i<datalen;i++) {
            tcp_queue[sockfd].recv_buffer[tcp_queue[sockfd].recv_pos] = (char)(data[tcphdrlen + i]);
            tcp_queue[sockfd].recv_pos++;
            tcp_queue[sockfd].recv_len++;
            if (tcp_queue[sockfd].recv_pos >= buf_length)
              tcp_queue[sockfd].recv_pos -= buf_length;
          }
          tcp_queue[sockfd].tcp_ack = recv_seq + datalen;
          tcp_queue[sockfd].tcp_remote_seq = tcp_queue[sockfd].tcp_ack;
          tcp_queue[sockfd].tcp_seq = tcp_queue[sockfd].tcp_my_seq;
//          kprintf("Now we have %d bytes\n", tcp_queue[sockfd].recv_len);
          tcp_send_packet(sockfd, TCP_FLAG_ACK, 0, 0);
          if (tcp_queue[sockfd].recv_waiting && (tcp_queue[sockfd].recv_len >= tcp_queue[sockfd].recv_len_target)) {
            wakeup_ethernet();
          }
        }

        if ((data[TCP_FLAGS] & TCP_FLAG_ACK) && (mem2int(data + TCP_ACK, 4) == tcp_queue[sockfd].tcp_my_seq + tcp_queue[sockfd].send_waiting) && (tcp_queue[sockfd].send_waiting > 0)) {
          tcp_queue[sockfd].tcp_my_seq += tcp_queue[sockfd].send_waiting;
          tcp_queue[sockfd].send_start += tcp_queue[sockfd].send_waiting;
          if (tcp_queue[sockfd].send_start >= buf_length)
            tcp_queue[sockfd].send_start -= buf_length;
          tcp_queue[sockfd].send_len -= tcp_queue[sockfd].send_waiting;
          tcp_queue[sockfd].send_waiting = 0;
          if (tcp_queue[sockfd].send_len > 0)
            tcp_send_queue(sockfd);
        }

        if (data[TCP_FLAGS] & TCP_FLAG_FIN) {
          tcp_queue[sockfd].tcp_ack +=  1;
          tcp_send_packet(sockfd, TCP_FLAG_ACK | TCP_FLAG_FIN, 0, 0);
          tcp_queue[sockfd].tcp_state = TCP_FIN_RECV;
        }
      return;
    }
}

// length is the len(data)
void tcp_send_packet(int sockfd, int flags, int * data, int length) {
  int * packet = ethernet_tx_data + ETHERNET_HDR_LEN + IP_HDR_LEN;
  int2mem(packet + TCP_SRC_PORT, 2, tcp_queue[sockfd].tcp_src_port);
  int2mem(packet + TCP_DST_PORT, 2, tcp_queue[sockfd].tcp_dst_port);
  int2mem(packet + TCP_SEQ, 4, tcp_queue[sockfd].tcp_seq);
  int2mem(packet + TCP_ACK, 4, tcp_queue[sockfd].tcp_ack);
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
  sum += mem2int(tcp_queue[sockfd].tcp_src_addr, 2) + mem2int(tcp_queue[sockfd].tcp_src_addr + 2, 2);
  sum += mem2int(tcp_queue[sockfd].tcp_dst_addr, 2) + mem2int(tcp_queue[sockfd].tcp_dst_addr + 2, 2);
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
}

int tcp_socket() {
  return 0;
}

int tcp_bind(int sockfd, int *ip, int port) {
  tcp_queue[sockfd].tcp_target_port = port;
  REMOTE_IP_ADDR[0] = ip[0];
  REMOTE_IP_ADDR[1] = ip[1];
  REMOTE_IP_ADDR[2] = ip[2];
  REMOTE_IP_ADDR[3] = ip[3];
  return 0;
}

int tcp_connect(int sockfd, int *ip, int port) {
  bool intr_flag;
  REMOTE_IP_ADDR[0] = ip[0];
  REMOTE_IP_ADDR[1] = ip[1];
  REMOTE_IP_ADDR[2] = ip[2];
  REMOTE_IP_ADDR[3] = ip[3];
  local_intr_save(intr_flag);
  tcp_queue_init(sockfd);
  tcp_queue[sockfd].tcp_dst_port = port;
  tcp_handshake(sockfd, IP_ADDR, REMOTE_IP_ADDR);
  local_intr_restore(intr_flag);
  wait_ethernet_int();
  return 0;
}

int tcp_listen(int sockfd) {
  tcp_queue[sockfd].tcp_state = TCP_LISTEN;
  wait_ethernet_int();
  return 0;
}

int tcp_send(int sockfd, char* data, int len) {
  int i;
  bool intr_flag;
  if (tcp_queue[sockfd].send_len + len > buf_length)
    return -1;
//  kprintf("tcp send len: %d\n", len);
  local_intr_save(intr_flag);
  for (i=0;i<len;i++) {
    tcp_queue[sockfd].send_buffer[tcp_queue[sockfd].send_pos] = (int)(data[i]);
    tcp_queue[sockfd].send_pos++;
    if (tcp_queue[sockfd].send_pos >= buf_length)
      tcp_queue[sockfd].send_pos -= buf_length;
    tcp_queue[sockfd].send_len++;
  }
  local_intr_restore(intr_flag);
  if (tcp_queue[sockfd].send_waiting == 0)
    tcp_send_queue(sockfd);
  return 0;
}

void tcp_send_queue(int sockfd) {
  int data_out[buf_length];
  int i;
  bool intr_flag;
  int send_now;
  if (tcp_queue[sockfd].send_len == 0)
    return;
//  kprintf("tcp sending len: %d\n", tcp_queue[sockfd].send_len);
  local_intr_save(intr_flag);
  send_now = tcp_queue[sockfd].send_start;
  for (i=0;i<tcp_queue[sockfd].send_len;i++) {
    data_out[i] = tcp_queue[sockfd].send_buffer[send_now];
    send_now++;
    if (send_now >= buf_length)
      send_now -= buf_length;
  }
  tcp_queue[sockfd].send_waiting = tcp_queue[sockfd].send_len;
  tcp_queue[sockfd].tcp_seq = tcp_queue[sockfd].tcp_my_seq;
  tcp_send_packet(sockfd, TCP_FLAG_PSH|TCP_FLAG_ACK, data_out, tcp_queue[sockfd].send_waiting);
  local_intr_restore(intr_flag);
}

int tcp_recv(int sockfd, char* data, int len) {
  int i;
  bool intr_flag;
//  kprintf("tcp_handle recving len: %d, now len: %d\n", len, tcp_queue[sockfd].recv_len);
  local_intr_save(intr_flag);
  if (len > buf_length)
    len = buf_length;
  if (tcp_queue[sockfd].recv_len < len) {
    tcp_queue[sockfd].recv_waiting = 1;
    tcp_queue[sockfd].recv_len_target = len;
    local_intr_restore(intr_flag);
    wait_ethernet_int();
    local_intr_save(intr_flag);
    tcp_queue[sockfd].recv_waiting = 0;
  }
  for (i = 0; (i < len) && (tcp_queue[sockfd].recv_len > 0);i++) {
    data[i] = tcp_queue[sockfd].recv_buffer[tcp_queue[sockfd].recv_start];
    tcp_queue[sockfd].recv_start++;
    tcp_queue[sockfd].recv_len--;
    if (tcp_queue[sockfd].recv_start >= buf_length)
      tcp_queue[sockfd].recv_start -= buf_length;
  }
  local_intr_restore(intr_flag);
  return 0;
}

int tcp_close(int sockfd) {
  return 0;
}