#include "includes.h"

int main () {
	uart_init();
	i2c_init();
	_delay_ms(40);
	i2c_start();
	i2c_stop();
	while (1) {
	}
}
