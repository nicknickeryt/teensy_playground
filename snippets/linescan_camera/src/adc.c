#include <zephyr/device.h>
#include <zephyr/drivers/adc.h>

#include "adc.h"
#include "led.h"

const struct adc_dt_spec adc_channels[] = {
    DT_FOREACH_PROP_ELEM(DT_PATH(zephyr_user), io_channels, DT_SPEC_AND_COMMA)};

void setupAdc() {
    if (!adc_is_ready_dt(&adc_channels[0])) {
        printk("ADC controller device %s not ready\n",
               adc_channels[0].dev->name);
        return;
    }

    int err = adc_channel_setup_dt(&adc_channels[0]);
    if (err < 0) {
        printk("Could not setup channel #%d (%d)", 0, err);
        return;
    }
}

void adcProc() {
    int64_t buf = 0;
    struct adc_sequence sequence = {
        .buffer = &buf,
        /* buffer size in bytes, not number of samples */
        .buffer_size = sizeof(buf),
        // Optional
        //.calibrate = true,
        .oversampling = 5
    };

    int err = adc_sequence_init_dt(&adc_channels[0], &sequence);
    if (err < 0) {
        printk("Could not initalize sequnce");
        return;
    }

    while (1) {
        printk("[ADC] Read ADC1\n");

        err = adc_read(adc_channels[0].dev, &sequence);
        if (err < 0) {
            printk("Could not read (%d)\n", err);
            return;
        }

        int32_t val_mv;

        if (adc_channels[0].channel_cfg.differential)
            val_mv = (int32_t)((int32_t)buf);
        else
            val_mv = (int32_t)buf;

        printk("%" PRId32, val_mv);
        err = adc_raw_to_millivolts_dt(&adc_channels[0], &val_mv);
        if (err < 0)
            printk(" (value in mV not available)\n");

        else
            printk(" = %" PRId32 " mV\n", val_mv);

        #ifdef AMBER_LED_1V1_EN
            val_mv > 1100 ? setAmberLed() : resetAmberLed();
        #endif

        k_msleep(ADC_SLEEP_MS);
    }
}