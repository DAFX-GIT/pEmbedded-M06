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

void i2c_start(void) {
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);	// START
	while (!(TWCR & (1 << TWINT)));
	print_status("START");

	TWDR = 0x38 << 1;									// AHT20 address + W
	TWCR = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)));
	print_status("SLA + W");
}