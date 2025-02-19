#include "camera.h"

#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>

#include "adc.h"
#include "gpio.h"

uint32_t bufArr[ADC_SAMPLES];
K_MSGQ_DEFINE(camera_dat_msgq, CAMERA_MSG_LEN, CAMERA_MSG_ARRAY_SIZE, 4);

void setupCamera() {
    setupGPIO();
    configurePin(CAMERA_SI_GPIO_PORT, CAMERA_SI_GPIO_PIN, GPIO_OUTPUT_ACTIVE);
    configurePin(CAMERA_CLK_GPIO_PORT, CAMERA_CLK_GPIO_PIN, GPIO_OUTPUT_ACTIVE);

    setPin(CAMERA_SI_GPIO_PORT, CAMERA_SI_GPIO_PIN);
    setPin(CAMERA_CLK_GPIO_PORT, CAMERA_CLK_GPIO_PIN);
}

void setSIPin() { setPin(CAMERA_SI_GPIO_PORT, CAMERA_SI_GPIO_PIN); }
void resetSIPin() { resetPin(CAMERA_SI_GPIO_PORT, CAMERA_SI_GPIO_PIN); }

void setCLKPin() { setPin(CAMERA_CLK_GPIO_PORT, CAMERA_CLK_GPIO_PIN); }

void resetCLKPin() { resetPin(CAMERA_CLK_GPIO_PORT, CAMERA_CLK_GPIO_PIN); }

void dummyCameraProc() {
    uint32_t buf = 0;

    struct adc_sequence_options adc_options = {.interval_us = 0};

    struct adc_sequence sequence = {
        .buffer = &buf,
        /* buffer size in bytes, not number of samples */
        .buffer_size = sizeof(buf),
        // Optional
        //.calibrate = true,
        .oversampling = 0,
        .options = &adc_options

    };

    int err =
        adc_sequence_init_dt(&adc_channels[CAMERA_ADC_CHANNEL], &sequence);
    if (err < 0) {
        printk("Could not initalize sequnce");
        return;
    }

    while (1) {
        bool initialSI = 1;
        resetCLKPin();
        setSIPin();

        for (unsigned int i = 0; i < ADC_SAMPLES; i++) {
            k_usleep(DELAY_US);

            setCLKPin();
            if (initialSI) {
                initialSI = 0;
                k_usleep(DELAY_US);  // this is necessary!
                resetSIPin();
            }

            err = adc_read(adc_channels[CAMERA_ADC_CHANNEL].dev, &sequence);
            if (err < 0) {
                printk("Could not read (%d)\n", err);
                return;
            }
            k_usleep(DELAY_US);
            resetCLKPin();

            bufArr[i] = buf;
        }

        // Extra 128th clock
        k_usleep(DELAY_US);
        setCLKPin();
        k_usleep(DELAY_US);
        resetCLKPin();

        k_msgq_put(&camera_dat_msgq, &bufArr, K_NO_WAIT);
    }
}

void periodCameraLog() {
    while (1) {
        printk("[CAMERA-LOG] ADC readings:\n");
        uint32_t cameraBufferArr[ADC_SAMPLES];
        char logBuffer[ADC_SAMPLES * 12];  // 12 znaków na liczbę (przybliżenie)

        k_msgq_get(&camera_dat_msgq, &cameraBufferArr, K_NO_WAIT);

        size_t offset = 0;
        for (unsigned int i = 0; i < ADC_SAMPLES; i++) {
            int32_t val_mv;

            val_mv = adc_channels[CAMERA_ADC_CHANNEL].channel_cfg.differential
                         ? (int32_t)((int32_t)cameraBufferArr[i])
                         : (int32_t)cameraBufferArr[i];

            int32_t err = adc_raw_to_millivolts_dt(
                &adc_channels[CAMERA_ADC_CHANNEL], &val_mv);
            offset +=
                err < 0
                    ? snprintf(logBuffer + offset, sizeof(logBuffer) - offset,
                               "(value in mV not available)\n")
                    : snprintf(logBuffer + offset, sizeof(logBuffer) - offset,
                               "%" PRId32 "\n", val_mv);
        }

        printk("%s\n", logBuffer);
        k_msleep(1000);
    }
}
