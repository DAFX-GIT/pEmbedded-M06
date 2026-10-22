#include "includes.h"

int main () {
	uart_init();
	i2c_init();
	i2c_start();
	i2c_stop();
	while (1) {
	}
}
