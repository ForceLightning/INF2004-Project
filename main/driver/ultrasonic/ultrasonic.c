#include "pico/stdlib.h"
#include <stdio.h>
#include "hardware/gpio.h"
#include "hardware/timer.h"

#define TRIG_PIN 0
#define ECHO_PIN 1

int timeout = 26100;

static absolute_time_t start_time;
static absolute_time_t end_time;
static uint64_t pulse_width;
static uint64_t width;

void gpio_callback(uint gpio, uint32_t events) {
    
    if (events == GPIO_IRQ_EDGE_RISE) {
        start_time = get_absolute_time();
        width++;
        if (width > timeout) {
            width = 0;    
        }
    } else if (events == GPIO_IRQ_EDGE_FALL) {
        end_time = get_absolute_time();
        pulse_width = absolute_time_diff_us(start_time, end_time);

    }
}

void setupUltrasonicPins(uint trigPin, uint echoPin)
{
    gpio_init(trigPin);
    gpio_init(echoPin);
    gpio_set_dir(trigPin, GPIO_OUT);
    gpio_set_dir(echoPin, GPIO_IN);
    gpio_set_irq_enabled_with_callback(echoPin, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
}

uint64_t getPulse(uint trigPin, uint echoPin)
{
    gpio_put(trigPin, 1);
    sleep_us(10);
    gpio_put(trigPin, 0);
    width = 0;
    while (!pulse_width) {
        tight_loop_contents();
    }
    return pulse_width;
}

uint64_t getCm(uint trigPin, uint echoPin)
{
    uint64_t pulseLength = getPulse(trigPin, echoPin);
    return pulseLength / 29 / 2;
}

uint64_t getInch(uint trigPin, uint echoPin)
{
    uint64_t pulseLength = getPulse(trigPin, echoPin);
    return (long)pulseLength / 74.f / 2.f;
}