#ifndef UART_H
#define UART_H

void UART_Init(void);
void UART_Transmit(unsigned char data);
void UART_SendString(const char* str);

#endif