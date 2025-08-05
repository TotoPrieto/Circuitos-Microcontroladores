#include <stdint.h>

#ifndef KEYPAD_H_
#define KEYPAD_H_

static void set_port_logic0(char port, uint8_t pin);
static void set_port_logic1(char port, uint8_t pin);
static void set_dir_logic0(char port, uint8_t pin);
static void set_dir_logic1(char port, uint8_t pin);
static uint8_t read_pin(char port, uint8_t pin);
uint8_t KeypadUpdate(void);
uint8_t KEYPAD_Scan (uint8_t *pkey);



#endif /* KEYPAD_H_ */