#include "zf_common_headfile.h"

#include <math.h>

#include "pid.h"

// ���������ݱ���
extern int16 encoder_data_1;
extern int16 encoder_data_2;
extern float pitch, roll, yaw;
short gyrox, gyroy, gyroz;
short aacx, aacy, aacz;

// �ջ������м����
int Vertical_out, Velocity_out, Turn_out, Target_Speed = 0, Target_turn, MOTO1, MOTO2;
float Med_Angle = -5; // ƽ��ʱ�Ƕ�ֵƫ��������е��ֵ��
// PID����
float Vertical_Kp = 440, Vertical_Kd = 1.3; // ֱ���� ��������Kp��0~1000��Kd��0~10��
float Velocity_Kp = 0.3, Velocity_Ki = 0;	// �ٶȻ� ��������Kp��0~1��
float Turn_Kp, Turn_Kd = 0;					// ת��

uint8_t stop;

#define SPEED_Y 30	// ����(ǰ��)����趨�ٶ�
#define SPEED_Z 150 // ƫ��(����)����趨�ٶ�

// ֱ����PD������
// ���룺�����Ƕȡ���ʵ�Ƕȡ����ٶ�
int Vertical(float Med, float Angle, float gyro_Y)
{
	int temp;
	temp = Vertical_Kp * (Angle - Med) + Vertical_Kd * gyro_Y;
	return temp;
}

// �ٶȻ�PI������
// ���룺�����ٶȡ�����������ұ�����
int Velocity(int Target, int encoder_L, int encoder_R)
{
	static int Err_LowOut_last, Encoder_S;
	static float a = 0.7;
	int Err, Err_LowOut, temp;
	Velocity_Ki = Velocity_Kp / 200;
	// 1������ƫ��ֵ
	Err = (encoder_L + encoder_R) - Target;
	// 2����ͨ�˲�
	Err_LowOut = (1 - a) * Err + a * Err_LowOut_last;
	Err_LowOut_last = Err_LowOut;
	// 3������
	Encoder_S += Err_LowOut;
	// 4�������޷�(-20000~20000)
	Encoder_S = Encoder_S > 20000 ? 20000 : (Encoder_S < (-20000) ? (-20000) : Encoder_S);
	if (stop == 1)
		Encoder_S = 0, stop = 0;
	// 5���ٶȻ�����
	temp = Velocity_Kp * Err_LowOut + Velocity_Ki * Encoder_S;
	return temp;
}

// ת��PD������
// ���룺CCD��ֵƫ��
int Turn(int Bias, int Last_Bias) // ת�����
{
	float temp;
	temp = Bias * Turn_Kp + Last_Bias * Turn_Kd;
	return temp;
}

void Control(void) // ÿ��10ms����һ��
{

	int PWM_out;

	// 1����ȡ�������������ǵ�����
	pit_encoder_handler();

	Velocity_out = Velocity(Target_Speed, Encoder_Left, Encoder_Right);
	Vertical_out = Vertical(Velocity_out + Med_Angle, pitch, gyroy);

	// Turn_out=Turn(Bias,gyroz);
	Turn_out = 0;
	PWM_out = Vertical_out;
	MOTO1 = PWM_out + Turn_out;
	MOTO2 = PWM_out - Turn_out;
	// Limit(&MOTO1,&MOTO2);
	// Load(MOTO1,MOTO2);
}
