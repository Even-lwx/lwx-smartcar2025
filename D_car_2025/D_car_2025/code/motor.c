#include "zf_common_headfile.h"

#include "motor.h"

// 电机的pwm是用的TIM5
void motor_init()
{
	// 此处编写用户代码 例如外设初始化代码等
	gpio_init(DIR_L, GPO, GPIO_HIGH, GPO_PUSH_PULL); // GPIO 初始化为输出 默认上拉输出高
	pwm_init(PWM_L, 17000, 0);						 // PWM 通道初始化频率 17KHz 占空比初始为 0

	gpio_init(DIR_R, GPO, GPIO_HIGH, GPO_PUSH_PULL); // GPIO 初始化为输出 默认上拉输出高
	pwm_init(PWM_R, 17000, 0);						 // PWM 通道初始化频率 17KHz 占空比初始为 0
	// 此处编写用户代码 例如外设初始化代码
}

void Load(int motor1, int motor2)
{
	if (motor1 < 0)
	{
		gpio_set_level(DIR_L, GPIO_HIGH);						 // DIR输出高电平
		pwm_set_duty(PWM_L, abs(motor1) * (PWM_DUTY_MAX / 100)); // 计算占空比
	}
	else
	{
		gpio_set_level(DIR_L, GPIO_LOW);						 // DIR输出低电平
		pwm_set_duty(PWM_L, abs(motor1) * (PWM_DUTY_MAX / 100)); // 计算占空比
	}
	if (motor2 < 0)
	{
		gpio_set_level(DIR_R, GPIO_LOW);						 // DIR输出低电平
		pwm_set_duty(PWM_R, abs(motor2) * (PWM_DUTY_MAX / 100)); // 计算占空比
	}
	else
	{

		gpio_set_level(DIR_R, GPIO_HIGH);						 // DIR输出高电平
		pwm_set_duty(PWM_R, abs(motor2) * (PWM_DUTY_MAX / 100)); // 计算占空比
	}

	system_delay_ms(50);
}
