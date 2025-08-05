#include <avr/io.h>
#include <avr/interrupt.h>
#include <serialPort.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <Timer.h>
#include <i2c.h>
#include <RTC.h>

#define F_CPU 16000000UL

#define MNS_INICIO "Lista de comandos del reloj"
#define COMANDO1 "ON"
#define COMANDO2 "OFF"
#define COMANDO3 "SET TIME"
#define COMANDO4 "SET ALARM"

volatile uint8_t Flag_USART_RX=0;

// Variables para el manejo de la alarma
volatile uint8_t alarma_activada = 0;
volatile uint8_t contador_alarma = 0;

void init_uart(){
	UCSR0B = (1<<TXEN0)|(1<<RXEN0);
	UCSR0C = (1<<UCSZ01)|(1<<UCSZ00);
	UBRR0L = 103;
	UCSR0B |= (1<<RXCIE0);
}

void mostrarComandos(){
	SerialPort_Send_String(MNS_INICIO);
	SerialPort_Send_String("\r\n");
	SerialPort_Send_String(COMANDO1);
	SerialPort_Send_String("\r\n");
	SerialPort_Send_String(COMANDO2);
	SerialPort_Send_String("\r\n");
	SerialPort_Send_String(COMANDO3);
	SerialPort_Send_String("\r\n");
	SerialPort_Send_String(COMANDO4);
	SerialPort_Send_String("\r\n");
}

int chequearComando(char * arreglo,int * comCode){
	if(strcmp(arreglo,COMANDO1)==0){
		*comCode=0;
		return 1;
	}
	if(strcmp(arreglo,COMANDO2)==0){
		*comCode=1;
		return 1;
	}
	if(strcmp(arreglo,COMANDO3)==0){
		*comCode=2;
		return 1;
	}
	if(strcmp(arreglo,COMANDO4)==0){
		*comCode=3;
		return 1;
	}
	return 0;
}

void obtenerCaracter(char* arreglo,int* comCode,int* hayDato){
	static int i=0;
	char dato=0;
	char c = 8;
	while (! (UCSR0A & (1<<RXC0)));
	dato=UDR0;
	while (! (UCSR0A & (1<<UDRE0))); //wait until UDR0 is empty
	if(dato != '\r' && dato != '\r\n'){
		if(dato == c){  // Backspace
			if(i != 0){
				UDR0 = dato; //transmit letter
				i--;
			}
			}else{
			if(dato != '\n'){
				UDR0 = dato; //transmit letter
				arreglo[i]=dato;
				i++;
			}
		}
		}else{
		UDR0 = dato;
		arreglo[i]='\0';
		if(chequearComando(arreglo,comCode)){
			*hayDato=0;
			}else{
			*hayDato=1;
		}
		i=0;
	}
}

void set_time_comando(char* arreglo,int*hayDato,int*comCode){
	static int lecturas=0;
	int valorTiempo=0;
	static uint8_t seg_temp=0, min_temp=0, hora_temp=0;
	if(*hayDato == 0 && lecturas == 0){
		lecturas=3;
		SerialPort_Send_String("Segundos (0-59): ");
	}
	if(*hayDato == 1){
		*hayDato=0;
		if(lecturas == 3){
			sscanf(arreglo, "%d", &valorTiempo);
			if(valorTiempo >= 0 && valorTiempo <= 59){
				seg_temp = valorTiempo;
				lecturas--;
				SerialPort_Send_String("Minutos (0-59): ");
				} else {
				SerialPort_Send_String("Valor invalido. Segundos (0-59): ");
			}
			}else if(lecturas == 2){
			sscanf(arreglo, "%d", &valorTiempo);
			if(valorTiempo >= 0 && valorTiempo <= 59){
				min_temp = valorTiempo;
				lecturas--;
				SerialPort_Send_String("Hora (0-23): ");
				} else {
				SerialPort_Send_String("Valor invalido. Minutos (0-59): ");
			}
			}else if(lecturas == 1){
			sscanf(arreglo, "%d", &valorTiempo);
			if(valorTiempo >= 0 && valorTiempo <= 23){
				hora_temp = valorTiempo;
				lecturas--;
				*comCode=-1;
				establecerHora(seg_temp, min_temp, hora_temp);
				SerialPort_Send_String("Tiempo establecido correctamente\r\n");
				} else {
				SerialPort_Send_String("Valor invalido. Hora (0-23): ");
			}
		}
	}
}

