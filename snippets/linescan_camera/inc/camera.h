#ifndef __CAMERA_H
#define __CAMERA_H

#include <stdint.h>
#include <stdarg.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>

#define CAMERA_SI_GPIO_PORT GPIO_PORT_6
#define CAMERA_SI_GPIO_PIN 2

#define CAMERA_CLK_GPIO_PORT GPIO_PORT_6
#define CAMERA_CLK_GPIO_PIN 3

#define DELAY_US 20
#define ADC_SAMPLES 128
#define CAMERA_ADC_CHANNEL 0

#define CAMERA_MSG_LEN sizeof(bufArr)
#define CAMERA_MSG_ARRAY_SIZE 4

void setupCamera();
void setCLKPin();
void resetCLKPin();

void dummyCameraProc();

void periodCameraLog();

#endif /* __CAMERA_H */