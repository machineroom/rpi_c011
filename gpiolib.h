#ifndef GPIOLIB_H
#define GPIOLIB_H

#include <stdint.h>

#define NUM_HDR_PINS 40
#define MAX_GPIO_PINS 300

#define GPIO_INVALID (~0U)
#define GPIO_GND (~1U)
#define GPIO_5V (~2U)
#define GPIO_3V3 (~3U)
#define GPIO_1V8 (~4U)
#define GPIO_OTHER (~5U)

typedef enum
{
    GPIO_FSEL_FUNC0,
    GPIO_FSEL_FUNC1,
    GPIO_FSEL_FUNC2,
    GPIO_FSEL_FUNC3,
    GPIO_FSEL_FUNC4,
    GPIO_FSEL_FUNC5,
    GPIO_FSEL_FUNC6,
    GPIO_FSEL_FUNC7,
    GPIO_FSEL_FUNC8,
    /* ... */
    GPIO_FSEL_INPUT = 0x10,
    GPIO_FSEL_OUTPUT,
    GPIO_FSEL_GPIO, /* Preserves direction if possible, else input */
    GPIO_FSEL_NONE, /* If possible, else input */
    GPIO_FSEL_MAX
} GPIO_FSEL_T;

typedef enum
{
    PULL_NONE,
    PULL_DOWN,
    PULL_UP,
    PULL_MAX
} GPIO_PULL_T;

typedef enum
{
    DIR_INPUT,
    DIR_OUTPUT,
    DIR_MAX,
} GPIO_DIR_T;

typedef enum
{
    DRIVE_LOW,
    DRIVE_HIGH,
    DRIVE_MAX
} GPIO_DRIVE_T;

typedef struct GPIO_CHIP_INTERFACE_ GPIO_CHIP_INTERFACE_T;
#define DECLARE_GPIO_CHIP(name, compatible, iface, size, data) \
    GPIO_CHIP_T name ## _chip __attribute__ ((section ("gpiochips"))) = \
    { #name, compatible, iface, size, data }

typedef struct GPIO_CHIP_
{
    const char *name;
    const char *compatible;
    const GPIO_CHIP_INTERFACE_T *interface;
    int size;
    uintptr_t data;
} GPIO_CHIP_T;

struct GPIO_CHIP_INTERFACE_
{
    void * (*gpio_create_instance)(const GPIO_CHIP_T *chip, const char *dtnode);
    int (*gpio_count)(void *priv);
    void * (*gpio_probe_instance)(void *priv, volatile uint32_t *base);
    GPIO_FSEL_T (*gpio_get_fsel)(void *priv, uint32_t gpio);
    void (*gpio_set_fsel)(void *priv, uint32_t gpio, const GPIO_FSEL_T func);
    void (*gpio_set_drive)(void *priv, uint32_t gpio, GPIO_DRIVE_T drv);
    void (*gpio_set_dir)(void *priv, uint32_t gpio, GPIO_DIR_T dir);
    GPIO_DIR_T (*gpio_get_dir)(void *priv, uint32_t gpio);
    int (*gpio_get_level)(void *priv, uint32_t gpio);  /* The actual level observed */
    GPIO_DRIVE_T (*gpio_get_drive)(void *priv, uint32_t gpio);  /* What it is being driven as */
    GPIO_PULL_T (*gpio_get_pull)(void *priv, uint32_t gpio);
    void (*gpio_set_pull)(void *priv, uint32_t gpio, GPIO_PULL_T pull);
    const char * (*gpio_get_name)(void *priv, uint32_t gpio);
    const char * (*gpio_get_fsel_name)(void *priv, uint32_t gpio, GPIO_FSEL_T fsel);
};

extern const GPIO_CHIP_T __start_gpiochips;
extern const GPIO_CHIP_T __stop_gpiochips;

int gpiolib_init(void);
int gpiolib_init_by_name(const char *name);
int gpiolib_mmap(void);
void gpiolib_set_verbose(void (*callback)(const char *));
int gpio_get_interface(unsigned gpio,
                              const GPIO_CHIP_INTERFACE_T **iface_ptr,
                              void **priv, unsigned *offset);

int gpio_num_is_valid(unsigned gpio);
GPIO_DIR_T gpio_get_dir(unsigned gpio);
void gpio_set_dir(unsigned gpio, GPIO_DIR_T dir);
GPIO_FSEL_T gpio_get_fsel(unsigned gpio);
void gpio_set_fsel(unsigned gpio, const GPIO_FSEL_T func);
void gpio_set_drive(unsigned gpio, GPIO_DRIVE_T drv);
void gpio_set(unsigned gpio);
void gpio_clear(unsigned gpio);
int gpio_get_level(unsigned gpio);  /* The actual level observed */
GPIO_DRIVE_T gpio_get_drive(unsigned gpio);  /* What it is being driven as */
GPIO_PULL_T gpio_get_pull(unsigned gpio);
void gpio_set_pull(unsigned gpio, GPIO_PULL_T pull);

void gpio_get_pin_range(unsigned *first, unsigned *last);
unsigned gpio_for_pin(int pin);
int gpio_to_pin(unsigned gpio);
unsigned gpio_get_gpio_by_name(const char *name, int namelen);
const char *gpio_get_name(unsigned gpio);
const char *gpio_get_gpio_fsel_name(unsigned gpio, GPIO_FSEL_T fsel);
const char *gpio_get_fsel_name(GPIO_FSEL_T fsel);
const char *gpio_get_pull_name(GPIO_PULL_T pull);
const char *gpio_get_drive_name(GPIO_DRIVE_T drive);

#endif
