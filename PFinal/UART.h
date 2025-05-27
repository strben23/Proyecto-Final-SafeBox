/*
 * UART.h
 *
 * Created: 4/9/2025 10:31:20 PM
 *  Author: Gaelp
 */ 

#ifndef UART_H_
#define UART_H_
#include <avr/io.h>

// Definición de parámetros
#define F_CPU 16000000UL // Frecuencia de 16MHz

#define UART_5BITS 0
#define UART_6BITS 1
#define UART_7BITS 2
#define UART_8BITS 3

#define UART_PARITY_DISABLE 0
#define UART_PARITY_EVEN 2
#define UART_PARITY_ODD 3

#define UART_STOPBIT_1 0
#define UART_STOPBIT_2 1

void UART_init(uint16_t baud_rate, uint8_t data_size, uint8_t parity_mode,
uint8_t stop_bits);
unsigned char UART_read();
void UART_write(unsigned char);
void UART_string(char*);

void UART_init(uint16_t baud_rate, uint8_t data_size, uint8_t parity_mode,
uint8_t stop_bits)
{
	// Configuración de baud rate
	uint16_t ubrr = (F_CPU/16/baud_rate)-1;
	UBRR0H = (uint8_t)(ubrr>>8);
	UBRR0L = (uint8_t)ubrr;
	
	// Configuración de tamaño de datos, paridad y bits de parada
	UCSR0C = (data_size<<UCSZ00) | (parity_mode<<UPM00) | (stop_bits<<USBS0);
	
	// Habilitar transmisor y receptor
	UCSR0B = (1<<TXEN0) | (1<<RXEN0);
}

unsigned char UART_read()
{
	if (UCSR0A & (1 << RXC0))
	{
		return UDR0;
	}
	return 0;
}

void UART_write(unsigned char data)
{
	while (!(UCSR0A & (1 << UDRE0)));
	UDR0 = data;
}

void UART_string(char* string)
{
	while (*string != 0)
	{
		UART_write(*string);
		string++;
	}
}

#endif /* UART_H_ */