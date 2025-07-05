#include "zf_common_headfile.h"

#include <math.h>

#include "pid.h"

// 传感器数据变量
extern int16 encoder_data_1;
extern int16 encoder_data_2;
extern float pitch, roll, yaw;
short gyrox, gyroy, gyroz;
short aacx, aacy, aacz;

// 闭环控制中间变量
int Vertical_out, Velocity_out, Turn_out, Target_Speed = 0, Target_turn, MOTO1, MOTO2;
float Med_Angle = -5; // 平衡时角度值偏移量（机械中值）
// PID参数
float Vertical_Kp = 440, Vertical_Kd = 1.3; // 直立环 数量级（Kp：0~1000、Kd：0~10）
float Velocity_Kp = 0.3, Velocity_Ki = 0;	// 速度环 数量级（Kp：0~1）
float Turn_Kp, Turn_Kd = 0;					// 转向环

uint8_t stop;

#define SPEED_Y 30	// 俯仰(前后)最大设定速度
#define SPEED_Z 150 // 偏航(左右)最大设定速度

// 直立环PD控制器
// 输入：期望角度、真实角度、角速度
int Vertical(float Med, float Angle, float gyro_Y)
{
	int temp;
	temp = Vertical_Kp * (Angle - Med) + Vertical_Kd * gyro_Y;
	return temp;
}

// 速度环PI控制器
// 输入：期望速度、左编码器、右编码器
int Velocity(int Target, int encoder_L, int encoder_R)
{
	static int Err_LowOut_last, Encoder_S;
	static float a = 0.7;
	int Err, Err_LowOut, temp;
	Velocity_Ki = Velocity_Kp / 200;
	// 1、计算偏差值
	Err = (encoder_L + encoder_R) - Target;
	// 2、低通滤波
	Err_LowOut = (1 - a) * Err + a * Err_LowOut_last;
	Err_LowOut_last = Err_LowOut;
	// 3、积分
	Encoder_S += Err_LowOut;
	// 4、积分限幅(-20000~20000)
	Encoder_S = Encoder_S > 20000 ? 20000 : (Encoder_S < (-20000) ? (-20000) : Encoder_S);
	if (stop == 1)
		Encoder_S = 0, stop = 0;
	// 5、速度环计算
	temp = Velocity_Kp * Err_LowOut + Velocity_Ki * Encoder_S;
	return temp;
}

// 转向环PD控制器
// 输入：CCD中值偏移
int Turn(int Bias, int Last_Bias) // 转向控制
{
	float temp;
	temp = Bias * Turn_Kp + Last_Bias * Turn_Kd;
	return temp;
}

void Control(void) // 每隔10ms调用一次
{

	int PWM_out;

	// 1、读取编码器和陀螺仪的数据
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
