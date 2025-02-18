#include "gpio.h"

void setupGPIO() {
    if (!device_is_ready(GPIO_PORT_6)) {
        printk("GPIO (Port 6) device not ready\n");
        return;
    }
    if (!device_is_ready(GPIO_PORT_2)) {
        printk("GPIO (Port 2) device not ready\n");
        return;
    }
    // TODO more ports...
}

void configurePin(const struct device *gpio_ct_dev, uint8_t pin, unsigned int flag) {
    int ret;
    ret = gpio_pin_configure(gpio_ct_dev, pin, flag);

    if (ret != 0) {
        printk("GPIO device error\n");
        return;
    }
}

void setPin(const struct device *gpio_ct_dev, uint8_t pin) {
    int ret = gpio_pin_set_raw(gpio_ct_dev, pin, 1);

    if (ret != 0) {
        printk("err 0\n");
        return;
    }
}
void resetPin(const struct device *gpio_ct_dev, uint8_t pin) {
    int ret = gpio_pin_set_raw(gpio_ct_dev, pin, 0);

    if (ret != 0) {
        printk("err 0\n");
        return;
    }
}
