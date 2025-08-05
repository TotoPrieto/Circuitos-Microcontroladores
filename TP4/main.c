#include <avr/io.h>
#include <avr/interrupt.h>
#include "uart.h"
#include "pwm.h"
#include "adc.h"
#include "led.h"

volatile uint8_t Flag_USART_RX = 0;

int main(void) {
	//Inicializaciones
	UART_Init();
	initPWM();
	initADC();

	//Habilitacion interrupciones
	sei();

	//Imprimir menu
	mostrarComandos();
	setColor(0,0,0);

	uint16_t adc_value = 0;
	uint8_t brightness = 100;
	uint8_t last_brightness = 255;
	char received_char;

	while (1) {
		if(Flag_USART_RX){
			//Leo caracter UART
			received_char = UDR0;
			if(received_char >= '1' && received_char <= '8'){
				UART_Transmit(received_char);
				processCommand(received_char);
			}
			Flag_USART_RX = 0;
		}
		
		//Leo potenciometro
		adc_value = readADC();
		brightness = (uint8_t)((adc_value * 100UL) / 1023UL); //Convierto porcentaje de brillo
		//Aplico solo si cambia valor
		if((brightness - last_brightness) != 0){
			applyBrightness(brightness);
			last_brightness = brightness;
		}
	}
	return 0;
}

// Interrupcion para PWM por software en PB5
ISR(TIMER2_COMPA_vect) {
	PWM_soft_Update();
}

//Interrupcion UART
ISR(USART_RX_vect) {
	Flag_USART_RX = 1;
}