/* 
* @Author: BlahGeek
* @Date:   2014-06-05
* @Last Modified by:   BlahGeek
* @Last Modified time: 2014-06-05
*/


#include "utils.h"
#include "defs.h"

void delay_ms(int ms) {
	int i, j;
    for(i = 0 ; i < ms ; i += 1)
        for(j = 0 ; j < 446 ; j += 1);
}

void delay_us(int us) {
	int i;
    for(i = 0 ; i < us ; i += 1) {
        nop();nop();nop();
        nop();nop();
    }
}


int eth_memcmp(int * a, int * b, int length) {
	int i;
    for(i = 0 ; i < length ; i += 1)
        if(a[i] != b[i])
            return -1;
    return 0;
}

void eth_memcpy(int * dst, int * src, int length) {
	int i;
    for(i = 0 ; i < length ; i += 1)
        dst[i] = LSB(src[i]);
}


int mem2int(int * data, int length) {
    int ret = 0;
	int i;
    for(i = 0 ; i < length ; i += 1) {
        ret <<= 8;
        ret |= LSB(data[i]);
    }
    return ret;
}

void int2mem(int * data, int length, int val) {
	int i;
    for(i = 0 ; i < length ; i += 1) {
        data[length - i - 1] = LSB(val);
        val >>= 8;
    }
}
