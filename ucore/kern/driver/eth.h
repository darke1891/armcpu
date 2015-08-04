/*
 * $File: eth.h
 * $Author: liangdun
 * $Discription:
 *      This ethernet driver only handle interrupt,
 *      let user prog to do the driver and other stuff.
 */

#include <defs.h>
#include <thumips.h>
#include <stdio.h>
#include <string.h>
#include <picirq.h>
#include <trap.h>

void ethernet_int_handler();
void wait_ethernet_int();
