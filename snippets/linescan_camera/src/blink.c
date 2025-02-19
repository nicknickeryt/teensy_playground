#include "blink.h"

#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/kernel.h>

#include "led.h"


void blinkAmber() {
    initAmberLed();

    while (1) {
        setAmberLed();
        k_msleep(500);
        resetAmberLed();
        k_msleep(500);
    }
}