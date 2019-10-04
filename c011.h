#include <stdint.h>

extern void c011_init(void);

extern void c011_reset(void);

extern void c011_analyse(void);

extern void c011_enable_out_int(void);

extern void c011_enable_in_int(void);

extern uint8_t c011_read_input_status(void);

extern uint8_t c011_read_output_status(void);

//timeout in ms
//return: 0 OK, -1 timeout
extern int c011_read_byte(uint8_t *byte, uint32_t timeout);

//timeout in ms
//return: 0 OK, -1 timeout
extern int c011_write_byte(uint8_t byte, uint32_t timeout);

//timeout in ms (per byte)
//return: num bytes read
extern uint32_t c011_read_bytes (uint8_t *bytes, uint32_t num, uint32_t timeout);

//timeout in ms (per byte)
//return: num bytes written
extern uint32_t c011_write_bytes (uint8_t *bytes, uint32_t num, uint32_t timeout);

