/*
 * $File: eth.h
 * $Author: liangdun
 */

#include <eth.h>
#include <wait.h>
#include <intr.h>
#include <dm9000aep.h>
#include <proc.h>

static wait_queue_t __wait_queue, *wait_queue = &__wait_queue;

unsigned int ethernet_read(unsigned int addr) {
    VPTR(ENET_IO_ADDR) = addr;
    return VPTR(ENET_DATA_ADDR);
}

void ethernet_write(unsigned int addr, unsigned int data) {
    VPTR(ENET_IO_ADDR) = addr;
    VPTR(ENET_DATA_ADDR) = data;
}


void ethernet_int_handler()
{
    kprintf("ethernet interrupt !!!");
    bool intr_flag;
    local_intr_save(intr_flag);

    ethernet_write(DM9000_REG_ISR, ISR_PR);
    if (!wait_queue_empty(wait_queue)) {
        wakeup_queue(wait_queue, WT_ETH, 1);
    }

    local_intr_restore(intr_flag);
}

void wait_ethernet_int()
{
    kprintf("wait eth proc : %d\n", current->pid);
    bool intr_flag;
    local_intr_save(intr_flag);
    wait_t __wait, *wait = &__wait;
try_again:
    wait_current_set(wait_queue, wait, WT_ETH);
    local_intr_restore(intr_flag);

    schedule();

    local_intr_save(intr_flag);

    wait_current_del(wait_queue, wait);
    if (wait->wakeup_flags == WT_ETH) {
        goto try_again;
    }
    local_intr_restore(intr_flag);
}
