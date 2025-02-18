#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>

#include "led.h"
#include "gpio.h"

void initAmberLed() {
    setupGPIO();
    configurePin(AMBER_LED_GPIO_PORT, AMBER_LED_GPIO_PIN, GPIO_OUTPUT_ACTIVE);
}

void setAmberLed() {
    setPin(AMBER_LED_GPIO_PORT, AMBER_LED_GPIO_PIN);
}
void resetAmberLed() {
    resetPin(AMBER_LED_GPIO_PORT, AMBER_LED_GPIO_PIN);
}