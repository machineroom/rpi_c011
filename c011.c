
/* MUST run code as root since BRCM timer registers used */

// RP1 chip as used on pi5
#define RP1

#define _POSIX_C_SOURCE 200809L

#include <bcm2835.h>
#include <time.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <unistd.h>
#include "pins.h"
#include "c011.h"

#ifdef RP1
#ifdef __cplusplus
extern "C" {
#endif
    #include "gpiolib.h"
    #include "gpiochip_rp1.h"
#ifdef __cplusplus
}
#endif
#endif

#define TCSLCSH (50)
#define TCSHCSL (50)
#define TCSLDrV (40)

static uint32_t bits=0;
#ifdef RP1
    static int gpio_bank;
    static int gpio_offset;
    static volatile uint32_t *gpio_base;
#else
    static volatile uint32_t *gpio_clr;
    static volatile uint32_t *gpio_set;
    static volatile uint32_t *gpio_fsel;
    static volatile uint32_t *gpio_lev;
#endif

typedef enum {
    UNDEFINED,
    OUTPUT,
    INPUT
} DATA_PINS_MODE;

DATA_PINS_MODE data_pins_mode = UNDEFINED;

static uint64_t total_reads=0;
static uint64_t total_read_waits=0;
static uint64_t total_read_timeouts=0;
static uint64_t total_read_success=0;

static uint64_t total_writes=0;
static uint64_t total_write_waits=0;
static uint64_t total_write_timeouts=0;
static uint64_t total_write_success=0;

void c011_dump_stats(const char *title) {
    printf ("C011 interfaces stats for '%s'\n",title);
    printf ("\ttotal reads %lu\n",total_reads);
    printf ("\ttotal read waits %lu\n",total_read_waits);
    printf ("\ttotal read timouts %lu\n",total_read_timeouts);
    printf ("\ttotal read successes %lu\n",total_read_success);
    printf ("\ttotal writes %lu\n",total_writes);
    printf ("\ttotal write waits %lu\n",total_write_waits);
    printf ("\ttotal write timouts %lu\n",total_write_timeouts);
    printf ("\ttotal write successes %lu\n",total_write_success);
    total_reads=0;
    total_read_waits=0;
    total_read_timeouts=0;
    total_read_success=0;
    total_writes=0;
    total_write_waits=0;
    total_write_timeouts=0;
    total_write_success=0;
}

/**
 * @brief sleep for specified ns (or longer). 
 * 
 * @param ns 
 */
static inline void sleep_ns(int ns) {
    //scope timing with rpi4 shows this is good enough for the small sleeps required by C011
    for (int i=0; i < ns; i++) {
        asm ("nop");
    }
}

static uint64_t get_ms() {
    struct timespec spec;
    clock_gettime (CLOCK_REALTIME, &spec);
    return spec.tv_sec * 1000 + spec.tv_nsec/1000000;
}

//testing with scope shows set_gpio_bit takes ~6ns (rpi4)
static inline void set_gpio_bit(uint8_t pin, uint8_t on) {
    if (on) {
        bits |= 1<<pin;
    } else {
        bits &= ~(1<<pin);
    }
}

static inline void gpio_commit(void) {
#ifdef RP1
   rp1_gpio_sys_rio_out_write(gpio_base, gpio_bank, gpio_offset, bits);
#else
    //testing with scope shows gpio_commit takes ~150ns (rpi4 -O3)
    // bcm2835_peri_write ~75ns
    // bcm2835_peri_write_nb ~5ns
    bcm2835_peri_write_nb (gpio_clr, ~bits);
    bcm2835_peri_write_nb (gpio_set, bits);
#endif
}

