#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/device.h>
#include <zephyr/sys/util.h>

#include "buttons.h"
#include "leds.h"

#include <stdio.h>

#define U_SE_LPS22HH
#define U_SE_HTS221
#define SLEEP_TIME_MS 500

#ifdef USE_LPS22HH
const struct device* const lps22hh_dev = DEVICE_DT_GET_ONE(st_lps22hh);
#endif

#ifdef USE_HTS221
const struct device* const hts221_dev = DEVICE_DT_GET_ONE(st_hts221);
#endif

/**
 * DONE:
 *    Leds
 *    Button
 *    LPS22HH Pressure sensor, 260-1260 hPa absolute digital output barometer
 *    HTS221 Relative humidity and temperature sensor
 * TODO:
 *    usarts
 *    512-Mbit octal-SPI Flash memory, 64-Mbit octal-SPI PSRAM, 256-Kbit I2C EEPROM
 *    usb_fs
 *    wifi
 *    ble
 *    microphones
 *    2 digital microphones
 *    3-axis magnetometer
 *    3D accelerometer and 3D gyroscope
 *    Time-of-flight and gesture-detection sensor
 *    Ambient-light sensor
 *    DAC
 *    ADC
 *    pwm die temperature sensor
 */

#ifdef USE_LPS22HH
static void lps22hh_process_sample()
{
    static unsigned int obs;
    struct sensor_value pressure, temp;

    if (sensor_sample_fetch(lps22hh_dev) < 0) {
        printf("LPS22HH: sensor sample update error\n");
        return;
    }

    if (sensor_channel_get(lps22hh_dev, SENSOR_CHAN_PRESS, &pressure) < 0) {
        printf("LPS22H: cannot read pressure channel\n");
        return;
    }

    if (sensor_channel_get(lps22hh_dev, SENSOR_CHAN_AMBIENT_TEMP, &temp) < 0) {
        printf("LPS22HH: cannot read temperature channel\n");
        return;
    }

    ++obs;
    printf("Observation: %u\n", obs);
    printf("  Pressure: %.3f kPa\n", sensor_value_to_double(&pressure));
    printf("  Temperature: %.2f C\n", sensor_value_to_double(&temp));
}

static void lps22hh_handler(const struct device* dev, const struct sensor_trigger* trig)
{
    lps22hh_process_sample(dev);
}
#endif

#ifdef USE_HTS221
static void hts221_process_sample(const struct device* dev)
{
    static unsigned int obs;
    struct sensor_value temp, hum;
    if (sensor_sample_fetch(dev) < 0) {
        printf("HTS221: sensor sample update error\n");
        return;
    }

    if (sensor_channel_get(dev, SENSOR_CHAN_AMBIENT_TEMP, &temp) < 0) {
        printf("HTS221: cannot read temperature channel\n");
        return;
    }

    if (sensor_channel_get(dev, SENSOR_CHAN_HUMIDITY, &hum) < 0) {
        printf("HTS221: cannot read humidity channel\n");
        return;
    }

    ++obs;
    printf("Observation:%u\n", obs);
    printf("Temperature:%.1f C\n", sensor_value_to_double(&temp));
    printf("Relative Humidity:%.1f%%\n", sensor_value_to_double(&hum));
}

static void hts221_handler(const struct device* dev, const struct sensor_trigger* trig)
{
    hts221_process_sample(dev);
}
#endif

void button_pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
    printk("Button pressed at %" PRIu32 "\n", k_cycle_get_32());

    led_toggle(&LED_RED);

#ifdef USE_LPS22HH
    if (!IS_ENABLED(CONFIG_LPS22HH_TRIGGER)) {
        lps22hh_process_sample();
    }
#endif

#ifdef USE_HTS221
    if (!IS_ENABLED(CONFIG_HTS221_TRIGGER)) {
        hts221_process_sample(hts221_dev);
    }
#endif
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

    // lps22hh
#ifdef USE_LPS22HH
    if (!device_is_ready(lps22hh_dev)) {
        printk("LPS22HH: sensor: device not ready.\n");
        return false;
    }
    if (IS_ENABLED(CONFIG_LPS22HH_TRIGGER)) {
        struct sensor_value attr = {
            .val1 = 1,
        };
        struct sensor_trigger trig = {
            .type = SENSOR_TRIG_DATA_READY,
            .chan = SENSOR_CHAN_ALL,
        };

        if (sensor_attr_set(lps22hh_dev, SENSOR_CHAN_ALL,
                            SENSOR_ATTR_SAMPLING_FREQUENCY, &attr) < 0) {
            printf("LPS22HH: cannot configure sampling rate\n");
            return false;
        }
        if (sensor_trigger_set(lps22hh_dev, &trig, lps22hh_handler) < 0) {
            printf("LPS22HH: cannot configure trigger\n");
            return false;
        }
        printk("Configured for triggered collection at %u Hz\n", attr.val1);
    }
#endif

#ifdef USE_HTS221
    // hts221
    if (!device_is_ready(hts221_dev)) {
        printk("HTS221: sensor: device not ready.\n");
        return false;
    }
    if (IS_ENABLED(CONFIG_HTS221_TRIGGER)) {
        struct sensor_trigger trig = {
            .type = SENSOR_TRIG_DATA_READY,
            .chan = SENSOR_CHAN_ALL,
        };
        if (sensor_trigger_set(hts221_dev, &trig, hts221_handler) < 0) {
            printf("HTS221: cannot configure trigger\n");
            return false;
        }
    }
#endif

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
