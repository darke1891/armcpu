#include <stdio.h>
#include <string.h>
#include <ulib.h>
#include "system.h"

int main() {
    cprintf("my pid %d\n", getpid());
    cprintf("test wait ethernet interrupt.....");
    sys_wait_eth_int();
    cprintf("done\n");
    return 0;
}
