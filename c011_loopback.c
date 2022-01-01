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
    uint8_t i=0;
    c011_init();
    c011_reset();
    uint8_t read;
    int count=0;
    while (true) {
        c011_write_byte(i,200);
        ret = c011_read_byte(&read, 200);
        if (ret == -1) {
            printf ("timeout\n");
        } else {        
            if (read != i) {
                printf ("*E* write=0x%X read=0x%X\n",i,read);
            } else {
                printf ("OK %d\n", count++);
            }
        }
        i++;
    }
    return 0;
}
