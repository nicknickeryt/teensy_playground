#include "servo.h"

#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/kernel.h>

static const struct pwm_dt_spec pwmServo = PWM_DT_SPEC_GET(DT_NODELABEL(servo));

void setupServo() {
    if (!pwm_is_ready_dt(&pwmServo)) {
        printk("Error: PWM device %s is not ready\n", pwmServo.dev->name);
        return;
    }
}

void setServo(float degrees) {
    // Ograniczenie zakresu do -45° .. 45°
    if (degrees < -45) degrees = -45;
    else if (degrees > 45) degrees = 45;

    int32_t pulseWidth = (SERVO_PWM_RANGE / 180.0f) * degrees + SERVO_PWM_ZERO;

    servoPulseSet(pulseWidth);
}

void servoPulseSet(int32_t pulseWidth) {
    int32_t err = pwm_set_pulse_dt(&pwmServo, pulseWidth);
    if (err) {
        printk("Error in pwm_set_dt(), err: %d\n", err);
        return;
    }
}

void dummyServoProc() {
    while (1) {
        setServo(0);
        k_msleep(1000);
        setServo(-45);
        k_msleep(1000);
        setServo(45);
        k_msleep(1000);
    }
}