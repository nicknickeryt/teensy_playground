#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#include "adc.h"
#include "blink.h"
#include "camera.h"
#include "gpio.h"
#include "led.h"
#include "usb.h"

#define STACKSIZE 4096
#define CAMERA_PROC_PRIORITY 7
#define CAMERA_LOG_PRIORITY 3
#define BLINK_AMBER_PRIORIY 1

int main() {
    setupGPIO();
    initAmberLed();
    setupUSB();
    setupAdc();
    setupCamera();

    printk("NXP says hello <3\n");
    uint32_t freq = CLOCK_GetCpuClkFreq();
    printk("CPU Freq: %d\n", freq);
}

K_THREAD_DEFINE(dummyCameraProc_id, STACKSIZE, dummyCameraProc, NULL, NULL,
                NULL, CAMERA_PROC_PRIORITY, 0, 0);

K_THREAD_DEFINE(periodCameraLog_id, STACKSIZE, periodCameraLog, NULL, NULL,
                NULL, CAMERA_LOG_PRIORITY, 0, 0);

K_THREAD_DEFINE(blinkAmber_id, STACKSIZE, blinkAmber, NULL, NULL, NULL,
                BLINK_AMBER_PRIORIY, 0, 0);

// K_THREAD_DEFINE(pwmSmoothFade_id, STACKSIZE, pwmSmoothFade, NULL, NULL, NULL,
//                 PRIORITY, 0, 0);

// K_THREAD_DEFINE(adcProc_id, STACKSIZE, adcProc, NULL, NULL, NULL, PRIORITY,
// 0,
//                 0);