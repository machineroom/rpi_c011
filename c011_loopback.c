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
    // AA,55 OK  10101010 01010101  4,4
    // 33,CC OK  00110011 11001100  4,4
    // 44,BB OK  01000100 10111011  2,6
    // 66,99 OK  01100110 10011001  4,4
    // 22,DD OK  00100010 11011101  2,6 
    // 22,DE OK  00100010 11011110  2,6
    // 09,F6 OK  00001001 11110110  2,6
    // FF    OK  11111111
    // FE    OK  11111110
    // FC    OK  11111100
    // 7C    OK  01111100
    // 3C    OK  00111100
    // 1C    OK  00011100
    // 31    OK  00110001
    // 43    OK  01000011
    // 84    OK  10000100
    // 42    OK  01000010

    // 21    BAD 00100001
    // 41    BAD 01000001
    // 18    BAD 00011000
    // 38    BAD 00111000
    // 88    BAD 10001000
    // 81    BAD 10000001
    // 80    BAD 10000000
    // 11    BAD 00010001
    // 08    BAD 00001000
    // 09    BAD 00001001 (but takes time)
    // 07    BAD 00000111 (but takes time)           W 07 R 20
    // 03    BAD 00000003
    // 01    BAD 00000001
    // 00    BAD 00000000 (really quickly)
    // 18,38 BAD 00011000 00111000  2,3
    // 21,DD BAD 00100001 11011101  2,6     W 21 R FF
    // 21,DE BAD 00100001 11011110  2,6     W 21 R FB
    // 20,DF BAD 00100000 11011111  1,7     W 20 R 28
    // 11,EE BAD 00010001 11101110  2,6     W 11 R FF
    uint8_t i=0x42;
    c011_init();
    c011_reset();
    uint8_t read;
    int count=0;
    while (true) {
        printf ("W 0x%X ", i);
        ret = c011_write_byte(i,200);
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
            if (read != i) {
                printf ("*E* write=0x%X read=0x%X\n",i,read);
                break;
            } else {
                printf ("OK %d\n", count++);
            }
        }
        //if (i==0x18) i=0x38; else i=0x18;
        //i = ~i;
    }
    return 0;
}
