/*
  Test C011 (mode2) boot a T425
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
#include <bcm2835.h>


int main(int argc, char *argv[])
{
	int ret = 0;
	c011_init();
    c011_reset();
    c011_analyse();
    c011_enable_in_int();
    c011_enable_out_int();
    //blatant copy from https://github.com/hessch/rpilink/blob/master/utils/tdetect.py
    // & http://www.geekdot.com/category/software/transputer-software/ (iTest)
    uint8_t boot[] = 
            {
             0,                             /* size of bootstrap */
             0xB1,                          /* AJW 1, allow for Iptr store    */
             0xD1,                          /* STL 1                          */
             0x24, 0xF2,                    /* MINT                           */
             0x21, 0xFC,                    /* STHF                           */
             0x24, 0xF2,                    /* MINT                           */
             0x21, 0xF8,                    /* STLF                           */
             0xF0,                          /* REV                            */
             0x60, 0x5C,                    /* LDNLP -4                       */
             0x2A, 0x2A, 0x2A, 0x4A,        /* LDC #AAAA                      */
             0xFF,                          /* OUTWORD                        */
             0x21, 0x2F, 0xFF              /* START                          */
             };  
    boot[0] = sizeof (boot)-1;  /*"If the first byte received is 2 or greater then that many bytes of code 
                                   will be input from the link and placed in memory starting at MEMSTART.
                                   This code will then be executed."
                                   http://www.wizzy.com/wizzy/transputer_faq.txt*/
    printf ("write %d bytes to link\n",sizeof(boot)); 
    ret = c011_write_bytes (boot, sizeof(boot), 200);
    if (ret != sizeof(boot)) {
        printf ("failed to write bootstrap - ret = %d\n", ret);
        exit(-1);
    }
    uint8_t rx[8];
    memset(rx,0,sizeof(rx));
    uint32_t num;
    num = c011_read_bytes (rx, sizeof(rx), 200);
    printf ("num read = %u, reply = [%02X %02X %02X %02X %02X %02X]\n", num, rx[0], rx[1], rx[2], rx[3], rx[4], rx[5]);
	return ret;
}
