#include "zf_common_headfile.h"
#include "buzzer.h"

void buzzer_init()
{
	gpio_init(D7, GPO, GPIO_LOW, GPO_PUSH_PULL);
	ips200_show_string(10,120,"buzzer init success");
}


void buzzer_on(uint16 tim)
{
	gpio_set_level(D7, GPIO_HIGH);
	system_delay_ms(tim);	
	gpio_set_level(D7, GPIO_LOW);  	

}









