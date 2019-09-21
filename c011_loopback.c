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
#include <bcm2835.h>
#include "pins.h"

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

void set_output_pins(void) {
    bcm2835_gpio_fsel(D0, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(D1, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(D2, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(D3, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(D4, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(D5, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(D6, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(D7, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(RS0, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(RS1, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(RESET, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(CS, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(RW, BCM2835_GPIO_FSEL_OUTP);
}

//all the C011 timings are <1uS (60ns top)
//TODO use nanosleep
#define TRWVCSL (0)
#define TCSLCSH (0)
#define TCSHCSL (0)
#define TRHRL (0)
#define TRSVCSL (0)

void reset(void) {
    //RESET=1
    bcm2835_gpio_write(RESET, HIGH);
    bcm2835_delayMicroseconds (TRHRL);
    bcm2835_gpio_write(RESET, LOW);
}

void enable_out_int(void) {
    bcm2835_gpio_write_mask (1<<RS1 | 1<<RS0 | 0<<RW | 1<<CS,
                             1<<RS1 | 1<<RS0 | 1<<RW | 1<<CS);
    bcm2835_gpio_write_mask (1<<D1, 1<<D1);
    bcm2835_delayMicroseconds (TRSVCSL);
    bcm2835_gpio_write(CS, LOW);
    bcm2835_delayMicroseconds (TCSLCSH);
    //CS=1
    bcm2835_gpio_write(CS, HIGH);
    bcm2835_delayMicroseconds (TCSHCSL);
}

void write_byte(uint8_t byte) {
    //RS1=0, RS0=1
    //RW=0
    //CS=1
    bcm2835_gpio_write_mask (0<<RS1 | 1<<RS0 | 0<<RW | 1<<CS,
                             1<<RS1 | 1<<RS0 | 1<<RW | 1<<CS);
    bcm2835_delayMicroseconds(TRWVCSL);
    //D0-D7
    uint8_t d7,d6,d5,d4,d3,d2,d1,d0;
    d7=(byte&0x80) >> 7;
    d6=(byte&0x40) >> 6;
    d5=(byte&0x20) >> 5;
    d4=(byte&0x10) >> 4;
    d3=(byte&0x08) >> 3;
    d2=(byte&0x04) >> 2;
    d1=(byte&0x02) >> 1;
    d0=(byte&0x01) >> 0;
    printf ("%d %d %d %d %d %d %d %d\n",d7,d6,d5,d4,d3,d2,d1,d0);
    bcm2835_gpio_write_mask (d7<<D7 | d6<<D6 | d5<<D5 | d4<<D4 | d3<<D3 | d2<<D2 | d1<<D1 | d0<<D0,
                             1<<D7 | 1<<D6 | 1<<D5 | 1<<D4 | 1<<D3 | 1<<D2 | 1<<D1 | 1<<D0);
    bcm2835_delayMicroseconds (TRSVCSL);
    //CS=0
    bcm2835_gpio_write(CS, LOW);
    bcm2835_delayMicroseconds (TCSLCSH);
    //CS=1
    bcm2835_gpio_write(CS, HIGH);
    bcm2835_delayMicroseconds (TCSHCSL);
}

int main(int argc, char *argv[])
{
	int ret = 0;
	int fd;
    if (!bcm2835_init()) return 1;
    uint8_t i=0;
    reset();
    enable_out_int();
    while (true) {
        write_byte(i);
        sleep(1);
        i++;
    }
	
	return ret;
}
