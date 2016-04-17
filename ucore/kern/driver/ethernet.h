/*
 * $File: eth.h
 * $Author: liangdun, Hao Sun
 * $Discription:
 * Now I handle everything under TCP in kernel
 */
#ifndef _KERN_DRIVER_ETHERNET_H_
#define _KERN_DRIVER_ETHERNET_H_

#include <defs.h>
#include <thumips.h>
#include <stdio.h>
#include <string.h>
#include <picirq.h>
#include <trap.h>
#include <wait.h>

extern int MAC_ADDR[6];
extern int R_MAC_ADDR[6];
extern int ethernet_rx_data[2048];
extern int ethernet_rx_len;
extern int ethernet_tx_data[2048];
extern int ethernet_tx_len;

#define ETHERNET_ISR (VPTRENET_INT_ADDR)

#define ETHERNET_DST_MAC 0
#define ETHERNET_SRC_MAC 6
#define ETHERNET_HDR_LEN 14

#define ethernet_rx_type ((ethernet_rx_data[12] << 8) | ethernet_rx_data[13])
#define ethernet_rx_src (ethernet_rx_data + ETHERNET_SRC_MAC)
#define ethernet_rx_dst (ethernet_rx_data + ETHERNET_DST_MAC)


void ethernet_int_handler();
void wait_ethernet_int();
void wakeup_ethernet();

unsigned int ethernet_read(unsigned int addr);
void ethernet_write(unsigned int addr, unsigned int data);

void ethernet_phy_write(int offset, int value);
int ethernet_phy_read(int offset);

void ethernet_powerup();
void ethernet_reset();
void ethernet_phy_reset();
void ethernet_init();

int ethernet_check_iomode(); // return 8 or 16bit mode
int ethernet_check_link(); // return 1 if link ok
int ethernet_check_speed(); // return 10 or 100 Mbps
int ethernet_check_duplex(); // return 1 if full duplex

void ethernet_send();
void ethernet_recv();

void ethernet_set_tx(int * dst, int type);
int get_tx_addr();
void set_tx_addr(int addr);
void set_eth_int(int open);

#endif