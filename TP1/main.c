#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>


int main (void)
{
	/* Setup */
	int modo = 0; // variable indica secuencia de encendido (0, 1 o 2)
	int cambio = 1; // indica cambio de una secuencia
	int sentido = 0; // indica el sentido actual en movimiento de luces en modo 3
	int anterior = 0; // variable que indica estado anterior de boton 1
	DDRD = 0xFF; // Configuro Puerto D como salida
	DDRB |= (1<<PORTB4 | 1<<PORTB3); // Configuro bit3 y bit4 del puerto B como salida
	DDRC &= ~(1<<PORTC0 | 1<<PORTC1); // Configuro bit0 y bit1 del puerto C como entrada
	PORTC |= (1<<PORTC0 | 1<<PORTC1); // Habilito Pull-Up en bit0 puerto C
	

	/* Loop */
	while(1)
	{
		if (~PINC & (1<<PINC1)){ // se apreta el boton 2
			switch (modo){		// Al oprimir el boton 2, muestra modo actual en portB
				case 0:
				PORTB |= (1<<PORTB3);
				break;
				case 1:
				PORTB |= (1<<PORTB4);
				break;
				case 2:
				PORTB = (1<<PORTB4 |  1<<PORTB3);
				break;
				}
		}else{
			PORTB &= ~(1<<PORTB3 | 1<<PORTB4); // Reinicia valores portB
		}	
			
		if (~PINC & (1<<PINC0))  // se oprime boton 1
		{
			if (anterior == 0){ // recien se oprime
				anterior = 1;
			}
		}
		if (PINC & (1<<PINC0)){ // se suelta el boton 1
			if (anterior == 1){ 
				anterior = 0;	
				modo = (modo+1)%3;  //cambia al siguiente modo
				sentido = 0;		//reinicio variables
				cambio = 1;
			}
		}
		
		switch(modo) {
			case 0 :
			if (cambio == 1) {		// al cambiar modo, inicializa en leds pares
				PORTD = 0b10101010;
				cambio = 0;
				} else {
				PORTD = ~PORTD; // Niego los bits
			}
			break;
			
			case 1 :
			if(cambio == 1){
				cambio = 0;
				PORTD = 0b00000001;		// Inicializo a modo 2
				}else{
				if(PORTD == 0b10000000){
					PORTD = 0b00000001; // Reinicio el patron al llegar al final
					}else{
					PORTD = PORTD<<1; // Desplazo un bit
				}
			}
			break;
			
			case 2 :
			if(cambio == 1){
				cambio = 0;
				PORTD = 0b10000000;  //Inicializo modo 3
				}else{
				if(PORTD == 0b10000000){  //Cambio el sentido al llegar a un extremo
					sentido=0;
					}else if (PORTD == 0b00000001){
					sentido=1;
				}
				if (sentido == 0){
					PORTD = PORTD>>1; // Desplazo un bit a la derecha
					}else {
					PORTD = PORTD<<1; // Desplazo un bit a la izquierda
				}
			}
		
			break;	
		}
		_delay_ms(100);
	}
	return 0;
}