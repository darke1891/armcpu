/*
 * $File: eth.h
 * $Author: liangdun, Hao Sun
 */

#include <ethernet.h>
#include <wait.h>
#include <intr.h>
#include <dm9000aep.h>
#include <proc.h>
#include <eth_utils.h>
#include <arp.h>
#include <ip.h>

int MAC_ADDR[6] = {0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};
// hard code remote mac addr when first send
int R_MAC_ADDR[6] = {0xf0,0xde,0xf1,0x72,0x19,0x2e};
int ethernet_rx_data[2048];
int ethernet_tx_data[2048];
int ethernet_tx_len;

wait_queue_t __eth_wait_queue;
wait_queue_t *eth_wait_queue = &__eth_wait_queue;

unsigned int ethernet_read(unsigned int addr) {
    VPTR(ENET_IO_ADDR) = addr;
    nop();nop();
    return VPTR(ENET_DATA_ADDR);
}

void ethernet_write(unsigned int addr, unsigned int data) {
    VPTR(ENET_IO_ADDR) = addr;
    nop();nop();
    VPTR(ENET_DATA_ADDR) = data;
    nop();nop();
}

void ethernet_int_handler()
{
    int *dataHead;
    int dataLength;
    ethernet_recv();
    dataLength = ethernet_rx_data[0];
    dataHead = ethernet_rx_data;
    while (dataLength >= 0) {
        dataHead++;
        if (dataLength >= 14) {
            int type = ((dataHead[12] << 8) | dataHead[13]);
            if(type == ETHERNET_TYPE_ARP) {
                arp_handle(dataHead, dataLength);
            } else
            if(type == ETHERNET_TYPE_IP) {
                ip_handle(dataHead, dataLength);
            } else
                kprintf("Unknow package type %d\n", type);
        }
        dataHead += (dataLength / 2) * 2;
        dataLength = dataHead[0];
    }
}

void wait_ethernet_int()
{
    bool intr_flag;
    local_intr_save(intr_flag);
    wait_t __wait, *wait = &__wait;
try_again:
    wait_current_set(eth_wait_queue, wait, WT_ETH);
    local_intr_restore(intr_flag);

    schedule();

    local_intr_save(intr_flag);

    wait_current_del(eth_wait_queue, wait);
    if (wait->wakeup_flags != WT_ETH) {
        goto try_again;
    }
    local_intr_restore(intr_flag);
}

void wakeup_ethernet() {
    bool intr_flag;
    local_intr_save(intr_flag);
    wakeup_first(eth_wait_queue, WT_ETH, 1);
    local_intr_restore(intr_flag);
}

void ethernet_reset() {
    ethernet_write(DM9000_REG_NCR, NCR_RST);
    int r;
    while((r=ethernet_read(DM9000_REG_NCR)) & NCR_RST) {
        //cprintf("NCR : %02x\n", r);
    };
}

void ethernet_phy_reset() {
    ethernet_phy_write(DM9000_PHY_REG_BMCR, BMCR_RST);
    while(ethernet_phy_read(DM9000_PHY_REG_BMCR) & BMCR_RST);
}

void ethernet_phy_write(int offset, int value) {
    ethernet_write(DM9000_REG_EPAR, offset | 0x40);
    ethernet_write(DM9000_REG_EPDRH, MSB(value));
    ethernet_write(DM9000_REG_EPDRL, LSB(value));

    ethernet_write(DM9000_REG_EPCR, EPCR_EPOS | EPCR_ERPRW);
    while(ethernet_read(DM9000_REG_EPCR) & EPCR_ERRE);
    delay_us(5);
    ethernet_write(DM9000_REG_EPCR, EPCR_EPOS);
}

int ethernet_phy_read(int offset) {
    ethernet_write(DM9000_REG_EPAR, offset | 0x40);
    ethernet_write(DM9000_REG_EPCR, EPCR_EPOS | EPCR_ERPRR);
    while(ethernet_read(DM9000_REG_EPCR) & EPCR_ERRE);

    ethernet_write(DM9000_REG_EPCR, EPCR_EPOS);
    delay_us(5);
    return (ethernet_read(DM9000_REG_EPDRH) << 8) |
            ethernet_read(DM9000_REG_EPDRL);
}

void ethernet_powerup() {
    ethernet_write(DM9000_REG_GPR, 0x00);
    delay_ms(100);
}

