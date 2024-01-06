void rp1_gpio_get_bank(int num, int *bank, int *offset);
uint32_t rp1_gpio_sys_rio_out_read(volatile uint32_t *base, int bank, int offset);
void rp1_gpio_sys_rio_out_write(volatile uint32_t *base, int bank, int offset, uint32_t value);


