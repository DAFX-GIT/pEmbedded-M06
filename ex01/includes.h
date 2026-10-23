#define BAUD 115200UL
#include <avr/io.h>
#include <util/delay.h>

void i2c_init(void);
void i2c_start(uint8_t read);
void i2c_stop(void);
void i2c_write(unsigned char data);
void i2c_read(void);
void i2c_read_nack(void);

void uart_init(void);
void uart_print_str(char *str);
void uart_print_hex(uint8_t value);
void print_hex_value(char c);
void uart_tx(char c);