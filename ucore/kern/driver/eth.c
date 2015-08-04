/*
 * $File: eth.h
 * $Author: liangdun
 */

#include <eth.h>
#include <wait.h>
#include <intr.h>

static wait_queue_t __wait_queue, *wait_queue = &__wait_queue;

void ethernet_int_handler()
{
    kprintf("ethernet interrupt !!!");
    bool intr_flag;
    local_intr_save(intr_flag);



    local_intr_restore(intr_flag);
}

void wait_ethernet_int()
{
}
