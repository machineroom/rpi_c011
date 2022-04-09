/*
  Test hardware interface by waggling bits
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

static uint32_t bits=0;
static volatile uint32_t *gpio_clr;
static volatile uint32_t *gpio_set;
static volatile uint32_t *gpio_fsel;
static volatile uint32_t *gpio_lev;

typedef enum {
    UNDEFINED,
    OUTPUT,
    INPUT
} DATA_PINS_MODE;

DATA_PINS_MODE data_pins_mode = UNDEFINED;

//testing with scope shows set_gpio_bit takes ~6ns
static inline void set_gpio_bit(uint8_t pin, uint8_t on) {
    if (on) {
        bits |= 1<<pin;
    } else {
        bits &= ~(1<<pin);
    }
}

//testing with scope shows gpio_commit takes ~150ns (rpi4 -O3)
// bcm2835_peri_write ~75ns
// bcm2835_peri_write_nb ~5ns
static inline void gpio_commit(void) {
    bcm2835_peri_write_nb (gpio_clr, ~bits);
    bcm2835_peri_write_nb (gpio_set, bits);
}

static void set_control_pins(void) {
    bcm2835_gpio_fsel(RS0, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(RS1, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(RESET, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(CS, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(RW, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(BYTE, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(BYTE_DIR, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(INTFC_OE, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(OUT_SPARE_1, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(OUT_SPARE_2, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(OUT_SPARE_3, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(OUT_SPARE_4, BCM2835_GPIO_FSEL_OUTP);

    bcm2835_gpio_fsel(IN_INT, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_fsel(OUT_INT, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_set_pud(IN_INT, BCM2835_GPIO_PUD_DOWN);
    bcm2835_gpio_set_pud(OUT_INT, BCM2835_GPIO_PUD_DOWN);
}

static inline void set_data_output_pins(void) {
    // set the level shifters to be output
    set_gpio_bit (BYTE_DIR, HIGH);
    gpio_commit();
    if (data_pins_mode != OUTPUT) {
        //bits 9-0 output (001)
        //%00001001001001001001001001001001
        //    0   9   2   4   9   2   4   9
        bcm2835_peri_write_nb (gpio_fsel, 0x09249249);
        data_pins_mode = OUTPUT;
    }
}

static inline void set_data_input_pins(void) {
    // set the level shifters to be input
    set_gpio_bit (BYTE_DIR, LOW);
    gpio_commit();
    if (data_pins_mode != INPUT) {
        //bits 9-0 input (000)
        bcm2835_peri_write_nb (gpio_fsel, 0);
        data_pins_mode = INPUT;
    }
}

void toggle_pin (uint8_t bit) {
    for (int i=0; i < 100; i++) {
        set_gpio_bit(bit, HIGH);
        gpio_commit();
        usleep(1);
        set_gpio_bit(bit, LOW);
        gpio_commit();
        usleep(1);
    }
}

int main(int argc, char *argv[])
{
    bcm2835_init();
    gpio_clr = bcm2835_regbase(BCM2835_REGBASE_GPIO) + BCM2835_GPCLR0/4;
    gpio_set = bcm2835_regbase(BCM2835_REGBASE_GPIO) + BCM2835_GPSET0/4;
    gpio_fsel = bcm2835_regbase(BCM2835_REGBASE_GPIO) + BCM2835_GPFSEL0/4;
    gpio_lev = bcm2835_regbase(BCM2835_REGBASE_GPIO) + BCM2835_GPLEV0/4;
    set_control_pins();

    toggle_pin (INTFC_OE);
    //toggle_pin (BYTE_DIR);
    toggle_pin (OUT_SPARE_1);
    toggle_pin (OUT_SPARE_2);
    toggle_pin (OUT_SPARE_3);
    toggle_pin (OUT_SPARE_4);

    set_data_output_pins();
    set_gpio_bit(D0, LOW);
    set_gpio_bit(D1, LOW);
    set_gpio_bit(D2, LOW);
    set_gpio_bit(D3, LOW);
    set_gpio_bit(D4, LOW);
    set_gpio_bit(D5, LOW);
    set_gpio_bit(D6, LOW);
    set_gpio_bit(D7, LOW);
    toggle_pin (D0);
    toggle_pin (D1);
    toggle_pin (D2);
    toggle_pin (D3);
    toggle_pin (D4);
    toggle_pin (D5);
    toggle_pin (D6);
    toggle_pin (D7);
}
