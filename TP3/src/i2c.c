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
	TWCR = (1 << TWINT) | (1 << TWEN);                   // Iniciar transmisi�n
	while ((TWCR & (1 << TWINT)) == 0);                  // Esperar a que finalice
}

unsigned char i2c_read(unsigned char isLast)
{
	if (isLast == 0)                                     // Si hay m�s datos por recibir, enviar ACK
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA); // Habilitar recepci�n y enviar ACK
	else                                                 // �ltimo dato: enviar NACK
	TWCR = (1 << TWINT) | (1 << TWEN);               // Habilitar recepci�n sin ACK

	while ((TWCR & (1 << TWINT)) == 0);                  // Esperar a que finalice la recepci�n
	return TWDR;                                         // Devolver el dato recibido
}

void i2c_stop()
{
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);  // Enviar condici�n STOP
}