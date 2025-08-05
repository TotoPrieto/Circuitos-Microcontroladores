#include "i2c.h"
#include <avr/io.h>

void i2c_init(void)
{
	TWSR = 0x00;        // Prescaler en 1 (bits = 00)
	TWBR = 152;         // TWBR formula: SCL freq = F_CPU / (16 + 2TWBRPrescaler)
	TWCR = 0x04;        // Habilita TWI (bit TWEN)
}

void i2c_start(void)
{
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);  // Enviar START
	while ((TWCR & (1 << TWINT)) == 0);                // Esperar a que se complete
}

void i2c_write(unsigned char data)
{
	TWDR = data;                                         // Cargar el dato a enviar
	TWCR = (1 << TWINT) | (1 << TWEN);                   // Iniciar transmisión
	while ((TWCR & (1 << TWINT)) == 0);                  // Esperar a que finalice
}

unsigned char i2c_read(unsigned char isLast)
{
	if (isLast == 0)                                     // Si hay más datos por recibir, enviar ACK
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA); // Habilitar recepción y enviar ACK
	else                                                 // Último dato: enviar NACK
	TWCR = (1 << TWINT) | (1 << TWEN);               // Habilitar recepción sin ACK

	while ((TWCR & (1 << TWINT)) == 0);                  // Esperar a que finalice la recepción
	return TWDR;                                         // Devolver el dato recibido
}

void i2c_stop()
{
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);  // Enviar condición STOP
}