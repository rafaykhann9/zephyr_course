#include <stdlib.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/shell/shell.h>

#define SET_COUNT_MAX 100

/* Custom extension API implemented in our_driver.c */
int our_driver_set_count(const struct device *dev, uint32_t count);

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

/* our_driver set <value>: calls the extension API our_driver_set_count() */
static int cmd_sensor_set(const struct shell *sh, size_t argc, char **argv)
{
    char *end;
    long value;

    if (argc < 2) {
        shell_error(sh, "Missing value. Usage: our_driver set <0-%d>", SET_COUNT_MAX);
        return -EINVAL;
    }

    value = strtol(argv[1], &end, 10);
    if (*end != '\0') {
        shell_error(sh, "'%s' is not a number", argv[1]);
        return -EINVAL;
    }

    if (value < 0 || value > SET_COUNT_MAX) {
        shell_error(sh, "Value %ld out of range (0-%d)", value, SET_COUNT_MAX);
        return -EINVAL;
    }

    our_driver_set_count(dev, (uint32_t)value);
    shell_print(sh, "Fetch count set to %ld", value);
    return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(sub_our_driver,
    SHELL_CMD(fetch, NULL, "Fetch a sample (LED on).", cmd_sensor_fetch),
    SHELL_CMD(read,  NULL, "Read the sample and print it (LED off).", cmd_sensor_read),
    SHELL_CMD(info,  NULL, "Show device name and ready state.", cmd_sensor_info),
    SHELL_CMD_ARG(set, NULL, "Set the fetch count: set <0-100>.", cmd_sensor_set, 2, 0),
    SHELL_SUBCMD_SET_END
);

SHELL_CMD_REGISTER(our_driver, &sub_our_driver, "LED sensor commands", NULL);
