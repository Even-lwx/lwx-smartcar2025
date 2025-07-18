#include "zf_common_headfile.h"
#include "motor.h"

#define PWM_MAX 10000
#define PWM_MIN -10000
#define SPEED_PROTECT 100
#define TURN_PPROTECT 5000
// �����pwm���õ�TIM5


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
	 ips200_show_string(10,160,"motor init success");
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
			gpio_set_level(DIR_L, GPIO_HIGH);						 // DIR����ߵ�ƽ
		pwm_set_duty(PWM_L, abs(motor1)); // ����ռ�ձ�
	}
	else
	{
		gpio_set_level(DIR_L, GPIO_LOW);						 // DIR����͵�ƽ
		pwm_set_duty(PWM_L, abs(motor1)); // ����ռ�ձ�
	}
	if (motor2 < 0)
	{
			gpio_set_level(DIR_R, GPIO_LOW);						 // DIR����͵�ƽ
			pwm_set_duty(PWM_R, abs(motor2)); // ����ռ�ձ�

	
	}
	else
	{
				gpio_set_level(DIR_R, GPIO_HIGH);						 // DIR����ߵ�ƽ
				pwm_set_duty(PWM_R, abs(motor2)); // ����ռ�ձ�
	}

}

/**
  * @brief  ����ٶ��޷�
  * @param  motor1�������ٶ�
						motor2���ҵ���ٶ�
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
  * @brief  ����ٶȱ�������ת����
  * @param  motor1�������ٶ�
						motor2���ҵ���ٶ�
  */
void motor_protect(int* motor1,int* motor2)
{

	if(car_run==1&&(abs(Encoder_Left)>SPEED_PROTECT||abs(Encoder_Right)>SPEED_PROTECT||image_out_of_bounds(binaryImage)))//||abs(Encoder_Left-Encoder_Right)>TURN_PPROTECT
	{
			car_run=0;
			buzzer_on(100);                                          
		
			Load(0,0);

	}
	if(Zebra_Detect())
	{
		printf("%d",system_count);
		car_run=0;
		Load(0,0);
	}

}

