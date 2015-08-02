#include <stdio.h>
#include "dm9000.h"
#include "readline.h"
#include "ulib.h"

typedef volatile unsigned* mem_ptr_t;

static mem_ptr_t const eth_addr = (mem_ptr_t)0xBFD003E0;
static mem_ptr_t const dig_addr = (mem_ptr_t)0xBFD00400;

void eth_write(int reg, int val) {
    eth_addr[0] = reg;
    sleep(1);
    eth_addr[1] = val;
    sleep(1);
    cprintf(".");
}

int eth_read(int reg) {
    eth_addr[0] = reg;
    //sleep(1);
    return eth_addr[1];
}

void phy_write(int offset, int value) {
    eth_write ( 0x0c, offset | 0x40 );
    eth_write ( 0x0E, (value >> 8) & 0xff );
    eth_write ( 0x0D, value & 0xff );
    eth_write ( 0x0B, 0x08 | 0x02);
    while (eth_read(0x0B)&1) cprintf(",");
    sleep(1);
    eth_write ( 0x0B, 0x08);
}

#define delay sleep(1);

void print_device_info() {
    int mac[6];
    int multi[8];
    int i;
    cprintf("---------- Device info --------------\n");
    for (i=0; i<6; i++)
        mac[i] = eth_read(DM9000_PAR+i);
    cprintf("MAC : ");
    for (i=0; i<6; i++)
        cprintf("%02x ", mac[i]);
    cprintf("\n");

    for (i=0; i<8; i++)
        multi[i] = eth_read(DM9000_MAR+i);

    cprintf("Multicast : ");
    for (i=0; i<8; i++)
        cprintf("%02x ", multi[i]);
    cprintf("\n");

    int mode = eth_read(DM9000_NSR);
    cprintf("NSR : %02x\n", mode);
    cprintf("I/O mode : %d\n", !!(mode & 0x40));
    cprintf("Speed mode : %d\n", !!(mode & 0x80));
    mode = eth_read(DM9000_NCR);
    cprintf("NCR : %02x\n", mode);

    cprintf("Duplex : %d\n", !!(mode & 8));

    mode = eth_read(DM9000_IMR);
    cprintf("IMR : %02x\n", mode);
    cprintf("BPTR : %02x\n", eth_read(DM9000_BPTR));
    cprintf("FCTR : %02x\n", eth_read(DM9000_FCTR));
    cprintf("FCR : %02x\n", eth_read(DM9000_FCR));
    cprintf("SMCR : %02x\n", eth_read(DM9000_SMCR));
    cprintf("GPR : %02x\n", eth_read(DM9000_GPR));
    cprintf("VID : %02x\n", eth_read(0x28));
}

#define eth_intr_on eth_write(DM9000_IMR, 0x81);
#define eth_intr_off eth_write(DM9000_IMR, 0x80);

void send_package(unsigned char *data, int len) {
    eth_intr_off;

    int i;
    for (i=0; i<len; i+=2) {
        eth_write(DM9000_MWCMD, data[i] | (data[i+1]<<8));
    }
    eth_write(DM9000_TCR, 0x01);

    eth_write(DM9000_TXPLH, (len>>8)&0xff);
    eth_write(DM9000_TXPLL, (len)&0xff);

    eth_intr_on;
}

int check_link(int n) {
    int i=0;
    while (i<n) {
        i++;
        int p = eth_read(0x01) & 0x40;
        if (p) {
            cprintf("Link detact\n");
            return 1;
        }
        cprintf("?");
        sleep(1);
    }
    cprintf("no link\n");
    return 0;
}

int phy_read(int offset) {
        eth_write(DM9000_EPAR, offset | 0x40);
        eth_write(DM9000_EPCR, EPCR_EPOS | EPCR_ERPRR);
        while(eth_read(DM9000_EPCR) & EPCR_ERRE);

        eth_write(DM9000_EPCR, EPCR_EPOS);
        sleep(1);
        return (eth_read(DM9000_EPDRH) << 8) |
                eth_read(DM9000_EPDRL);
}

