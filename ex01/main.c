#include "includes.h"

int main () {
	uart_init();
	i2c_init();
	_delay_ms(40);
// init ATH20
	i2c_start();
	i2c_write(0xBE);
	i2c_write(0x08);
	i2c_write(0x00);
	i2c_stop();
	_delay_ms(80);

	uint8_t data[7];

	while (1) {
	// ask for mesurement
		i2c_start();
		i2c_write(0xAC);
		i2c_write(0x33);
		i2c_write(0x00);
		i2c_stop();
		_delay_ms(80);

	// read the data sent back
		i2c_start();                       // read the 7 bytes
        for (uint8_t i = 0; i < 6; i++)
            data[i] = i2c_read();      // ACK bytes 0-5, NACK byte 6
		data[6] = i2c_read_nack();
        i2c_stop();
		

	}
}
