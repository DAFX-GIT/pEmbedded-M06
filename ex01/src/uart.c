#include "../includes.h"

void uart_init(void) {
    uint16_t ubrr = (F_CPU / (8UL * BAUD)) - 1;   // U2X: 2.1% baud error vs 3.5% in normal mode
    UBRR0H = (uint8_t)(ubrr >> 8);					// UBRR is 12 bits, split across two 8-bit registers: HIGH
    UBRR0L = (uint8_t)ubrr;							// here LOW
    UCSR0A = (1 << U2X0);                          // also clears leftovers from a bootloader
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);			// transmitter and receiver on
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);        // 8N1
}

void uart_tx(char c) {
	while (!(UCSR0A & (1 << UDRE0))); // wait until transmit buffer is empty
	UDR0 = c;
}

void uart_print_str(char *str) { 
	while (*str) {
		uart_tx(*str++);
	}
}

void uart_print_hex(uint8_t value) {
	const char hex[] = "0123456789ABCDEF";
	uart_print_str("0x");
	uart_tx(hex[value >> 4]);               // high nibble
	uart_tx(hex[value & 0x0F]);             // low nibble
}

void print_hex_value(char c) {
	const char hex[] = "0123456789ABCDEF";
	unsigned char u = (unsigned char)c;
	uart_tx(hex[u >> 4]);               // high nibble
	uart_tx(hex[u & 0x0F]);             // low nibble
}