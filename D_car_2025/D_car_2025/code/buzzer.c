#include "zf_common_headfile.h"
#include "buzzer.h"

void buzzer_init()
{
	gpio_init(D7, GPO, GPIO_LOW, GPO_PUSH_PULL);
	ips200_show_string(10,120,"buzzer init success");
}


void buzzer_on()
{
	gpio_set_level(D7, GPIO_HIGH);  

}

void buzzer_off()
{
	gpio_set_level(D7, GPIO_LOW);  
}








