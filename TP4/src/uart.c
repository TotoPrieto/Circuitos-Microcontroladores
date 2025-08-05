#include "uart.h"
#include <avr/io.h>

void UART_Init() { 

	UBRR0H = 0;
	UBRR0L = 103;
	
	// Habilita RX, TX y la interrupcion
	UCSR0B = (1<<RXEN0)|(1<<TXEN0)|(1<<RXCIE0);
	
	// 8 data, 1 stop bit
	UCSR0C = (1<<UCSZ01)|(1<<UCSZ00); 
	}
	
void UART_Transmit(unsigned char data) { 
	//Espero buffer vacio
	while (!(UCSR0A & (1<<UDRE0)));
	//Cargar y enviar dato en buffer
	UDR0 = data;
 }
void UART_SendString(const char* str) { 	
	while(*str){
		UART_Transmit(*str);
		str++;
	} 
}