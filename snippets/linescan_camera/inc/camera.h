#ifndef __CAMERA_H
#define __CAMERA_H

#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>

#include <stdint.h>

#define CAMERA_SI_GPIO_PORT GPIO_PORT_6
#define CAMERA_SI_GPIO_PIN 2

#define CAMERA_CLK_GPIO_PORT GPIO_PORT_6
#define CAMERA_CLK_GPIO_PIN 3

void setupCamera();
void setCLKPin();
void resetCLKPin();

void dummyCameraProc();

#endif /* __CAMERA_H */