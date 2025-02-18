#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>
#include <zephyr/usb/usb_device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/drivers/adc.h>
#include <string.h>

#define STACKSIZE 1024
#define PRIORITY 7

static const struct device *gpio_ct_dev2 = DEVICE_DT_GET(DT_NODELABEL(gpio2));

const struct device *const dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_console));
uint32_t dtr = 0;

static const struct pwm_dt_spec pwmLeds = PWM_DT_SPEC_GET(DT_NODELABEL(leds));

static const struct pwm_dt_spec pwmAdc = PWM_DT_SPEC_GET(DT_NODELABEL(adcpwm));

#if !DT_NODE_EXISTS(DT_PATH(zephyr_user)) || \
    !DT_NODE_HAS_PROP(DT_PATH(zephyr_user), io_channels)
#error "No suitable devicetree overlay specified"
#endif

#define DT_SPEC_AND_COMMA(node_id, prop, idx) \
    ADC_DT_SPEC_GET_BY_IDX(node_id, idx),

/* Data of ADC io-channels specified in devicetree. */
static const struct adc_dt_spec adc_channels[] = {
    DT_FOREACH_PROP_ELEM(DT_PATH(zephyr_user), io_channels,
                         DT_SPEC_AND_COMMA)};

void blinkAmber()
{
    if (!device_is_ready(gpio_ct_dev2))
    {
        printk("GPIO device not ready\n");
        return;
    }

    int ret;
    ret = gpio_pin_configure(gpio_ct_dev2, 3, GPIO_OUTPUT_ACTIVE);

    if (ret != 0)
    {
        printk("GPIO device error\n");
        return;
    }

    while (1)
    {
        int ret = gpio_pin_set_raw(gpio_ct_dev2, 3, 1);
        printk("[BLINKAMBER] LED ON\n");
        if (ret != 0)
        {
            printk("err 0\n");
            return;
        }
        k_msleep(1000);

        ret = gpio_pin_set_raw(gpio_ct_dev2, 3, 0);
        printk("[BLINKAMBER] LED OFF\n");
        if (ret != 0)
        {
            printk("err 1\n");
            return;
        }
        k_msleep(1000);
    }
}

void pwmSmoothFade()
{
    if (!pwm_is_ready_dt(&pwmLeds))
    {
        printk("Error: PWM device %s is not ready\n",
               pwmLeds.dev->name);
        return;
    }

    uint32_t pulse_width = 0;
    bool increasing = true;

    while (1)
    {

        if (increasing)
        {
            pulse_width += 100000;
            if (pulse_width >= 20000000)
            {
                increasing = false;
            }
        }
        else
        {
            pulse_width -= 100000;
            if (pulse_width <= 0)
            {
                increasing = true;
            }
        }

        int err = pwm_set_pulse_dt(&pwmLeds, pulse_width);
        if (err)
        {
            printk("Error in pwm_set_dt(), err: %d", err);
            return;
        }

        k_msleep(10);
    }
}

void pwmSmoothFadePWMADC()
{
    if (!pwm_is_ready_dt(&pwmAdc))
    {
        printk("Error: PWM device %s is not ready\n",
               pwmAdc.dev->name);
        return;
    }

    int err = pwm_set_pulse_dt(&pwmAdc, 10000000);
    if (err)
    {
        printk("Error in pwm_set_dt(), err: %d", err);
        return;
    }
}

int main()
{
    if (usb_enable(NULL))
    {
        return 1;
    }

    printk("NXP says hello <3\n");

    if (!adc_is_ready_dt(&adc_channels[0]))
    {
        printk("ADC controller device %s not ready\n", adc_channels[0].dev->name);
        return 0;
    }

    int err = adc_channel_setup_dt(&adc_channels[0]);
    if (err < 0)
    {
        printk("Could not setup channel #%d (%d)", 0, err);
        return 0;
    }
}

void thread0(void)
{
    int16_t buf;
    struct adc_sequence sequence = {
        .buffer = &buf,
        /* buffer size in bytes, not number of samples */
        .buffer_size = sizeof(buf),
        // Optional
        //.calibrate = true,
    };

    int err = adc_sequence_init_dt(&adc_channels[0], &sequence);
    if (err < 0)
    {
        printk("Could not initalize sequnce");
        return;
    }

    while (1)
    {
        printk("[THREAD0] Hello, I am thread0\n");
        k_msleep(500);

        printk("[THREAD0] READ ADC1\n");

        err = adc_read(adc_channels[0].dev, &sequence);
        if (err < 0)
        {
            printk("Could not read (%d)", err);
            return;
        }

        int32_t val_mv;

        if (adc_channels[0].channel_cfg.differential)
        {
            val_mv = (int32_t)((int16_t)buf);
        }
        else
        {
            val_mv = (int32_t)buf;
        }
        printk("%" PRId32, val_mv);
        err = adc_raw_to_millivolts_dt(&adc_channels[0],
                                       &val_mv);
        /* conversion to mV may not be supported, skip if not */
        if (err < 0)
        {
            printk(" (value in mV not available)\n");
        }
        else
        {
            printk(" = %" PRId32 " mV\n", val_mv);
        }
    }
}

K_THREAD_DEFINE(blinkAmber_id, STACKSIZE, blinkAmber, NULL, NULL, NULL,
                PRIORITY, 0, 0);

K_THREAD_DEFINE(pwmSmoothFade_id, STACKSIZE, pwmSmoothFade, NULL, NULL, NULL,
                PRIORITY, 0, 0);

K_THREAD_DEFINE(pwmSmoothFadePWMADC_id, STACKSIZE, pwmSmoothFadePWMADC, NULL, NULL, NULL,
                PRIORITY, 0, 0);

K_THREAD_DEFINE(thread0_id, STACKSIZE, thread0, NULL, NULL, NULL,
                PRIORITY, 0, 0);