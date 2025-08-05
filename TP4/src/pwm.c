#include "pwm.h"
#include <avr/io.h>
#include <avr/interrupt.h>

#define PWM_ON  (PORTB &= ~(1<<PB5))
#define PWM_OFF (PORTB |= (1<<PB5))
#define PWM_PERIOD 255

volatile uint8_t PWM_DELTA = 0;

void initPWM() { 
	// Configuro pines como salida
	DDRB |= (1<<PB1)|(1<<PB2)|(1<<PB5);

	// Configuro Timer1 para PWM en PB1 (OC1A) y PB2 (OC1B)
	// Modo Fast PWM, 8-bit (Modo 5)
	TCCR1A = (1<<COM1A1)|(1<<COM1A0)|(1<<COM1B1)|(1<<COM1B0)|(1<<WGM10);  // Modo invertido
	TCCR1B = (1<<WGM12)|(1<<CS11)|(1<<CS10);      // Prescaler 64, Fast PWM 8-bit

	// Valores iniciales (LED apagado)
	OCR1A = 0;  // PB1 (Blue) - 0 = apagado en modo invertido
	OCR1B = 0;  // PB2 (Green) - 0 = apagado en modo invertido

	// Configuro Timer2 para PWM por software en PB5 (más estable que Timer0)
	TCCR2A = (1<<WGM21);  // CTC mode
	TCCR2B = (1<<CS21);  // Prescaler 8
	OCR2A = 155;  // Para aprox 12kHz con prescaler 8
	TIMSK2 = (1<<OCIE2A);  // Habilito compare match interrupt

	// Inicializo PB5 en estado apagado (HIGH para ánodo común)
	PORTB |= (1<<PB5);
 }

void PWM_soft_Update(void) {
	static uint8_t PWM_position = 0;
	if (++PWM_position >= PWM_PERIOD) PWM_position = 0;

	if (PWM_position < PWM_DELTA) PWM_ON;
	else PWM_OFF;
}