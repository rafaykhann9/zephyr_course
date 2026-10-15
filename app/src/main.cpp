#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

/* Custom extension API implemented in drivers/our_driver/our_driver.c */
extern "C" int our_driver_set_count(const struct device *dev, uint32_t count);

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

int main(void)
{
    const struct device *dev = DEVICE_DT_GET_ANY(our_driver);
    struct sensor_value val;

    if (!device_is_ready(dev)) {
        LOG_ERR("LED sensor not ready");
        return -ENODEV;
    }

    while (1) {
        sensor_sample_fetch(dev);                        /* LED on */
        LOG_INF("LED on");
        k_msleep(CONFIG_APP_HEARTBEAT_PERIOD_MS);

        sensor_channel_get(dev, SENSOR_CHAN_ALL, &val);  /* LED off */
        LOG_INF("LED off (state was %d, fetch count %d)", val.val1, val.val2);
        k_msleep(CONFIG_APP_HEARTBEAT_PERIOD_MS);

        if (val.val2 >= 5) {
            our_driver_set_count(dev, 0);                /* custom API */
            LOG_INF("Fetch count reset");
        }
    }
    return 0;
}
