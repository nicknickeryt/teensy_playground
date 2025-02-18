#include "blink.h"

#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/kernel.h>

#include "led.h"

static const struct pwm_dt_spec pwmLeds = PWM_DT_SPEC_GET(DT_NODELABEL(leds));

void blinkAmber() {
    initAmberLed();

    while (1) {
        setAmberLed();
        k_msleep(500);
        resetAmberLed();
        k_msleep(500);
    }
}

void pwmSmoothFade() {
    if (!pwm_is_ready_dt(&pwmLeds)) {
        printk("Error: PWM device %s is not ready\n", pwmLeds.dev->name);
        return;
    }

    uint32_t pulse_width = 0;
    bool increasing = true;

    while (1) {
        if (increasing) {
            pulse_width += 100000;
            if (pulse_width >= 20000000) increasing = false;
        } else {
            pulse_width -= 100000;
            if (pulse_width <= 0) increasing = true;
        }

        int err = pwm_set_pulse_dt(&pwmLeds, pulse_width);
        if (err) {
            printk("Error in pwm_set_dt(), err: %d", err);
            return;
        }

        k_msleep(10);
    }
}