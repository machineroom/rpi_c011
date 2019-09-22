/*
  Test pins on RPI GFIO header. Simple toggle of pin to check function
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
#include <bcm2835.h>
#include "pins.h"

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

int main(int argc, char *argv[])
{
	int ret = 0;
	int fd;
    if (!bcm2835_init()) return 1;
	
    #define PIN RESET

 	bcm2835_gpio_fsel(PIN, BCM2835_GPIO_FSEL_OUTP);
 	while (1) {
    bcm2835_gpio_write(PIN, HIGH);
    delay(2);
    bcm2835_gpio_write(PIN, LOW);
    delay(2);
    }
	return ret;
}
