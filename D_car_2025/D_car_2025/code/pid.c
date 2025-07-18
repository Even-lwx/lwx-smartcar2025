#include "zf_common_headfile.h"
#include "pid.h"

uint8_t car_run = 0; // ������־λ

// ���������ݱ���
extern int16 Encoder_Left;
extern int16 Encoder_Right;
// �ջ������м����
int Vertical_out, Velocity_out, Gyro_Out, Turn_out, Target_turn, MOTO1, MOTO2;
int turn_offset, turn_offset_last;
float Med_Angle = 0; // ƽ��ʱ�Ƕ�ֵƫ��������е��ֵ��
// PID����
float Gyro_Kp = -1.05, Gyro_Ki = -0.455;
float Vertical_Kp = -2.3, Vertical_Kd = -0.1;				 // ֱ����
/*�����ǲ˵�����*/
int  Target_Speed = 125;
float Velocity_Kp = -23, Velocity_Ki = -0;					 // �ٶȻ�
float Turn_kp1=0.2, Turn_kp2 = 0.635, Turn_kd1, Turn_kd2 = -0.51; // ppddת��
/*===============================================================*/
uint32 pid_count = 0;
uint8_t stop;

int Gyro_Loop(int target_gyro, float actual_gyro)
{
	static float integral = 0;				 // ������
	float error = target_gyro - actual_gyro; // ���ٶ�ƫ��
	float p_term = Gyro_Kp * error;			 // ������

	// ������������޷�
	// ��������
	if (pid_count % 100 == 0) // 100ms
	{
		integral = 0;
	}
	integral += error;
	integral = integral > 1000 ? 1000 : (integral < -1000 ? -1000 : integral); // �����޷�
	float i_term = Gyro_Ki * integral;										   // �������Ҫ���Gyro_Ki����

	float output = p_term + i_term;
	return (int)output;
}

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
	// �����ʱ����
	if (pid_count % 100 == 0)
	{
		Encoder_S = 0;
	}
	Encoder_S += Err_LowOut;
	// 4�������޷�(-20000~20000)
	Encoder_S = Encoder_S > 10000 ? 10000 : (Encoder_S < (-10000) ? (-10000) : Encoder_S);
	if (stop == 1)
		Encoder_S = 0, stop = 0;
	// 5���ٶȻ�����
	temp = Velocity_Kp * Err_LowOut + Velocity_Ki * Encoder_S;
	return temp;
}

// ת��PD������(���������)
int Turn(int turn_offset, int gyro_z) // ת�����
{
	float temp;
	temp = turn_offset * Turn_kp1 + abs(turn_offset) * turn_offset * Turn_kp2 + (turn_offset - turn_offset_last) * Turn_kd1 + gyro_z * Turn_kd2;
	turn_offset_last = turn_offset;
	return temp;
}

void Control(void)
{

	int PWM_out;
	// 1����ȡ������������
	if (pid_count % 2 == 0)
	{
		first_order_complementary_filtering();
		pit_encoder_handler();
	}

	// printf("%d\r\n",turn_offset);
	if (car_run == 1)
	{

		// 2.ƽ�����

		if (pid_count % 20 == 0)
		{
			Velocity_out = Velocity(Target_Speed, Encoder_Left, Encoder_Right);
		}
		if (pid_count % 5 == 0)
		{

			Vertical_out = Vertical(Velocity_out + Med_Angle, filtering_angle, gx);
		}

		Gyro_Out = Gyro_Loop(Vertical_out, gx);

		PWM_out = Gyro_Out;

		// 3.ת�����
		if (image_proess)
		{
			Turn_out = Turn(turn_offset, gz);
			image_proess = 0;
		}
		//	printf("%d,%d,%d\r\n",PWM_out, Turn_out,turn_offset);
		MOTO1 = PWM_out + Turn_out;
		MOTO2 = PWM_out - Turn_out;

		Limit(&MOTO1, &MOTO2);
		Load(MOTO1, MOTO2);
		motor_protect(&MOTO1, &MOTO2); // �������
	}
	pid_count++;
}
