#include <zephyr/drivers/sensor.h>
#include <zephyr/shell/shell.h>

/* LED sensor from app.overlay (compatible = "our,driver") */
static const struct device *const dev = DEVICE_DT_GET_ANY(our_driver);

/* our_driver fetch: calls sensor_sample_fetch() -> LED on */
static int cmd_sensor_fetch(const struct shell *sh, size_t argc, char **argv)
{
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);

    int ret = sensor_sample_fetch(dev);

    if (ret < 0) {
        shell_error(sh, "sample_fetch failed (%d)", ret);
        return ret;
    }

    shell_print(sh, "Sample fetched (LED on)");
    return 0;
}

/* our_driver read: calls sensor_channel_get() and prints the result -> LED off */
static int cmd_sensor_read(const struct shell *sh, size_t argc, char **argv)
{
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);

    struct sensor_value val;
    int ret = sensor_channel_get(dev, SENSOR_CHAN_ALL, &val);

    if (ret < 0) {
        shell_error(sh, "channel_get failed (%d)", ret);
        return ret;
    }

    shell_print(sh, "LED state: %d, fetch count: %d (LED now off)", val.val1, val.val2);
    return 0;
}

/* our_driver info: prints the device name and ready state */
static int cmd_sensor_info(const struct shell *sh, size_t argc, char **argv)
{
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);

    shell_print(sh, "Device: %s", dev->name);
    shell_print(sh, "Ready:  %s", device_is_ready(dev) ? "yes" : "no");
    return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(sub_our_driver,
    SHELL_CMD(fetch, NULL, "Fetch a sample (LED on).", cmd_sensor_fetch),
    SHELL_CMD(read,  NULL, "Read the sample and print it (LED off).", cmd_sensor_read),
    SHELL_CMD(info,  NULL, "Show device name and ready state.", cmd_sensor_info),
    SHELL_SUBCMD_SET_END
);

SHELL_CMD_REGISTER(our_driver, &sub_our_driver, "LED sensor commands", NULL);
