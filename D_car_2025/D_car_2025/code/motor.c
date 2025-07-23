#include "zf_common_headfile.h"
#include "motor.h"

#define PWM_MAX 8000
#define PWM_MIN -8000
#define SPEED_PROTECT 200
#define TURN_PPROTECT 6000
// �����pwm���õ�TIM5

uint8 zebra_count;
int zebracount = 2; // Ԫ�������˵�����
uint8 speed_over;//������ת�ĺ�����
/**
 * @brief  �����ʼ��
 * @param  ��
 */

void motor_init()
{
	// �˴���д�û����� ���������ʼ�������
	gpio_init(DIR_L, GPO, GPIO_HIGH, GPO_PUSH_PULL); // GPIO ��ʼ��Ϊ��� Ĭ�����������
	pwm_init(PWM_L, 17000, 0);						 // PWM ͨ����ʼ��Ƶ�� 17KHz ռ�ձȳ�ʼΪ 0

	gpio_init(DIR_R, GPO, GPIO_HIGH, GPO_PUSH_PULL); // GPIO ��ʼ��Ϊ��� Ĭ�����������
	pwm_init(PWM_R, 17000, 0);						 // PWM ͨ����ʼ��Ƶ�� 17KHz ռ�ձȳ�ʼΪ 0
													 // �˴���д�û����� ���������ʼ������
	ips200_show_string(10, 160, "motor init success");
}

/**
  * @brief  ����ٶ�����
  * @param  motor1�������ٶ�
						motor2���ҵ���ٶ�
  */
void Load(int motor1, int motor2)
{
	if (motor1 < 0)
	{
		gpio_set_level(DIR_L, GPIO_HIGH); // DIR����ߵ�ƽ
		pwm_set_duty(PWM_L, abs(motor1)); // ����ռ�ձ�
	}
	else
	{
		gpio_set_level(DIR_L, GPIO_LOW);  // DIR����͵�ƽ
		pwm_set_duty(PWM_L, abs(motor1)); // ����ռ�ձ�
	}
	if (motor2 < 0)
	{
		gpio_set_level(DIR_R, GPIO_LOW);  // DIR����͵�ƽ
		pwm_set_duty(PWM_R, abs(motor2)); // ����ռ�ձ�
	}
	else
	{
		gpio_set_level(DIR_R, GPIO_HIGH); // DIR����ߵ�ƽ
		pwm_set_duty(PWM_R, abs(motor2)); // ����ռ�ձ�
	}
}

/**
  * @brief  ����ٶ��޷�
  * @param  motor1�������ٶ�
						motor2���ҵ���ٶ�
  */
void Limit(int *motor1, int *motor2)
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
  * @brief  ����ٶȱ�������ת����
  * @param  motor1�������ٶ�
						motor2���ҵ���ٶ�
  */
void motor_protect(int *motor1, int *motor2)
{
	if (car_run == 1 && (abs(Encoder_Left) > SPEED_PROTECT || abs(Encoder_Right) > SPEED_PROTECT)) //||abs(Encoder_Left-Encoder_Right)>TURN_PPROTECT
	{
		speed_over++;
		if (speed_over >= 50)
		{
			car_run = 0;
			Load(0, 0);
			buzzer_on(1000);
			
		}
	}
	else
	{
		speed_over = 0;
	}

	if (image_out_of_bounds(binaryImage))
	{
		car_run = 0;
		buzzer_on(100);
		Load(0, 0);
	}
}

/*����Ĵ�����������4·pwm������ */

// #define PWM_CH1                 (TIM5_PWM_CH1_A0)
// #define PWM_CH2                 (TIM5_PWM_CH2_A1)
// #define PWM_CH3                 (TIM5_PWM_CH3_A2)
// #define PWM_CH4                 (TIM5_PWM_CH4_A3)
// void motor_init()
//{

//
//	  pwm_init(PWM_CH1, 17000, 0);                                                // ��ʼ�� PWM ͨ�� Ƶ�� 17KHz ��ʼռ�ձ� 0%
//    pwm_init(PWM_CH2, 17000, 0);                                                // ��ʼ�� PWM ͨ�� Ƶ�� 17KHz ��ʼռ�ձ� 0%
//    pwm_init(PWM_CH3, 17000, 0);                                                // ��ʼ�� PWM ͨ�� Ƶ�� 17KHz ��ʼռ�ձ� 0%
//    pwm_init(PWM_CH4, 17000, 0);                                                // ��ʼ�� PWM ͨ�� Ƶ�� 17KHz ��ʼռ�ձ� 0%
//	ips200_show_string(10, 160, "motor init success");
//}

// void Load(int motor1, int motor2)
//{
//
//	if (motor1 < 0)
//	{

//		pwm_set_duty(TIM5_PWM_CH3_A2, abs(motor1)); // ����ռ�ձ�
//		pwm_set_duty(	TIM5_PWM_CH4_A3,0);
//	}
//	else
//	{
//			pwm_set_duty(	TIM5_PWM_CH4_A3, abs(motor1)); // ����ռ�ձ�
// pwm_set_duty(TIM5_PWM_CH3_A2,0);
//	}
//	if (motor2 < 0)
//	{
//		pwm_set_duty(TIM5_PWM_CH2_A1, abs(motor2)); // ����ռ�ձ�
//		pwm_set_duty(TIM5_PWM_CH1_A0,0); // ����ռ�ձ�
//	}
//	else
//	{
//		pwm_set_duty(TIM5_PWM_CH2_A1,0); // ����ռ�ձ�
//			pwm_set_duty(TIM5_PWM_CH1_A0, abs(motor2)); // ����ռ�ձ�
//	}
//
//}
