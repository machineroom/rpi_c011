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
	int ret = 0;
	int fd;
    uint8_t i=0;
	c011_init();
    reset();
    enable_in_int();
    enable_out_int();
    uint8_t read;
    while (true) {
        write_byte(i);
        read = read_byte();
        if (read != i) {
            printf ("*E* write=0x%X read=0x%X\n",i,read);
        }
        i++;
    }
	
	return ret;
}
