#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/device.h>

#include "buttons.h"
#include "leds.h"

#include <stdio.h>

#define SLEEP_TIME_MS 500

void button_pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
    printk("Button pressed at %" PRIu32 "\n", k_cycle_get_32());
    led_toggle(&LED_RED);
}

bool board_init(void)
{
    // leds
    if (!led_init(&LED_GREEN, LED_OFF) || !led_init(&LED_RED, LED_OFF)) {
        return false;
    }
    // button
    if (!button_init(&BUTTON, button_pressed)) {
        return false;
    }
    return true;
}

int main(void)
{
    if (!board_init()) {
        return -1;
    }

    while (1) {
      k_msleep(SLEEP_TIME_MS);
      led_toggle(&LED_GREEN);
    }
    return 0;
}
