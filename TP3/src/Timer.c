#include <avr/io.h>
#include <avr/interrupt.h>

#define F_CPU 16000000UL
#define PRESCALER 64

// Cálculo para 0.1 segundos exactos:
// Frecuencia deseada = 10 Hz
// OCR1A = (F_CPU / (PRESCALER * 10)) - 1
// OCR1A = (16000000 / (64 * 10)) - 1 = 24999
#define OCR1A_VALUE 24999

// Variables globales
uint8_t flagMEF;
uint8_t timer1_counter;

void init_timer1_ctc(void) {
	// Configurar Timer1 en modo CTC
	TCCR1A = 0;                           // Normal port operation, CTC mode
	TCCR1B |= (1 << WGM12);               // CTC mode (Clear Timer on Compare Match)
	TCCR1B |= (1 << CS11) | (1 << CS10);  // Prescaler = 64 (CS12=0, CS11=1, CS10=1)

	// Configurar valor de comparación para 0.1 segundos
	OCR1A = OCR1A_VALUE;

	// Habilitar interrupción por comparación A
	TIMSK1 |= (1 << OCIE1A);

	// Inicializar variables
	flagMEF = 0;
	timer1_counter = 10;  // Contador para 10 interrupciones = 1 segundo
}

uint8_t get_flagTimer(void) {
	if(flagMEF) {
		flagMEF = 0;
		return 1;
	}
	return 0;
}

// ISR del Timer1 Compare Match A
// Se ejecuta cada 0.1 segundos exactos
ISR(TIMER1_COMPA_vect) {
	timer1_counter--;

	if(timer1_counter == 0) {
		flagMEF = 1;           // Setear flag cada 1 segundo
		timer1_counter = 10;   // Reiniciar contador
	}
}