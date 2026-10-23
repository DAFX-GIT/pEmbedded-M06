#include "includes.h"

uint32_t hum_buf[3], temp_buf[3];
uint8_t  idx = 0, count = 0;

/*
        d[1]      d[2]     d[3]      d[4]    d[5]
     ┌────────┬────────┬────┬────┬────────┬────────┐
     │HHHHHHHH│HHHHHHHH│HHHH│TTTT│TTTTTTTT│TTTTTTTT│
     └────────┴────────┴────┴────┴────────┴────────┘
      humidity: 8 + 8 + 4 = 20 bits │ temperature: 4 + 8 + 8 = 20 bits

*/

void add_measure(uint8_t *d) {
    hum_buf[idx]  = ((uint32_t)d[1] << 12) | ((uint32_t)d[2] << 4) | (d[3] >> 4);
    temp_buf[idx] = ((uint32_t)(d[3] & 0x0F) << 16) | ((uint32_t)d[4] << 8) | d[5];
    idx = (idx + 1) % 3;
    if (count < 3)
        count++;
}

void print_measure(void) {
    uint32_t sum_h = 0, sum_t = 0;
    for (uint8_t i = 0; i < count; i++) {
        sum_h += hum_buf[i];
        sum_t += temp_buf[i];
    }
    uint32_t avg_h = sum_h / count;
    uint32_t avg_t = sum_t / count;

    int32_t  temp = (int32_t)((avg_t * 625 + 16384) >> 15) - 5000;   // °C x 100
	/*
	datasheet formula: Temperature (°C) = S / 2²⁰ × 200 − 50
	Multiply the formula by 100: temp × 100 = S × 20000 / 2²⁰ − 5000.
	Overflow problem: S can be up to about 1 million, and 1 million × 20000 = 20 billion. That doesn’t fit in a uint32_t, which tops out around 4.29 billion.
	Simplify the fraction. 20000 / 1048576 = 625 / 32768 . largest product = 655 million, which fits.
	32768 = 2¹⁵, and dividing by a power of 2 is just a right shift: >> 15. It’s much faster than a real division on an 8-bit CPU.
	Rounding: + 16384. A shift always rounds down (7217.9 becomes 7217). Adding half the divisor first (16384 = 32768 / 2) turns that into normal rounding: anything ≥ .5 goes up.
	(int32_t) cast, then - 5000. The temperature can be negative below 0 °C. If you subtracted 5000 from an unsigned number that’s smaller than 5000, it would wrap around to a huge positive value. Casting to signed first allows negative results.
	*/
    uint32_t hum  = (avg_h * 125 + 65536) >> 17;                     // %  x 10
	/*
	datasheet formula: Humidity (%) = S / 2²⁰ × 100
	Multiply by 10: hum × 10 = S × 1000 / 2²⁰.
	Simplify: 1000 / 1048576 divided top and bottom by 8 gives 125 / 131072.
	131072 = 2¹⁷, so the division becomes >> 17.
	Rounding: + 65536, which is half of 131072.
	The largest product is about 1 million × 125 = 131 million, so it fits, and humidity is never negative, so it stays unsigned.
	*/

    uart_print_str("Temperature: ");
    if (temp < 0) {
        uart_tx('-');
        temp = -temp;
    }
    uart_putnbr(temp / 100);
    uart_tx('.');
    if (temp % 100 < 10)
        uart_tx('0');                       // 23.05, not 23.5
    uart_putnbr(temp % 100);
    uart_print_str("\xC2\xB0" "C, Humidity: ");   // UTF-8 degree sign

    uart_putnbr(hum / 10);
    uart_tx('.');
    uart_putnbr(hum % 10);
    uart_print_str("%\r\n");
}

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

	uint8_t data[7];

	while (1) {
	// ask for mesurement
		i2c_start(0);
		i2c_write(0xAC);
		i2c_write(0x33);
		i2c_write(0x00);
		i2c_stop();
		_delay_ms(80);

	// read the data sent back
		i2c_start(1);
        for (uint8_t i = 0; i < 6; i++){
            data[i] = i2c_read();      // ACK bytes 0-5, 
		}
		data[6] = i2c_read_nack(); //NACK byte 6

		if (!(data[0] & 0x80)) {        // only if the sensor wasn't busy
    		add_measure(data);
    		print_measure();
		}
        i2c_stop();
		_delay_ms(200);
	}
}
