#include <zephyr/kernel.h>
#include "buttons.h"
#include <stdio.h>

#define SW0_NODE	DT_ALIAS(sw0)

const struct gpio_dt_spec BUTTON = GPIO_DT_SPEC_GET_OR(SW0_NODE, gpios, {0});
static struct gpio_callback button_cb_data;

bool button_init(const struct gpio_dt_spec* button, button_callback bcb)
{
    int ret = 0;

    if (!gpio_is_ready_dt(button)) {
        printf("Error: button device %s is not ready\n", button->port->name);
        return false;
    }

    ret = gpio_pin_configure_dt(button, GPIO_INPUT);
    if (ret != 0) {
        printf("Error %d: failed to configure %s pin %d\n", ret, button->port->name, button->pin);
        return false;
    }

    ret = gpio_pin_interrupt_configure_dt(button, GPIO_INT_EDGE_TO_ACTIVE);
    if (ret != 0) {
        printf("Error %d: failed to configure interrupt on %s pin %d\n", ret, button->port->name, button->pin);
        return false;
    }

    gpio_init_callback(&button_cb_data, bcb, BIT(button->pin));
    ret = gpio_add_callback(button->port, &button_cb_data);
    if (ret != 0) {
        printf("Error %d: failed to add button callback", ret); 
        return false;
    }

    printf("Set up button at %s pin %d\n", button->port->name, button->pin);
    return true;
}