void ethernet_reset() {
    eth_write(DM9000_NCR, NCR_RST);
    while(eth_read(DM9000_NCR) & NCR_RST);
}

void phy_reset() {
    phy_write(DM9000_PHY_BMCR, BMCR_RST);
    while(phy_read(DM9000_PHY_BMCR) & BMCR_RST);
}

int driver_init() {
    cprintf("device init ....... \n");
    // power up
    eth_write(DM9000_GPR, 0x00);
    cprintf("sleep...");
    sleep(10);
    cprintf("wake\n");

    eth_write(DM9000_NCR, 0x03);
    delay;
    eth_write(DM9000_NCR, 0x00);
    delay;

    eth_write(DM9000_NCR, 0x03);
    delay;
    eth_write(DM9000_NCR, 0x00);
    while (eth_read(DM9000_NCR) & 1);
    delay;

    phy_reset();
    // phy_write(0,0x8000);
    check_link(5);
    // auto nego
    phy_write ( 4, 0x01E1 | 0x0400 );
    phy_write ( 0, 0x1200 );
    check_link(5);
    sleep(10);

    // set up mac and multicast addr

    // clean tx & intr status
    // clean tx1, tx2, and wakest
    eth_write(DM9000_NSR, 0x2c);
    // reset interrupt reg
    eth_write(DM9000_ISR, 0x3f);

    eth_write(DM9000_BPTR, 0x3f);
    eth_write(DM9000_FCTR, 0x3a);
    eth_write(DM9000_FCR, 0xff);
    eth_write(DM9000_SMCR, 0x00);

    // reset again
    eth_write(DM9000_NSR, 0x2c);
    eth_write(DM9000_ISR, 0x3f);

    // enable recv interrupt and SRAM read/write
    eth_write(DM9000_IMR, 0x81);
    return 0;
}

int device_io_test() {
    int i,j;
    i=0;
    while (i<10) {
        i++;
        for (j=DM9000_PAR; j<DM9000_PAR+6; j++) {
            eth_write(j, i+j);
        }
        for (j=DM9000_PAR; j<DM9000_PAR+6; j++) {
            int k = eth_read(j);
            if (k != i+j) {
                cprintf("device io test Error\n");
                return 1;
            }
        }
    }
    cprintf("device io test ok\n");
    return 0;
}

void forever_print() {
    while (1) {
        print_device_info();
        sleep(10);
    }
}

int from_hex(char *s) {
    int r=0;
    int i=0;
    while (s[i] != '\0') {
        if (s[i] >= '0' && s[i] <= '9')
            r = (r*16) + (s[i]-'0');
        else
        if (s[i] >= 'a' && s[i] <= 'f')
            r = (r*16) + (s[i]-'a') + 10;
        i++;
    }
    return r;
}

int main(int argc, char **argv) {
    cprintf("argc : %d\n", argc);
    for (int i=0; i<argc; i++)
        cprintf("argv[%d] : %s\n", i, argv[i]);

    if (argc > 1) {
        int reg = from_hex(argv[1]);
        if (argc == 2) {
            cprintf("read reg[0x%02x] = %04x\n", reg, eth_read(reg));
        } else {
            int v = from_hex(argv[2]);
            eth_write(reg, v);
            cprintf("write reg[0x%02x] = %04x\n", reg, v);
        }
        print_device_info();
        return 0;
    }
    //forever_print();
    unsigned char data[] = "asdasdasdasdasd";
    /*
    device_io_test();
    print_device_info();
    driver_init();
    print_device_info();
    device_io_test();
    print_device_info();
    */

    check_link(10);
    send_package(data,10);
    print_device_info();
    check_link(10);
    return 0;


    while (0) {
        char *c = readline(">");
        cprintf("\n");
        dig_addr[0] = c[0];
        if (c[0] == 'q')
            return 0;
    }
    dig_addr[0] = eth_read(0x28);
    return 0;
}
