#define BAUD 115200UL
#include <avr/io.h>
#include <util/delay.h>

void i2c_init(void);
void i2c_start(void);
void i2c_stop(void);

void uart_init(void);
void uart_print_str(char *str);
void uart_print_hex(uint8_t value);