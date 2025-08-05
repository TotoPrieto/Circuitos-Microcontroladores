#include <avr/interrupt.h>
#include <MEF.h>
#include <Timer.h>

#define F_CPU 16000000UL  // Frecuencia del reloj del sistema (16 MHz)
#define PRESCALER 64

// Calculamos el valor de OCR0A para generar una interrupción cada 1 ms
#define OCR0A_VALUE ((F_CPU / (PRESCALER * 1000)) - 1)

uint64_t tiempoTimer = 0;
uint8_t flagMEF;
uint8_t cont;

void init_timer0_ctc(void) {
	TCCR0A |= (1 << WGM01);               // Modo CTC (Clear Timer on Compare Match)
	TCCR0B |= (1 << CS01) | (1 << CS00);  // Prescaler = 64

	OCR0A = OCR0A_VALUE;                  // Valor del comparador

	TIMSK0 |= (1 << OCIE0A);              // Habilita interrupción por comparación A
	
	flagMEF=0;
	cont=1;
}

uint64_t get_tiempoTimer(){
	return tiempoTimer;
}

uint8_t get_flagMEF(){
	if(flagMEF){
		flagMEF=0;
		cont=1;
		return 1;
	}
	return 0;
}

// ISR del Timer0 Compare Match A
ISR(TIMER0_COMPA_vect) {
	tiempoTimer++;
	cont--;
	if(!cont){
		flagMEF = 1;
	}
}