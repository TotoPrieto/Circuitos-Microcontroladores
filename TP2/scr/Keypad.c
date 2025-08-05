/*
 * Keypad.c
 *
 * Created: 5/10/2025 2:07:30 PM
 *  Author: tobis
 */ 

#include <Keypad.h>
#include <avr/io.h>
#include <stdint.h>

static const uint8_t row_port[4] = { 'B','B','B','D' };
static const uint8_t row_pin [4] = { PB4,   PB3,   PB0,   PD7   };
static const uint8_t col_port[4] = { 'D','D','D','D' };
static const uint8_t col_pin [4] = { PD3,   PD5,   PD4,   PD2   };


static void set_port_logic0(char port, uint8_t pin){
	if (port == 'B'){
		PORTB &= ~(0x01<<pin);
		}else{
		PORTD &= ~(0x01<<pin);
	}
}

static void set_port_logic1(char port, uint8_t pin){
	if (port == 'B'){
		PORTB |= (0x01<<pin);
		}else{
		PORTD |= (0x01<<pin);
	}
}

static void set_dir_logic0(char port, uint8_t pin){
	if (port == 'B'){
		DDRB &= ~(0x01<<pin);
		}else{
		DDRD &= ~(0x01<<pin);
	}
}

static void set_dir_logic1(char port, uint8_t pin){
	if (port == 'B'){
		DDRB |= (0x01<<pin);
		}else{
		DDRD |= (0x01<<pin);
	}
}

static uint8_t read_pin(char port, uint8_t pin){
	if (port == 'B'){
		return !(PINB&(1<<pin));
		}else{
		return !(PIND&(1<<pin));
	}
}

uint8_t KeypadUpdate(void)
{
    uint8_t r, c,i;
	
	//  PULL-UP en las 4 columnas
	/*PORTD |= (1<<PD3) | (1<<PD5) | (1<<PD4) | (1<<PD2);*/
	for(i = 0; i < 4; i++){
		set_port_logic1(col_port[i],col_pin[i]);/**/
	}
    

    for (c = 0; c < 4; c++)          // Recorremos fila por fila 
    {
        //Reseteamos DDR  TODO en entrada
		//DDRB &= ~((1<<PB4)|(1<<PB3)|(1<<PB0));          // filas PB en IN
        //DDRD &= ~((1<<PD7)|(1<<PD3)|(1<<PD5)|(1<<PD4)|(1<<PD2)); // todo PD en IN
		for(i = 0; i<4;i++){
			set_dir_logic0(col_port[i],col_pin[i]);/**/
			set_dir_logic0(row_port[i],row_pin[i]);/**/
		}   
        
        /* Dejo solo la fila c como salida LOW
         *    – PORTD &= ~mask  ? la fuerza a ‘0’
         *    – DDRD  |= mask   ? la hace salida                       */
		set_port_logic0(row_port[c],row_pin[c]);
		set_dir_logic1(row_port[c],row_pin[c]);
        //Escaneamos las 4 columnas chequeando si alguna entrada va a 0  
        for (r = 0; r < 4; r++)
        {
            if ( read_pin(col_port[r],col_pin[r]) ) return r + 4*c;
        }
    }
	return 0XFF;
	
	}
	

/* Devuelve 1 si hay tecla nueva; 0 si no */
uint8_t KEYPAD_Scan (uint8_t *pkey)
{
	static uint8_t Old_key, Last_valid_key=0xFF; // no hay tecla presionada;
	uint8_t Key;
	
	Key= KeypadUpdate();
	if(Key==0xFF){
		Old_key=0xFF;	// no hay tecla presionada
		Last_valid_key=0xFF;
		return 0;
	}
	if(Key==Old_key) { //2da verificación
		if(Key!=Last_valid_key){ //evita múltiple detección
			*pkey=Key;
			Last_valid_key = Key;
			return 1;
		}
	}
	Old_key=Key; //1era verificación
	return 0;
}
