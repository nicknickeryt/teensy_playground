#ifndef __ADC_H
#define __ADC_H

#include <zephyr/device.h>
#include <zephyr/drivers/adc.h>

// #define AMBER_LED_1V1_EN 1 // voltage > 1V1 ? led on : led off
#define ADC_SLEEP_MS 500

#if !DT_NODE_EXISTS(DT_PATH(zephyr_user)) || \
    !DT_NODE_HAS_PROP(DT_PATH(zephyr_user), io_channels)
#error "No suitable devicetree overlay specified"
#endif

#define DT_SPEC_AND_COMMA(node_id, prop, idx) \
    ADC_DT_SPEC_GET_BY_IDX(node_id, idx),

extern const struct adc_dt_spec adc_channels[];

void setupAdc();
void adcProc();

#endif /* __ADC_H */