void ethernet_init() {
    wait_queue_init(eth_wait_queue);

        kprintf("Network initializing...\n");
        ethernet_powerup();
        kprintf("Power up done.\n");
        ethernet_reset();
        kprintf("reset done.\n");
        ethernet_phy_reset();
        kprintf("phy reset done.\n");

        // auto nego
        ethernet_phy_write ( 4, 0x01E1 | 0x0400 );
        ethernet_phy_write ( 0, 0x1200 );

        // 10M duplex mode
        //ethernet_phy_write ( 4, 0x0041 | 0x0400 );
        //ethernet_phy_write ( 0, 0x0100 );

        // 10M duplex mode
        //ethernet_phy_write ( 4, 0x0021 );
        //ethernet_phy_write ( 0, 0x0000 );

        int i;
        // set MAC address
        for(i = 0 ; i < 6 ; i += 1)
            ethernet_write(DM9000_REG_PAR0 + i, MAC_ADDR[i]);
        // initialize hash table
        for(i = 0 ; i < 8 ; i += 1)
            ethernet_write(DM9000_REG_MAR0 + i, 0x00);
        // accept broadcast
        ethernet_write(DM9000_REG_MAR7, 0x80);
        // enable pointer auto return function
        ethernet_write(DM9000_REG_IMR, IMR_PAR);
        // clear NSR status
        ethernet_write(DM9000_REG_NSR, NSR_WAKEST | NSR_TX2END | NSR_TX1END);
        // clear interrupt flag
        ethernet_write(DM9000_REG_ISR,
            ISR_UDRUN | ISR_ROO | ISR_ROS | ISR_PT | ISR_PR);
        // enable interrupt (recv only)
        ethernet_write(DM9000_REG_IMR, IMR_PAR | IMR_PRI);
        // enable reciever
        ethernet_write(DM9000_REG_RCR,
            RCR_DIS_LONG | RCR_DIS_CRC | RCR_RXEN);
        // enable checksum calc
        ethernet_write(DM9000_REG_TCSCR,
            TCSCR_IPCSE);
        kprintf("Network initializing Done\n");
        pic_enable(ETH_IRQ);
}

int ethernet_check_iomode() {
    int val = ethernet_read(DM9000_REG_ISR) & ISR_IOMODE;
    if(val) return 8;
    return 16;
}
int ethernet_check_link() {
    return (ethernet_read(0x01) & 0x40) >> 6;
}
int ethernet_check_speed() {
    int val = ethernet_read(0x01) & 0x80;
    if(val == 0) return 100;
    return 10;
}
int ethernet_check_duplex() {
    return (ethernet_read(0x00) & 0x08) >> 3;
}

void ethernet_send() {
    // int is char
    // A dummy write
    ethernet_write(DM9000_REG_MWCMDX, 0);
    // select reg
    VPTR(ENET_IO_ADDR) = DM9000_REG_MWCMD;
    nop(); nop();
    int i;
    for(i = 0 ; i < ethernet_tx_len ; i += 2){
        int val = ethernet_tx_data[i];
        if(i + 1 != ethernet_tx_len) val |= (ethernet_tx_data[i+1] << 8);
        VPTR(ENET_DATA_ADDR) = val;
        nop();
    }
    // write length
    ethernet_write(DM9000_REG_TXPLH, MSB(ethernet_tx_len));
    ethernet_write(DM9000_REG_TXPLL, LSB(ethernet_tx_len));
    // clear interrupt flag
    ethernet_write(DM9000_REG_ISR, ISR_PT);
    // transfer data
    ethernet_write(DM9000_REG_TCR, TCR_TXREQ);
}

void ethernet_recv() {
    int start = 0;
    int ethernet_rx_len = 0;
    int i;
    while (ethernet_rx_len >= 0) {
        // a dummy read
        ethernet_read(DM9000_REG_MRCMDX);
        // select reg
        VPTR(ENET_IO_ADDR) = DM9000_REG_MRCMDX1;
        nop(); nop();
        int status = LSB(VPTR(ENET_DATA_ADDR));
        if(status != 0x01){
            ethernet_rx_len = -1;
            ethernet_rx_data[start] = ethernet_rx_len;
            break;
        }
        VPTR(ENET_IO_ADDR) = DM9000_REG_MRCMD;
        nop(); nop();
        status = MSB(VPTR(ENET_DATA_ADDR));
        nop(); nop();
        ethernet_rx_len = VPTR(ENET_DATA_ADDR);
        nop(); nop();
        if(status & (RSR_LCS | RSR_RWTO | RSR_PLE |
                     RSR_AE | RSR_CE | RSR_FOE)) {
            ethernet_rx_len = -1;
        }
        ethernet_rx_data[start] = ethernet_rx_len;
        start++;
        for(i = 0 ; i < ethernet_rx_len ; i += 2) {
            int data = VPTR(ENET_DATA_ADDR);
            ethernet_rx_data[start+i] = LSB(data);
            ethernet_rx_data[start+i+1] = MSB(data);
        }
        start += ethernet_rx_len;
    }
    // clear intrrupt
    ethernet_write(DM9000_REG_ISR, ISR_PR);
}

void ethernet_set_tx(int type) {
    eth_memcpy(ethernet_tx_data + ETHERNET_DST_MAC, R_MAC_ADDR, 6);
    eth_memcpy(ethernet_tx_data + ETHERNET_SRC_MAC, MAC_ADDR, 6);
    ethernet_tx_data[12] = MSB(type);
    ethernet_tx_data[13] = LSB(type);
}

int get_tx_addr() {
    return ethernet_read(DM9000_REG_TRPAL) | (ethernet_read(DM9000_REG_TRPAH)<<8);
}

void set_tx_addr(int addr) {
    ethernet_write(DM9000_REG_TRPAL, addr & 0xff);
    ethernet_write(DM9000_REG_TRPAH, (addr>>8) & 0xff);
}

void set_eth_int(int open) {
    if (open)
        ethernet_write(DM9000_REG_IMR, IMR_PAR | IMR_PRI);
    else
        ethernet_write(DM9000_REG_IMR, IMR_PAR);
}