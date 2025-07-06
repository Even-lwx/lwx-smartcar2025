#include "zf_common_headfile.h"
#include <math.h>
#include "pid.h"


uint8_t car_run=1; //������־λ



// ���������ݱ���
extern int16 Encoder_Left;
extern int16 Encoder_Right;
extern float pitch, roll, yaw;


// �ջ������м����
int Vertical_out, Velocity_out,Gyro_Out, Turn_out, Target_Speed = 0, Target_turn, MOTO1, MOTO2;
float Med_Angle = 0; // ƽ��ʱ�Ƕ�ֵƫ��������е��ֵ��
// PID����
float Gyro_Kp=0.96,Gyro_Ki=0.004;
float Vertical_Kp =-53 , Vertical_Kd =-0.071; // ֱ���� 
float Velocity_Kp = 0, Velocity_Ki = 0;	// �ٶȻ� 


float Turn_Kp=0, Turn_Kd = 0;					// ת��

uint8_t stop;

#define SPEED_Y 30	// ����(ǰ��)����趨�ٶ�
#define SPEED_Z 150 // ƫ��(����)����趨�ٶ�


int Gyro_Loop(int target_gyro, float actual_gyro)
{
    static float integral = 0; // ������
    float error = target_gyro - actual_gyro; // ���ٶ�ƫ��
    float p_term = Gyro_Kp * error;          // ������
    
    // ������������޷�
    integral += error;
    integral = integral > 1000 ? 1000 : (integral < -1000 ? -1000 : integral); // �����޷�
    float i_term = Gyro_Ki * integral;      // �������Ҫ���Gyro_Ki����
    
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

int Turn(int Bias, int Last_Bias) // ת�����
{
	float temp;
	temp = Bias * Turn_Kp + Last_Bias * Turn_Kd;
	return temp;
}

void Control(void) 
{

	int PWM_out;
	// 1����ȡ������������
	pit_encoder_handler();//���������ݲɼ�
	first_order_filtering();//������rollֵ
	
	if(car_run ==1)
	{
	//2.ƽ�����
	Velocity_out = Velocity(Target_Speed, Encoder_Left, Encoder_Right);
	Vertical_out = Vertical(Velocity_out + Med_Angle, roll_offset, imu963ra_gyro_x);
	Gyro_Out = Gyro_Loop(Vertical_out, imu963ra_gyro_x);
	PWM_out =  Gyro_Out ;
	
	//3.ת�����
	
	
	
	MOTO1 = PWM_out + Turn_out;
	MOTO2 = PWM_out - Turn_out;
	
	Limit(&MOTO1,&MOTO2);
	Load(MOTO1,MOTO2);
	//motor_protect(&MOTO1,&MOTO2);
	}
}
