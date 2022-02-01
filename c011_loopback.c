/*
  Test C011 (mode2) loopback. Loop LinkIn-LinkOut on C011
 */

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <stdbool.h>
#include <time.h>
#include "c011.h"

int main(int argc, char *argv[])
{
    int ret;
    // fails before 1 million iterations
    // F7   11111110 in 80255
    // C0   11000000 in 1136
    // A0   10100000 in 4
    // 90   10010000 in 37445
    // 8A   10001010 in 42501
    // 88   10001000 in 2122
    // 84   10000100 in 1076
    // 81   10000001 in 462K
    // 80   10000000 in 1481
    // 60   01100000 in 17
    // 50   01010000 in 1
    // 40   01000000 in 0
    // 30   00110000 in 2
    // 20   00100000 in 2
    // 10   00010000 in 28
    // 08   00001000 in 596
    // 07   00000111 in 97175
    // 06   00000100 in 1285
    // 05   00000101 in 347K
    // 04   00000100 in 1
    // 03   00000011 in 26
    // 02   00000010 in 220
    // 01   00000001 in 2
    // 00   00000000 in 1 iteration
    uint8_t i=0;
    c011_init();
    c011_reset();
    uint8_t read;
    int count=0;
    bool good=true;
    while (true) {
        c011_reset();
        good = true;
        for (count=0; count < 1000000 && good; count++) {
            //printf ("W 0x%X ", i);
            ret = c011_write_byte(i,200);
            if (ret == -1) {
                printf ("write timeout\n");
                good = false;
                break;
            }
            ret = c011_read_byte(&read, 200);
            if (ret == -1) {
                printf ("read timeout\n");
                good = false;
                break;
            } else {        
                //printf ("R 0x%X ", read);
                if (read != i) {
                    //printf ("*E* write=0x%X read=0x%X\n",i,read);
                    good = false;
                    break;
                } else {
                    //printf ("OK %d\n", count++);
                }
            }
        }
        if (good) {
            printf ("0x%X OK\n", i);
        } else {
            printf ("0x%X BAD after %d iterations\n",i,count);
        }

        i++;
        //if (i==0x18) i=0x38; else i=0x18;
        //i = ~i;
    }
    return 0;
}
