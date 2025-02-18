#ifndef __GPIO_H
#define __GPIO_H

#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>

#include <stdint.h>

#define GPIO_PORT_6 DEVICE_DT_GET(DT_NODELABEL(gpio6))
#define GPIO_PORT_2 DEVICE_DT_GET(DT_NODELABEL(gpio2))

void setupGPIO();

void configurePin(const struct device *gpio_ct_dev, uint8_t pin, unsigned int flag);
void setPin(const struct device *gpio_ct_dev, uint8_t pin);
void resetPin(const struct device *gpio_ct_dev, uint8_t pin);

#endif /* __GPIO_H */