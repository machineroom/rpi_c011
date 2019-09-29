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
    c011_enable_in_int();
    c011_enable_out_int();
    uint8_t read;
    while (true) {
        c011_write_byte(i);
        ret = c011_read_byte(&read, 200);
        if (ret == -1) {
            printf ("timeou\n");
            } else {        
            if (read != i) {
                printf ("*E* write=0x%X read=0x%X\n",i,read);
            }
        }
        i++;
    }
	return 0;
}
