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

int test_all_byte_values(void) {
    int ret;
    // POWER IS CRITICAL! without good 3V3 and 5V feeds to the TXS0108 parts you get random power drops for certain byte values...  
    // fails before 1 million iteration. (T)=timeout, else wrong val read
    // F7   11110111 in 80255
    // D0   11010000 in 44
    // C0   11000000 in 1136    (T)
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
    // 30   00110000 in 2       (T)
    // 20   00100000 in 2
    // 10   00010000 in 28
    // 08   00001000 in 5
    // 07   00000111 in 17513   (T)
    // 06   00000100 in 87381
    // 05   00000101 in 239     (T)
    // 04   00000100 in 34219
    // 03   00000011 in 26      (T)
    // 02   00000010 in 220
    // 01   00000001 in 2
    // 00   00000000 in 0 iteration
    uint8_t i=0;
    c011_init();
    c011_reset();
    uint8_t read;
    int count=0;
    bool good=true;
    bool timeout = false;
    while (true) {
        c011_reset();
        good = true;
        timeout = false;
        for (count=0; count < 1000000 && good && !timeout; count++) {
            ret = c011_write_byte(i,200);
            if (ret == -1) {
                printf ("write timeout\n");
                timeout = true;
                break;
            }
            ret = c011_read_byte(&read, 200);
            if (ret == -1) {
                printf ("read timeout\n");
                timeout = true;
                break;
            } else {        
                if (read != i) {
                    good = false;
                    break;
                } else {
                }
            }
        }
        if (timeout) {
            printf ("0x%X T after %d iterations\n",i,count);
        } else if (!good) {
            printf ("0x%X X after %d iterations\n",i,count);
        }
        else {
            printf ("0x%X OK\n", i);
        }
        i++;
    }
    return 0;
}

int test_single_byte_value (uint8_t val) {
    int ret;
    c011_init();
    c011_reset();
    uint8_t read;
    int count=0;
    while (true) {
        printf ("W 0x%X ", val);
        ret = c011_write_byte(val,200);
        if (ret == -1) {
            printf ("write timeout\n");
            break;
        }
        ret = c011_read_byte(&read, 200);
        if (ret == -1) {
            printf ("read timeout\n");
            break;
        } else {        
            printf ("R 0x%X ", read);
            if (read != val) {
                printf ("*E* write=0x%X read=0x%X\n",val,read);
                break;
            } else {
                printf ("OK %d\n", count++);
            }
        }
    }
    return 0;
}

int main(int argc, char *argv[])
{
    return test_all_byte_values();
//    return test_single_byte_value(0x40);
}
