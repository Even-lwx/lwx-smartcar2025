#include "zf_common_headfile.h"
#include "motor.h"

#define PWM_MAX 10000
#define PWM_MIN -10000
#define SPEED_PROTECT 8000
#define TURN_PPROTECT 1000
// 电机的pwm是用的TIM5


/**
  * @brief  电机初始化
  * @param  无
  */

void motor_init()
{
	// 此处编写用户代码 例如外设初始化代码等
	gpio_init(DIR_L, GPO, GPIO_HIGH, GPO_PUSH_PULL); // GPIO 初始化为输出 默认上拉输出高
	pwm_init(PWM_L, 17000, 0);						 // PWM 通道初始化频率 17KHz 占空比初始为 0

	gpio_init(DIR_R, GPO, GPIO_HIGH, GPO_PUSH_PULL); // GPIO 初始化为输出 默认上拉输出高
	pwm_init(PWM_R, 17000, 0);						 // PWM 通道初始化频率 17KHz 占空比初始为 0
	// 此处编写用户代码 例如外设初始化代码
}


/**
  * @brief  电机速度设置
  * @param  motor1：左电机速度
						motor2：右电机速度
  */
void Load(int motor1, int motor2)
{
	if (motor1 < 0)
	{
			gpio_set_level(DIR_L, GPIO_HIGH);						 // DIR输出高电平
		pwm_set_duty(PWM_L, abs(motor1)); // 计算占空比
	}
	else
	{
gpio_set_level(DIR_L, GPIO_LOW);						 // DIR输出低电平
		pwm_set_duty(PWM_L, abs(motor1)); // 计算占空比
	}
	if (motor2 < 0)
	{
			gpio_set_level(DIR_R, GPIO_LOW);						 // DIR输出低电平
		pwm_set_duty(PWM_R, abs(motor2)); // 计算占空比

	
	}
	else
	{
				gpio_set_level(DIR_R, GPIO_HIGH);						 // DIR输出高电平
		pwm_set_duty(PWM_R, abs(motor2)); // 计算占空比

		
	}

	system_delay_ms(50);
}

/**
  * @brief  电机速度限幅
  * @param  motor1：左电机速度
						motor2：右电机速度
  */
void Limit(int* motor1, int* motor2)
{
	if (*motor1 > PWM_MAX)
		*motor1 = PWM_MAX;
	if (*motor1 < PWM_MIN)
		*motor1 = PWM_MIN;
	if (*motor2 > PWM_MAX)
		*motor2 = PWM_MAX;
	if (*motor2 < PWM_MIN)
		*motor2 = PWM_MIN;
}


/**
  * @brief  电机速度保护、旋转保护
  * @param  motor1：左电机速度
						motor2：右电机速度
  */
void motor_protect(int* motor1,int* motor2)
{

	if(abs(*motor1)>SPEED_PROTECT||abs(*motor2)>SPEED_PROTECT||abs(*motor1-*motor2)>TURN_PPROTECT)
	{
			car_run=0;
			gpio_set_level(D7, GPIO_HIGH);                                            // BEEP 响
		system_delay_ms(50);
		gpio_set_level(D7, GPIO_LOW); 
			Load(0,0);
	
	
	}

}

