#include "led.h"
#include "uart.h"
#include "pwm.h"

#include <avr/io.h>

static uint8_t base_red = 0, base_green = 0, base_blue = 0;
static uint8_t current_brightness_percent = 100;

void setColor(uint8_t red, uint8_t green, uint8_t blue) {
	base_red = red;
	base_green = green;
	base_blue = blue;
	applyBrightness(current_brightness_percent);
}

void applyBrightness(uint8_t brightness_percent) {
	if(brightness_percent > 100) brightness_percent = 100;
	current_brightness_percent = brightness_percent;

	OCR1A = (base_blue * brightness_percent) / 100;
	OCR1B = (base_green * brightness_percent) / 100;
	PWM_DELTA = (base_red * brightness_percent) / 100;
}

void mostrarComandos() {
	UART_SendString("Lista de colores LED\r\n"
	"1 - RED\r\n"
	"2 - GREEN\r\n"
	"3 - BLUE\r\n"
	"4 - CIAN\r\n"
	"5 - AMARILLO\r\n"
	"6 - MAGENTA\r\n"
	"7 - WHITE\r\n"
	"8 - BLACK\r\n"
	"Seleccione: ");
}

void processCommand(char command) {
	switch(command){
		case '1': setColor(255,0,0); UART_SendString("\r\nRED seleccionado\r\n"); break;
		case '2': setColor(0,255,0); UART_SendString("\r\nGREEN seleccionado\r\n"); break;
		case '3': setColor(0,0,255); UART_SendString("\r\nBLUE seleccionado\r\n"); break;
		case '4': setColor(0,255,255); UART_SendString("\r\nCIAN seleccionado\r\n"); break;
		case '5': setColor(255,255,0); UART_SendString("\r\nAMARILLO seleccionado\r\n"); break;
		case '6': setColor(255,0,255); UART_SendString("\r\nMAGENTA seleccionado\r\n"); break;
		case '7': setColor(255,255,255); UART_SendString("\r\nWHITE seleccionado\r\n"); break;
		case '8': setColor(0,0,0); UART_SendString("\r\nBLACK seleccionado\r\n"); break;
		default: UART_SendString("\r\nComando invalido\r\n"); break;
	}
	UART_SendString("Seleccione: ");
}