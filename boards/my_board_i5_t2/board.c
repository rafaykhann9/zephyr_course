#include <zephyr/kernel.h>
#include <zephyr/platform/hooks.h>

void board_late_init_hook(void)
{
	printk("Board Initialized\n");
}
