#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#define DT_DRV_COMPAT our_driver
LOG_MODULE_REGISTER(our_driver, CONFIG_SENSOR_LOG_LEVEL);

struct our_driver_config {
    struct gpio_dt_spec led; /* read-only, from DTS */
};

struct our_driver_data {
    int led_state; /* runtime state: 1 = on, 0 = off */
};

/* sample_fetch: turn the LED on */
static int our_driver_sample_fetch(const struct device *dev,
                                   enum sensor_channel chan)
{
    const struct our_driver_config *cfg = dev->config;
    struct our_driver_data *data = dev->data;

    data->led_state = 1;
    return gpio_pin_set_dt(&cfg->led, 1);
}

/* channel_get: return the LED state, then turn the LED off */
static int our_driver_channel_get(const struct device *dev,
                                  enum sensor_channel chan,
                                  struct sensor_value *val)
{
    const struct our_driver_config *cfg = dev->config;
    struct our_driver_data *data = dev->data;

    val->val1 = data->led_state;
    val->val2 = 0;

    data->led_state = 0;
    return gpio_pin_set_dt(&cfg->led, 0);
}

static DEVICE_API(sensor, our_driver_api) = {
    .sample_fetch = our_driver_sample_fetch,
    .channel_get  = our_driver_channel_get,
};

static int our_driver_init(const struct device *dev)
{
    const struct our_driver_config *cfg = dev->config;

    if (!gpio_is_ready_dt(&cfg->led)) {
        return -ENODEV;
    }

    return gpio_pin_configure_dt(&cfg->led, GPIO_OUTPUT_INACTIVE);
}

#define OUR_DRIVER_DEFINE(inst)                                 \
    static struct our_driver_data data_##inst;                  \
    static const struct our_driver_config cfg_##inst = {        \
        .led = GPIO_DT_SPEC_INST_GET(inst, led_gpios),          \
    };                                                          \
    DEVICE_DT_INST_DEFINE(inst,                                 \
        our_driver_init, NULL,                                  \
        &data_##inst, &cfg_##inst,                              \
        POST_KERNEL, CONFIG_SENSOR_INIT_PRIORITY,               \
        &our_driver_api);

DT_INST_FOREACH_STATUS_OKAY(OUR_DRIVER_DEFINE)
