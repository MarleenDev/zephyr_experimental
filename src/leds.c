#include "leds.h"
#include <stdio.h>
#include <zephyr/drivers/gpio.h>

#define LED0_NODE DT_ALIAS(led0)
#define LED1_NODE DT_ALIAS(led1)

const struct gpio_dt_spec LED_GREEN = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
const struct gpio_dt_spec LED_RED = GPIO_DT_SPEC_GET(LED1_NODE, gpios);

bool led_init(const struct gpio_dt_spec* led, bool on)
{
	if (!gpio_is_ready_dt(led)) {
        printf("Error: led GPIO not ready\n");
        return false;
	}
    int ret = gpio_pin_configure_dt(led, (on ? GPIO_OUTPUT_ACTIVE : GPIO_OUTPUT_INACTIVE));
    if (ret < 0) {
        printf("Error: gpio_pin_configure_dt returned %i\n", ret);
        return false;
    }
    return true;
}

bool led_toggle(const struct gpio_dt_spec* led)
{
    int ret = gpio_pin_toggle_dt(led);
    if (ret < 0) {
        printf("Error: gpio_pin_toggle_dt returned %i\n", ret);
        return false;
    }
    return true;
}

bool led_on(const struct gpio_dt_spec* led)
{
    int ret = gpio_pin_set_dt(led, 1);
    if (ret < 0) {
        printf("Error: gpio_pin_set_dt returned %i [led_on]\n]", ret);
        return false;
    }
    return true;
}

bool led_off(const struct gpio_dt_spec* led)
{
    int ret = gpio_pin_set_dt(led, 0);
    if (ret < 0) {
        printf("Error: gpio_pin_set_dt returned %i [led_off]\n", ret);
        return false;
    }
    return true;
}
