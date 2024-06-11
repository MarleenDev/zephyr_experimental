#include <stdio.h>
#include <zephyr/kernel.h>
#include "leds.h"

#define SLEEP_TIME_MS 500

int main(void)
{
    if (!led_init(&LED_GREEN, LED_ON) || !led_init(&LED_RED, LED_ON))
    {
        return -1;
    }

	while (1) {
        //led_on(&LED_GREEN);
		k_msleep(SLEEP_TIME_MS);
        //led_off(&LED_GREEN);
		k_msleep(SLEEP_TIME_MS);
        led_toggle(&LED_GREEN);
	}
	return 0;
}
