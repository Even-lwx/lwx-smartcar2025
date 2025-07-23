#include "zf_common_headfile.h"
#include "buzzer.h"

void buzzer_init()
{
	gpio_init(D7, GPO, GPIO_LOW, GPO_PUSH_PULL);
	ips200_show_string(10, 120, "buzzer init success");
}

/**
 * @brief  蜂鸣器警报
 * @param  tim：时间（0手动开启，-1手动关闭）
 */
void buzzer_on(int16 tim)
{
	if (tim == 0)
	{
		gpio_set_level(D7, GPIO_HIGH);
		return;
	}
	if (tim == -1)
	{
		gpio_set_level(D7, GPIO_LOW);
		return;
	}

	gpio_set_level(D7, GPIO_HIGH);
	system_delay_ms(tim);
	gpio_set_level(D7, GPIO_LOW);
}
