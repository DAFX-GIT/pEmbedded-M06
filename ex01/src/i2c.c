#include "../includes.h"

void print_status(char *step) {
	uart_print_str(step);
	uart_print_str(" : ");
	uart_print_hex(TWSR & 0xF8);
	uart_print_str("\r\n");
}

void i2c_init(void) {
	TWSR = 0;				// prescaler = 1
	TWBR = 72;				// 100 kHz @ 16 MHz
	TWCR = (1 << TWEN);		// enable TWI
}

void i2c_stop(void) {
	TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
	while (TWCR & (1 << TWSTO));	// wait until the STOP has been sent
}

void i2c_start(uint8_t read) {
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);	// START
	while (!(TWCR & (1 << TWINT)));

	TWDR = 0x38 << 1 | read;									// AHT20 address + W (+ 1 for read mode)
	TWCR = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)));
}

void i2c_write(unsigned char data){
	TWDR = data;
	TWCR = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)));
}

void i2c_read(void){
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
	while (!(TWCR & (1 << TWINT)));
	print_hex_value(TWDR);
}

void i2c_read_nack(void){
	TWCR = (1 << TWINT) | (1 << TWEN) | 0;
	while (!(TWCR & (1 << TWINT)));
	print_hex_value(TWDR);
}