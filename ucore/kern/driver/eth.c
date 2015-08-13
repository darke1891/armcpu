/*
 * $File: eth.h
 * $Author: liangdun
 */

#include <eth.h>
#include <wait.h>
#include <intr.h>
#include <dm9000aep.h>
#include <proc.h>

#define nop() __asm volatile ("nop")
#define LSB(x) ((x) & 0xFF)
#define MSB(x) (((x) >> 8) & 0xFF)

static wait_queue_t __wait_queue, *wait_queue = &__wait_queue;

unsigned int ethernet_read(unsigned int addr) {
    VPTR(ENET_IO_ADDR) = addr;
    return VPTR(ENET_DATA_ADDR);
}

void ethernet_write(unsigned int addr, unsigned int data) {
    VPTR(ENET_IO_ADDR) = addr;
    VPTR(ENET_DATA_ADDR) = data;
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

void ethernet_int_handler()
{
    bool intr_flag;
    local_intr_save(intr_flag);

    ethernet_write(DM9000_REG_ISR, ISR_PR);
    if (!wait_queue_empty(wait_queue)) {
        wakeup_queue(wait_queue, WT_ETH, 1);
    }

    local_intr_restore(intr_flag);
    //current->need_resched = 1;
}

void wait_ethernet_int()
{
    bool intr_flag;
    local_intr_save(intr_flag);
    wait_t __wait, *wait = &__wait;
try_again:
    wait_current_set(wait_queue, wait, WT_ETH);
    local_intr_restore(intr_flag);

    schedule();

    local_intr_save(intr_flag);

    wait_current_del(wait_queue, wait);
    if (wait->wakeup_flags != WT_ETH) {
        goto try_again;
    }
    local_intr_restore(intr_flag);
}

static int MAC_ADDR[6] = {0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};

void delay_ms(int ms) {
    int i, j;
    for(i = 0 ; i < ms ; i += 1) {
        for(j = 0 ; j < 1000 ; j += 1) nop();
      }
}

void delay_us(int us) {
    int i;
    for(i = 0 ; i < us ; i += 1) {
        nop();nop();nop();
        nop();nop();

        nop();nop();nop();
        nop();nop();

        nop();nop();nop();
    }
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
    wait_queue_init(wait_queue);

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
        kprintf(".");
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
        kprintf("Done\n");
        //!pic_enable(IRQ_ETH);
}
