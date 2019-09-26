#include <bcm2835.h>
#include "pins.h"
#include "c011.h"

//all the C011 timings are <1uS (60ns top)
//TODO use nanosleep
#define TRWVCSL (0)
#define TCSLCSH (1) //60ns
#define TCSHCSL (0)
#define TRHRL (1)
#define TRSVCSL (0)
#define TCSLDrV (1) //50ns

static void set_control_output_pins(void) {
    bcm2835_gpio_fsel(RS0, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(RS1, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(RESET, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(CS, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(RW, BCM2835_GPIO_FSEL_OUTP);
}

static void set_data_output_pins(void) {
    bcm2835_gpio_fsel(D0, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(D1, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(D2, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(D3, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(D4, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(D5, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(D6, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(D7, BCM2835_GPIO_FSEL_OUTP);
}

static void set_data_input_pins(void) {
    bcm2835_gpio_fsel(D0, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_fsel(D1, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_fsel(D2, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_fsel(D3, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_fsel(D4, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_fsel(D5, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_fsel(D6, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_fsel(D7, BCM2835_GPIO_FSEL_INPT);
}

void c011_init(void) {
    bcm2835_init();
    set_control_output_pins();
}

void reset(void) {
    //RESET=1
    bcm2835_gpio_write(RESET, LOW);
    bcm2835_delayMicroseconds (TRHRL);
    bcm2835_gpio_write(RESET, HIGH);
    bcm2835_delayMicroseconds (TRHRL);
    bcm2835_gpio_write(RESET, LOW);
}

void enable_out_int(void) {
    set_data_output_pins();
    bcm2835_gpio_write_mask (1<<RS1 | 1<<RS0 | 0<<RW | 1<<CS,
                             1<<RS1 | 1<<RS0 | 1<<RW | 1<<CS);
    bcm2835_gpio_write_mask (1<<D1, 1<<D1);
    bcm2835_gpio_write(CS, LOW);
    bcm2835_delayMicroseconds (TCSLCSH);
    //CS=1
    bcm2835_gpio_write(CS, HIGH);
}

void enable_in_int(void) {
    set_data_output_pins();
    bcm2835_gpio_write_mask (1<<RS1 | 0<<RS0 | 0<<RW | 1<<CS,
                             1<<RS1 | 1<<RS0 | 1<<RW | 1<<CS);
    bcm2835_gpio_write_mask (1<<D1, 1<<D1);
    bcm2835_gpio_write(CS, LOW);
    bcm2835_delayMicroseconds (TCSLCSH);
    //CS=1
    bcm2835_gpio_write(CS, HIGH);
}

void write_byte(uint8_t byte) {
    //wait for output ready
    while (read_output_status() & 0x01 != 0x01) {
        printf ("OW\n");
        bcm2835_delayMicroseconds(1);
    }
    //RS1=0, RS0=1
    //RW=0
    //CS=1
    set_data_output_pins();
    bcm2835_gpio_write_mask (0<<RS1 | 1<<RS0 | 0<<RW | 1<<CS,
                             1<<RS1 | 1<<RS0 | 1<<RW | 1<<CS);
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
    bcm2835_gpio_write_mask (d7<<D7 | d6<<D6 | d5<<D5 | d4<<D4 | d3<<D3 | d2<<D2 | d1<<D1 | d0<<D0,
                             1<<D7 | 1<<D6 | 1<<D5 | 1<<D4 | 1<<D3 | 1<<D2 | 1<<D1 | 1<<D0);
    //CS=0
    bcm2835_gpio_write(CS, LOW);
    bcm2835_delayMicroseconds (TCSLCSH);
    //CS=1
    bcm2835_gpio_write(CS, HIGH);
}

uint8_t read_c011(void) {
    uint8_t d7,d6,d5,d4,d3,d2,d1,d0,byte;
    set_data_input_pins();
    bcm2835_gpio_write(CS, LOW);
    //must allow time for data valid after !CS
    bcm2835_delayMicroseconds (TCSLDrV);
    d7=bcm2835_gpio_lev(D7);
    d6=bcm2835_gpio_lev(D6);
    d5=bcm2835_gpio_lev(D5);
    d4=bcm2835_gpio_lev(D4);
    d3=bcm2835_gpio_lev(D3);
    d2=bcm2835_gpio_lev(D2);
    d1=bcm2835_gpio_lev(D1);
    d0=bcm2835_gpio_lev(D0);
    byte = d7;
    byte <<= 1;
    byte |= d6;
    byte <<= 1;
    byte |= d5;
    byte <<= 1;
    byte |= d4;
    byte <<= 1;
    byte |= d3;
    byte <<= 1;
    byte |= d2;
    byte <<= 1;
    byte |= d1;
    byte <<= 1;
    byte |= d0;
    bcm2835_gpio_write(CS, HIGH);
    return byte;
}

uint8_t read_input_status(void) {
    uint8_t byte;
    bcm2835_gpio_write_mask (1<<RS1 | 0<<RS0 | 1<<RW | 1<<CS,
                             1<<RS1 | 1<<RS0 | 1<<RW | 1<<CS);
    byte = read_c011();
    return byte;
}

uint8_t read_output_status(void) {
    uint8_t byte;
    bcm2835_gpio_write_mask (1<<RS1 | 1<<RS0 | 1<<RW | 1<<CS,
                             1<<RS1 | 1<<RS0 | 1<<RW | 1<<CS);
    byte = read_c011();
    return byte;
}

uint8_t read_byte(void) {
    uint8_t byte;
    while (read_input_status() & 0x01 == 0x00) {
        printf ("IW\n");
        bcm2835_delayMicroseconds(1);
    }
    bcm2835_gpio_write_mask (0<<RS1 | 0<<RS0 | 1<<RW | 1<<CS,
                             1<<RS1 | 1<<RS0 | 1<<RW | 1<<CS);
    byte = read_c011();
    return byte;
}

void c011_write_bytes (uint8_t *bytes, uint32_t num) {
    for (uint32_t i=0; i < num; i++) {
        write_byte(bytes[i]);
    }
}

void c011_read_bytes (uint8_t *bytes, uint32_t num) {
    for (uint32_t i=0; i < num; i++) {
        bytes[i] = read_byte();
    }
}

