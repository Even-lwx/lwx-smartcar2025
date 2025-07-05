#include "zf_common_headfile.h"

#include "motor.h"

// �����pwm���õ�TIM5
void motor_init()
{
	// �˴���д�û����� ���������ʼ�������
	gpio_init(DIR_L, GPO, GPIO_HIGH, GPO_PUSH_PULL); // GPIO ��ʼ��Ϊ��� Ĭ�����������
	pwm_init(PWM_L, 17000, 0);						 // PWM ͨ����ʼ��Ƶ�� 17KHz ռ�ձȳ�ʼΪ 0

	gpio_init(DIR_R, GPO, GPIO_HIGH, GPO_PUSH_PULL); // GPIO ��ʼ��Ϊ��� Ĭ�����������
	pwm_init(PWM_R, 17000, 0);						 // PWM ͨ����ʼ��Ƶ�� 17KHz ռ�ձȳ�ʼΪ 0
	// �˴���д�û����� ���������ʼ������
}

void Load(int motor1, int motor2)
{
	if (motor1 < 0)
	{
		gpio_set_level(DIR_L, GPIO_HIGH);						 // DIR����ߵ�ƽ
		pwm_set_duty(PWM_L, abs(motor1) * (PWM_DUTY_MAX / 100)); // ����ռ�ձ�
	}
	else
	{
		gpio_set_level(DIR_L, GPIO_LOW);						 // DIR����͵�ƽ
		pwm_set_duty(PWM_L, abs(motor1) * (PWM_DUTY_MAX / 100)); // ����ռ�ձ�
	}
	if (motor2 < 0)
	{
		gpio_set_level(DIR_R, GPIO_LOW);						 // DIR����͵�ƽ
		pwm_set_duty(PWM_R, abs(motor2) * (PWM_DUTY_MAX / 100)); // ����ռ�ձ�
	}
	else
	{

		gpio_set_level(DIR_R, GPIO_HIGH);						 // DIR����ߵ�ƽ
		pwm_set_duty(PWM_R, abs(motor2) * (PWM_DUTY_MAX / 100)); // ����ռ�ձ�
	}

	system_delay_ms(50);
}
