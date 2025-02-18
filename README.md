# Teensy 4.1 Zephyr playground
Contains various examples that I've made to learn myself Zephyr RTOS using Teensy 4.1

## Building
Say we want to build adc_12bit_and_pwm:
```
cd snippets/adc_12bit_and_pwm
source ~/zephyrproject/zephyr/zephyr-env.sh
west build
```
'teensy41' target board should be automatically set when using `west`.

## Flashing
```
west flash
```
Note: Teensy 4.1 must be in bootloader mode!