void set_alarm_comando(char* arreglo,int*hayDato,int*comCode){
	static int lecturas=0;
	int valorTiempo=0;
	static uint8_t seg_temp=0, min_temp=0, hora_temp=0;
	if(*hayDato == 0 && lecturas == 0){
		lecturas=3;
		SerialPort_Send_String("Segundos (0-59): ");
	}
	if(*hayDato == 1){
		*hayDato=0;
		if(lecturas == 3){
			sscanf(arreglo, "%d", &valorTiempo);
			if(valorTiempo >= 0 && valorTiempo <= 59){
				seg_temp = valorTiempo;
				lecturas--;
				SerialPort_Send_String("Minutos (0-59): ");
				} else {
				SerialPort_Send_String("Valor invalido. Segundos (0-59): ");
			}
			}else if(lecturas == 2){
			sscanf(arreglo, "%d", &valorTiempo);
			if(valorTiempo >= 0 && valorTiempo <= 59){
				min_temp = valorTiempo;
				lecturas--;
				SerialPort_Send_String("Hora (0-23): ");
				} else {
				SerialPort_Send_String("Valor invalido. Minutos (0-59): ");
			}
			}else if(lecturas == 1){
			sscanf(arreglo, "%d", &valorTiempo);
			if(valorTiempo >= 0 && valorTiempo <= 23){
				hora_temp = valorTiempo;
				lecturas--;
				*comCode=-1;
				configurarAlarma1(seg_temp, min_temp, hora_temp);
				char buffer[64];
				sprintf(buffer, "Alarma configurada: %02d:%02d:%02d\r\n", hora_temp, min_temp, seg_temp);
				SerialPort_Send_String(buffer);
				} else {
				SerialPort_Send_String("Valor invalido. Hora (0-23): ");
			}
		}
	}
}

int main (void) {
	//Inicializa el UART a 9600bps, 8 bit data, 1 stop @ F_CPU = 16MHz.
	init_uart();
	// Inicializar timer
	init_timer1_ctc();
	
	// Habilitar interrupciones globales
	sei();
	
	int timerFlag=0;
	char* arreglo = malloc(20);
	int comCode=-1;
	int hayDato=0;
	
	i2c_init();
	
	// Configuración inicial del RTC - IMPORTANTE: EOSC=0 para habilitar oscilador
	i2c_start();
	i2c_write(0xD0); // Dirección del RTC + escritura
	i2c_write(0x0E); // Registro de control del RTC
	i2c_write(0x04); // INTCN=1, EOSC=0, alarmas deshabilitadas inicialmente
	i2c_stop();
	
	// Limpiar cualquier flag pendiente
	i2c_start();
	i2c_write(0xD0);
	i2c_write(0x0F); // Registro de status
	i2c_write(0x00); // Limpiar todos los flags
	i2c_stop();
	
	mostrarComandos();
	
	while(1){
		timerFlag = get_flagTimer();
		if(Flag_USART_RX){
			obtenerCaracter(arreglo,&comCode,&hayDato);
			Flag_USART_RX = 0;
		}
		
		// Comando ON - mostrar hora cada segundo
		if(comCode==0){
			if(timerFlag){
				leerFechaHoraRTC();
			}
		}
		
		// Comando OFF - detener transmisión
		if(comCode==1){
			// Simplemente cambia el código de comando para detener la transmisión
			comCode = -1;
		}
		
		// Comando SET TIME
		if(comCode==2){
			set_time_comando(arreglo,&hayDato,&comCode);
		}
		
		// Comando SET ALARM
		if(comCode==3){
			set_alarm_comando(arreglo,&hayDato,&comCode);
		}
		
		// Verificar constantemente si se activó la alarma
		if(timerFlag){
			if(verificarAlarma()) {
				alarma_activada = 1;
				contador_alarma = 0;
			}
			// Manejar la alarma activada - mostrar ALARMA 5 veces cada segundo
			if(alarma_activada) {
				if(contador_alarma < 5) {
					SerialPort_Send_String("ALARMA\r\n");
					contador_alarma++;
					if(contador_alarma >= 5) {
						alarma_activada = 0;
					}
				}
			}
		}
		
		timerFlag = 0;
		
	}
	
	return 0;
}

ISR(USART_RX_vect)
{
	Flag_USART_RX = 1;
}