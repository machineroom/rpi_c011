void rp1_gpio_get_bank(int num, int *bank, int *offset);
uint32_t rp1_gpio_sys_rio_out_read(volatile uint32_t *base, int bank, int offset);
void rp1_gpio_sys_rio_out_write(volatile uint32_t *base, int bank, int offset, uint32_t value);


void rp1_gpio_sys_rio_oe_clr(volatile uint32_t *base, int bank, int offset);
void rp1_gpio_sys_rio_oe_set(volatile uint32_t *base, int bank, int offset);

void rp1_gpio_sys_rio_oe_set_word(volatile uint32_t *base, int bank, uint32_t word);
void rp1_gpio_sys_rio_oe_clr_word(volatile uint32_t *base, int bank, uint32_t word);

uint32_t rp1_gpio_sys_rio_oe_read(volatile uint32_t *base, int bank);
