#include "adc.h"
#include <avr/io.h>

void initADC() { 
	// Configuro ADC para leer potenciometro en ADC3
	ADMUX = (1<<REFS0) | (1<<MUX1) | (1<<MUX0);  // AVCC como referencia, canal ADC3
	ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);  // Habilito ADC, prescaler 128
	
	// Conversion dummy para inicializar
	ADCSRA |= (1<<ADSC);
	while(ADCSRA & (1<<ADSC));
 }

uint16_t readADC() {
	ADCSRA |= (1<<ADSC);
	while (ADCSRA & (1<<ADSC));
	uint16_t result = ADCL;
	result |= (ADCH << 8);
	return result;
}