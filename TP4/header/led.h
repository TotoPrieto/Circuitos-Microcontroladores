#ifndef LED_H
#define LED_H

#include <stdint.h>

void setColor(uint8_t red, uint8_t green, uint8_t blue);
void applyBrightness(uint8_t brightness_percent);
void mostrarComandos(void);
void processCommand(char command);

#endif