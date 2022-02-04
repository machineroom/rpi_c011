/*
  Test C011 (mode2) loopback. Loop LinkIn-LinkOut on C011
 */

// POWER IS CRITICAL! without good 3V3 and 5V feeds to the TXS0108 suffer random power drops for certain byte values...  


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

int test_random_byte_values (void) {
    int ret;
    c011_init();
    c011_reset();
    uint8_t read;
    int count=0;
    uint64_t start;
    start = bcm2835_st_read();

    while (true) {
        uint8_t val = rand();
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
            if (read != val) {
                printf ("*E* write=0x%X read=0x%X\n",val,read);
                break;
            } else {
            }
        }
        count++;
        if (count%1000000==0) {
            uint64_t now = bcm2835_st_read();
            //1M bytes Tx & Rx in uS
            double bits = 8.0f * 1000000.0f * 2.0f;
            double seconds = (double)(now-start)/1000000.0f;
            printf ("%f Mbits/second (total %dM bytes)\n",bits/seconds/1000000.0f, count/1000000);
            start = now;
        }
    }
    return 0;
}

int main(int argc, char *argv[])
{
    return test_random_byte_values();
//    return test_all_byte_values();
//    return test_single_byte_value(0x40);
}
