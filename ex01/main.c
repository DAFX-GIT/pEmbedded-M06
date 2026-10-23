#include "includes.h"

int main () {
	uart_init();
	i2c_init();
	_delay_ms(40);
// init ATH20
	i2c_start(0);
	i2c_write(0xBE);
	i2c_write(0x08);
	i2c_write(0x00);
	i2c_stop();
	_delay_ms(80);

	while (1) {
	// ask for mesurement
		i2c_start(0);
		i2c_write(0xAC);
		i2c_write(0x33);
		i2c_write(0x00);
		i2c_stop();
		_delay_ms(80);

	// read the data sent back
		i2c_start(1);                       // read the 7 bytes
        for (uint8_t i = 0; i < 6; i++){
            i2c_read();      // ACK bytes 0-5, NACK byte 6
			uart_tx(' ');
		}
		i2c_read_nack();
		uart_print_str("\r\n");
        i2c_stop();
		_delay_ms(200);
	}
}
