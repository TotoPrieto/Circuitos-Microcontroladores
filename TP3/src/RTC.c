#include "RTC.h"

unsigned char bcd_to_decimal(unsigned char bcd) {
	return (bcd >> 4) * 10 + (bcd & 0x0F);
}

unsigned char decimal_to_bcd(uint8_t decimal){
	return ((decimal / 10) << 4) | (decimal % 10);
}

void leerFechaHoraRTC() {
	i2c_start();
	i2c_write(0xD0);      // Dirección del RTC + escritura
	i2c_write(0x00);      // Dirección del primer registro (segundos)
	i2c_start();          // Repeated START
	i2c_write(0xD1);      // Dirección del RTC + lectura

	unsigned char seg  = bcd_to_decimal(i2c_read(0));  // ACK
	unsigned char min  = bcd_to_decimal(i2c_read(0));  // ACK
	unsigned char hora = bcd_to_decimal(i2c_read(0));  // ACK
	i2c_read(0); // Día de la semana (lo descartamos)
	unsigned char dia  = bcd_to_decimal(i2c_read(0));  // ACK
	unsigned char mes  = bcd_to_decimal(i2c_read(0));  // ACK
	unsigned char anio = bcd_to_decimal(i2c_read(1));  // NACK (último byte)
	
	i2c_stop();

	// Armar cadena para mostrar
	char buffer[64];
	sprintf(buffer, "FECHA: %02d/%02d/%02d HORA: %02d:%02d:%02d\r\n", dia, mes, anio, hora, min, seg);
	SerialPort_Send_String(buffer);
}

void configurarAlarma1(uint8_t seg, uint8_t min, uint8_t hora) {
	// PASO 1: Limpiar flag de alarma ANTES de configurar
	i2c_start();
	i2c_write(0xD0);     // Dirección del RTC + escritura
	i2c_write(0x0F);     // Registro de status (0x0F)
	i2c_start();         // Repeated start
	i2c_write(0xD1);     // Lectura
	uint8_t status = i2c_read(1);
	i2c_stop();
	
	// Limpiar solo el bit A1F (bit 0) manteniendo otros bits
	i2c_start();
	i2c_write(0xD0);     // Dirección del RTC + escritura
	i2c_write(0x0F);     // Registro de status
	i2c_write(status & 0xFE);  // Limpiar solo el bit A1F
	i2c_stop();
	
	// PASO 2: Configurar registros de alarma 1 (0x07-0x0A)
	i2c_start();
	i2c_write(0xD0);      // Dirección del RTC + escritura
	i2c_write(0x07);      // Registro 0x07: Alarma 1 Segundos
	
	// Configurar para alarma diaria (solo hora:minuto:segundo)
	// A1M1=0, A1M2=0, A1M3=0, A1M4=1 (ignora día/fecha)
	i2c_write(decimal_to_bcd(seg));           // 0x07: Segundos (A1M1=0)
	i2c_write(decimal_to_bcd(min));           // 0x08: Minutos (A1M2=0)
	i2c_write(decimal_to_bcd(hora));          // 0x09: Hora (A1M3=0)
	i2c_write(0x80);                          // 0x0A: A1M4=1, ignora día
	
	i2c_stop();
	
	// PASO 3: Configurar registro de control (0x0E)
	// Bit 2 (INTCN) = 1: Habilitar interrupción por alarma
	// Bit 0 (A1IE) = 1: Habilitar alarma 1
	// Bit 7 (EOSC) = 0: Oscilador habilitado
	i2c_start();
	i2c_write(0xD0);     // Dirección del RTC + escritura
	i2c_write(0x0E);     // Registro de control
	i2c_write(0x05);     // 00000101: INTCN=1, A1IE=1, otros=0
	i2c_stop();
	
	SerialPort_Send_String("Alarma configurada correctamente\r\n");
}

void establecerHora(uint8_t seg, uint8_t min, uint8_t hora) {
	i2c_start();
	i2c_write(0xD0);      // Dirección del RTC + escritura
	i2c_write(0x00);      // Dirección del primer registro (segundos)
	
	i2c_write(decimal_to_bcd(seg));
	i2c_write(decimal_to_bcd(min));
	i2c_write(decimal_to_bcd(hora));
	
	i2c_stop();
}

uint8_t verificarAlarma() {
	i2c_start();
	i2c_write(0xD0);     // Dirección del RTC + escritura
	i2c_write(0x0F);     // Registro de status (0x0F)
	i2c_start();         // Repeated START
	i2c_write(0xD1);     // Dirección del RTC + lectura
	
	unsigned char status = i2c_read(1);  // NACK
	i2c_stop();
	
	// Verificar flag A1F (bit 0)
	if(status & 0x01) {
		// Limpiar el flag de alarma inmediatamente
		i2c_start();
		i2c_write(0xD0);     // Dirección del RTC + escritura
		i2c_write(0x0F);     // Registro de status
		i2c_write(status & 0xFE);  // Limpiar solo el bit A1F
		i2c_stop();
		
		return 1;  // Alarma detectada
	}
	
	return 0;  // No hay alarma
}
