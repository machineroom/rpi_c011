#include <stdint.h>

extern void c011_init(void);

extern void reset(void);

extern void enable_out_int(void);

extern void enable_in_int(void);

extern void write_byte(uint8_t byte);

extern uint8_t read_c011(void);

extern uint8_t read_input_status(void);

extern uint8_t read_output_status(void);

extern uint8_t read_byte(void);

extern void c011_write_bytes (uint8_t *bytes, uint32_t num);

extern void c011_read_bytes (uint8_t *bytes, uint32_t num);
