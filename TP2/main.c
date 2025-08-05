#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <lcd.h>
#include <Keypad.h>
#include <time.h>
#include <stdlib.h>
#include <MEF.h>
#include <Timer.h>
#include <avr/interrupt.h>

int main(void) {

	init_timer0_ctc();   // Inicializa el temporizador
	MEF_init();
	sei();               // Habilita interrupciones globales

	while (1) {
		// El trabajo principal se hace en la ISR
		
		if(get_flagMEF()){
			MEF_update();
		}
	}
}