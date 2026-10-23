#include "../includes.h"

/*
I2C (called TWI on AVR) is a serial bus to talk to chips like sensors.
It only needs 2 wires, shared by all the devices on the bus.
One master (the ATmega328P) drives the talk, the slaves (the AHT20) answer.
Each slave has a 7-bit address (0x38 for the AHT20).

SDA (Serial DAta, PC4): carries the bits, in both directions.
SCL (Serial CLock, PC5): the clock, given by the master. One bit per clock pulse.

Both lines are open-drain with pull-up resistors: idle is HIGH, a device can only pull them LOW.
START = SDA goes LOW while SCL is HIGH. STOP = SDA goes HIGH while SCL is HIGH.
After each byte, the receiver sends an ACK (SDA LOW) or a NACK (SDA HIGH).
*/

void print_status(char *step) {
	uart_print_str(step);
	uart_print_str(" : ");
	uart_print_hex(TWSR & 0xF8);
	uart_print_str("\r\n");
}

void i2c_init(void) {
	TWSR = 0;				// prescaler = 1
	TWBR = 72;				// SCL = F_CPU / (16 + 2 * TWBR * prescaler) = 16 MHz / 160 = 100 kHz
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