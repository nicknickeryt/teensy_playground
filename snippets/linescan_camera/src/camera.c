#include "camera.h"

#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>

#include "adc.h"
#include "gpio.h"

void setupCamera() {
    // Here we initialize CLK, SI GPIOs and ADC1.

    setupGPIO();
    configurePin(CAMERA_SI_GPIO_PORT, CAMERA_SI_GPIO_PIN, GPIO_OUTPUT_ACTIVE);
    configurePin(CAMERA_CLK_GPIO_PORT, CAMERA_CLK_GPIO_PIN, GPIO_OUTPUT_ACTIVE);

    setPin(CAMERA_SI_GPIO_PORT, CAMERA_SI_GPIO_PIN);
    setPin(CAMERA_CLK_GPIO_PORT, CAMERA_CLK_GPIO_PIN);
}

// Note: the minimum time for full clock period is 1/8MHz = 125ns
//       So a pulse should be no shorter than 63ns

void setSIPin() { setPin(CAMERA_SI_GPIO_PORT, CAMERA_SI_GPIO_PIN); }
void resetSIPin() { setPin(CAMERA_SI_GPIO_PORT, CAMERA_SI_GPIO_PIN); }

void setCLKPin() { setPin(CAMERA_CLK_GPIO_PORT, CAMERA_CLK_GPIO_PIN); }

void resetCLKPin() { resetPin(CAMERA_CLK_GPIO_PORT, CAMERA_CLK_GPIO_PIN); }

void dummyCameraProc() {
    // printk("Dummy camera proc\n");

    // // First we RESET SLK pin
    // resetCLKPin();
    // // Then we SET SI pin ASAP
    // setSIPin();

    // // Wait minimum pulse time
    // K_NSEC(63);

    // // Then we can SET CLK pin
    // setCLKPin();

    // // Now we start first ADC read
    // int read = readADC();

    // // After we read ADC, we reset release SI pin (I guess?)
    // resetSIPin();

    // // Wait minimum pulse time
    // K_NSEC(63);

    // // Then we can RESET CLK pin
    // resetCLKPin();

    // // Wait minimum pulse time
    // K_NSEC(63);

    // // And the cycle continues...

    // Just for testing....

    #define ADC_SAMPLES 128

    int64_t buf = 0;
    int64_t bufArr[ADC_SAMPLES];
    struct adc_sequence sequence = {
        .buffer = &buf,
        /* buffer size in bytes, not number of samples */
        .buffer_size = sizeof(buf),
        // Optional
        //.calibrate = true,
        .oversampling = 5};

    int err = adc_sequence_init_dt(&adc_channels[0], &sequence);
    if (err < 0) {
        printk("Could not initalize sequnce");
        return;
    }

    while (1) {

        bool initialSI = 1;
        resetCLKPin();
        setSIPin();

        printk("[ADC] Read ADC1...\n");

        for (unsigned int i = 0; i < ADC_SAMPLES; i++) {
            setCLKPin();
            if(initialSI) {
                initialSI = 0;
                resetSIPin();
            }

            err = adc_read(adc_channels[0].dev, &sequence);
            if (err < 0) {
                printk("Could not read (%d)\n", err);
                return;
            }
            resetCLKPin();

            bufArr[i] = buf;
        }

        printk("ADC readings:\n");
        for (unsigned int i = 0; i < ADC_SAMPLES; i++) {
            int32_t val_mv;

            if (adc_channels[0].channel_cfg.differential)
                val_mv = (int32_t)((int32_t)bufArr[i]);
            else
                val_mv = (int32_t)bufArr[i];

            printk("%" PRId32, val_mv);
            err = adc_raw_to_millivolts_dt(&adc_channels[0], &val_mv);
            if (err < 0)
                printk(" (value in mV not available)\n");

            else
                printk(" = %" PRId32 " mV\n", val_mv);
        }
        printk("\n");

    k_usleep(1);

    }
}
