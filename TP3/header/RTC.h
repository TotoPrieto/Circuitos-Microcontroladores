#include <stdio.h>

#ifndef RTC_H_
#define RTC_H_

unsigned char bcd_to_decimal(unsigned char bcd);
unsigned char decimal_to_bcd(uint8_t decimal);
void leerFechaHoraRTC();
void configurarAlarma1(uint8_t seg, uint8_t min, uint8_t hora);
void establecerHora(uint8_t seg, uint8_t min, uint8_t hora);
uint8_t verificarAlarma();



#endif /* RTC_H_ */