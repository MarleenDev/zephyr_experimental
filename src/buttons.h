#ifndef _BUTTONS_H_
#define _BUTTONS_H_
#include <zephyr/drivers/gpio.h>
#include <inttypes.h>
#include <stdbool.h>

extern const struct gpio_dt_spec BUTTON;

typedef void (*button_callback)(const struct device *, struct gpio_callback *, uint32_t);

bool button_init(const struct gpio_dt_spec* button, button_callback bcb);

#endif
