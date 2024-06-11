#ifndef _LEDS_H_
#define _LEDS_H_
#include <stdbool.h>

#define LED_ON true
#define LED_OFF false

extern const struct gpio_dt_spec LED_GREEN;
extern const struct gpio_dt_spec LED_RED;

bool led_init(const struct gpio_dt_spec* led, bool on);
bool led_toggle(const struct gpio_dt_spec* led);
bool led_on(const struct gpio_dt_spec* led);
bool led_off(const struct gpio_dt_spec* led);

#endif
