#include <stdio.h>
#include "ir_sensor.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"

#define ADC_PIN 26
uint received_adc_val;

int main(){
    stdio_init_all();
    adc_init();

    adc_gpio_init(ADC_PIN);
    gpio_set_dir(ADC_PIN, GPIO_IN);

    adc_select_input(0);

    while(1){
        received_adc_val = adc_read();

        printf("ADC Value: %d\n", received_adc_val);
        sleep_ms(1000);
    }
}