static void set_control_pins(void) {
#ifdef RP1
    gpio_set_fsel(RS0, GPIO_FSEL_OUTPUT);
    gpio_set_fsel(RS0, GPIO_FSEL_OUTPUT);
    gpio_set_fsel(RS1, GPIO_FSEL_OUTPUT);
    gpio_set_fsel(RESET, GPIO_FSEL_OUTPUT);
    gpio_set_fsel(CS, GPIO_FSEL_OUTPUT);
    gpio_set_fsel(RW, GPIO_FSEL_OUTPUT);
    gpio_set_fsel(BYTE, GPIO_FSEL_OUTPUT);
    gpio_set_fsel(BYTE_DIR, GPIO_FSEL_OUTPUT);
    gpio_set_fsel(INTFC_OE, GPIO_FSEL_OUTPUT);

    gpio_set_fsel(IN_INT, GPIO_FSEL_INPUT);
    gpio_set_fsel(OUT_INT, GPIO_FSEL_INPUT);
    gpio_set_pull(IN_INT, PULL_DOWN);
    gpio_set_pull(OUT_INT, PULL_DOWN);

#else
    bcm2835_gpio_fsel(RS0, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(RS1, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(RESET, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(CS, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(RW, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(BYTE, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(BYTE_DIR, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(INTFC_OE, BCM2835_GPIO_FSEL_OUTP);

    bcm2835_gpio_fsel(IN_INT, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_fsel(OUT_INT, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_set_pud(IN_INT, BCM2835_GPIO_PUD_DOWN);
    bcm2835_gpio_set_pud(OUT_INT, BCM2835_GPIO_PUD_DOWN);
#endif
}

static inline void set_data_output_pins(void) {
    if (data_pins_mode != OUTPUT) {
        // set the level shifters to be output
        set_gpio_bit (BYTE_DIR, HIGH);
        gpio_commit();
#ifdef RP1
        // bits 9-2 output (1)
        uint32_t oe = rp1_gpio_sys_rio_oe_read(gpio_base, gpio_bank);
        oe |= 0x3FC;
        rp1_gpio_sys_rio_oe_set_word(gpio_base, gpio_bank, oe);
#else
        //bits 9-2 output (001)
        //%00001001001001001001001001001001
        //    0   9   2   4   9   2   4   9
        bcm2835_peri_write_nb (gpio_fsel, 0x09249249);
#endif
        data_pins_mode = OUTPUT;
    }
}

static inline void set_data_input_pins(void) {
    if (data_pins_mode != INPUT) {
        // set the level shifters to be input
        set_gpio_bit (BYTE_DIR, LOW);
        gpio_commit();
#ifdef RP1
        uint32_t oe = rp1_gpio_sys_rio_oe_read(gpio_base, gpio_bank);
        oe &= ~0x3FC;
        rp1_gpio_sys_rio_oe_set_word(gpio_base, gpio_bank, oe);
#else
        //bits 9-2 input (000)
        bcm2835_peri_write_nb (gpio_fsel, 0);
        data_pins_mode = INPUT;
#endif
    }
}

//write byte to whatever register has been setup previously
static void c011_put_byte(uint8_t byte) {
    uint32_t word = (uint32_t)byte;
    //clear bits 2-9 and OR in our byte
    //1111 1111 1111 1111 1111 1100 0000 0011
    bits &= 0xFFFFFC03;
    word <<= 2;
    bits |= word;
    // commit the byte to the data pins before asserting CS
    gpio_commit();
    //CS=0
    set_gpio_bit(CS, LOW);
    gpio_commit();
    //CS=1
    set_gpio_bit(CS, HIGH);
    gpio_commit();
}

static void c011_enable_in_int(void) {
    set_data_output_pins();
    set_gpio_bit (RS1,1);
    set_gpio_bit (RS0,0);
    set_gpio_bit (RW,0);
    gpio_commit();
    c011_put_byte (0x02); // set int enable bit
}

static void c011_enable_out_int(void) {
    set_data_output_pins();
    set_gpio_bit (RS1,1);
    set_gpio_bit (RS0,1);
    set_gpio_bit (RW,0);
    gpio_commit();
    c011_put_byte (0x02); // set int enable bit
}

static void verbose_callback(const char *msg)
{
    printf("%s", msg);
}

#ifdef __cplusplus
extern "C" {
#endif
    #include "gpiochip_rp1.h"
#ifdef __cplusplus
}
#endif

void c011_init(void) {
#ifdef RP1
    gpiolib_set_verbose(&verbose_callback);
    int ret = gpiolib_init();
    unsigned int gpio = D0;
    ret = gpiolib_mmap();

    void *priv;
    const GPIO_CHIP_INTERFACE_T *iface_ignored = NULL;
    unsigned offset_ignored;
    gpio_get_interface(gpio, &iface_ignored, &priv, &offset_ignored);
    gpio_base = (volatile uint32_t *)priv;
    rp1_gpio_get_bank(gpio, &gpio_bank, &gpio_offset);

#else
    bcm2835_init();
    gpio_clr = bcm2835_regbase(BCM2835_REGBASE_GPIO) + BCM2835_GPCLR0/4;
    gpio_set = bcm2835_regbase(BCM2835_REGBASE_GPIO) + BCM2835_GPSET0/4;
    gpio_fsel = bcm2835_regbase(BCM2835_REGBASE_GPIO) + BCM2835_GPFSEL0/4;
    gpio_lev = bcm2835_regbase(BCM2835_REGBASE_GPIO) + BCM2835_GPLEV0/4;
#endif
    set_control_pins();
    set_gpio_bit(CS, HIGH);
    set_gpio_bit(INTFC_OE, LOW);   // enable the level shifters (!OE pin on 74LVC8T245)
    gpio_commit();

}

void c011_reset(void) {
    //TN29 states "Recommended pulse width is 5 ms, with a delay of 5 ms before sending anything down a link."
    set_gpio_bit(RESET, LOW);
    gpio_commit();
    set_gpio_bit(RESET, HIGH);
    gpio_commit();
    sleep(1);
//    bcm2835_delayMicroseconds (5*1000);
    set_gpio_bit(RESET, LOW);
    gpio_commit();
    c011_enable_in_int();
    c011_enable_out_int();
}

void c011_set_byte_mode(void) {
    set_gpio_bit (BYTE,HIGH);
    gpio_commit();
//    bcm2835_delay(1500);
    sleep(2);
}

void c011_clear_byte_mode(void) {
    set_gpio_bit (BYTE,LOW);
    gpio_commit();
    //The whitecross HSL takes some time to cascade
//    bcm2835_delay(1500);
    sleep(2);
}

void c011_analyse(void) {
    assert(true);
}

/**
 * @brief write a single byte to the C011
 * 
 * @param byte the byte to write
 * @param timeout timeout in ms
 * @return int -1 on timeout or 0 on success
 */
int c011_write_byte(uint8_t byte, uint32_t timeout) {
    total_writes++;
    //wait for OutputInt pin to go high (thereby indicating ready to write)
    uint64_t start;
#ifdef RP1
    start = get_ms();
    while (((gpio_get_level(OUT_INT)) == 0)) {
        if (get_ms() - start >  timeout) {
            total_write_timeouts++;
            return -1;
        }
        total_write_waits++;
    }
#else
    uint64_t timeout_us = timeout*1000;    // 1000us=1ms
    start = bcm2835_st_read();
    while (((bcm2835_peri_read_nb(gpio_lev) & (1<<OUT_INT)) == 0)) {
        if (bcm2835_st_read() - start > timeout_us) {
            total_write_timeouts++;
            return -1;
        }
        total_write_waits++;
    }
#endif
    set_data_output_pins();
    set_gpio_bit (RS1,0);
    set_gpio_bit (RS0,1);
    set_gpio_bit (RW,0);
    gpio_commit();
    c011_put_byte(byte);
    total_write_success++;
    return 0;
}

static uint8_t read_c011(void) {
    set_data_input_pins();
    set_gpio_bit(CS, LOW);
    gpio_commit();
    //should allow time for data valid after !CS (the code is slow enough for this to not be required?!)
    //sleep_ns (TCSLDrV);
    uint32_t reg = 0;
#ifdef RP1
    reg = rp1_gpio_sys_rio_out_read(gpio_base, gpio_bank, gpio_offset);
#else
    reg = bcm2835_peri_read_nb (gpio_lev);
#endif
    uint8_t byte;
    reg >>= 2;
    byte = reg;
    set_gpio_bit(CS, HIGH);
    gpio_commit();
    return byte;
}

/**
 * @brief read a single byte from the C011 link
 * 
 * @param byte 
 * @param timeout timeout in ms (or 0 blocking)
 * @return int -1 on timeout or 0 on success
 */
int c011_read_byte(uint8_t *byte, uint32_t timeout) {
    total_reads++;
    // wait for InputInt bit to go high
    if (timeout==0) {
#ifdef RP1
        while (gpio_get_level(IN_INT) == 0) {
            total_read_waits++;
        }
#else
        while ((bcm2835_peri_read_nb(gpio_lev) & (1<<IN_INT)) == 0) {
            total_read_waits++;
        }
#endif
    } else {
        uint64_t start;
#ifdef RP1
        start = get_ms();
        while (((gpio_get_level(IN_INT)) == 0)) {
            if (get_ms() - start >  timeout) {
                total_read_timeouts++;
                return -1;
            }
            total_read_waits++;
        }
#else
        uint64_t timeout_us = timeout*1000;    // 1000us=1ms
        start = bcm2835_st_read();
        while (((bcm2835_peri_read_nb(gpio_lev) & (1<<IN_INT)) == 0)) {
            if (bcm2835_st_read() - start > timeout_us) {
                total_read_timeouts++;
                return -1;
            }
            total_read_waits++;
        }
#endif
    }
    set_gpio_bit (RS1,0);
    set_gpio_bit (RS0,0);
    set_gpio_bit (RW,1);
    gpio_commit();
    *byte = read_c011();
    total_read_success++;
    return 0;
}

uint32_t c011_write_bytes (uint8_t *bytes, uint32_t num, uint32_t timeout) {
    uint32_t i;
    for (i=0; i < num; i++) {
        int ret = c011_write_byte(bytes[i], timeout);
        if (ret == -1) {
            break;
        }
    }
    return i;
}

uint32_t c011_read_bytes (uint8_t *bytes, uint32_t num, uint32_t timeout) {
    uint32_t i;
    for (i=0; i < num; i++) {
        int ret = c011_read_byte(&bytes[i], timeout);
        if (ret == -1) {
            break;
        }
    }
    return i;
}

uint8_t c011_read_input_status(void) {
    uint8_t byte;
    set_gpio_bit (RS1,1);
    set_gpio_bit (RS0,0);
    set_gpio_bit (RW,1);
    gpio_commit();
    byte = read_c011();
    return byte;
}

uint8_t c011_read_output_status(void) {
    uint8_t byte;
    set_gpio_bit (RS1,1);
    set_gpio_bit (RS0,1);
    set_gpio_bit (RW,1);
    gpio_commit();
    byte = read_c011();
    return byte;
}
