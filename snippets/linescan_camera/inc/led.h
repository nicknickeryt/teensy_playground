#ifndef __LED_H
#define __LED_H

#include "gpio.h"

#define AMBER_LED_GPIO_PORT GPIO_PORT_2
#define AMBER_LED_GPIO_PIN 3

void initAmberLed();

void setAmberLed();
void resetAmberLed();

#endif /* __LED_H */