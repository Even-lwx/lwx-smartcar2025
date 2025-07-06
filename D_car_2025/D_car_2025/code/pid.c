#include "zf_common_headfile.h"
#include <math.h>
#include "pid.h"


uint8_t car_run=1; //发车标志位



// 传感器数据变量
extern int16 Encoder_Left;
extern int16 Encoder_Right;
extern float pitch, roll, yaw;


// 闭环控制中间变量
int Vertical_out, Velocity_out,Gyro_Out, Turn_out, Target_Speed = 0, Target_turn, MOTO1, MOTO2;
float Med_Angle = 0; // 平衡时角度值偏移量（机械中值）
// PID参数
float Gyro_Kp=0.96,Gyro_Ki=0.004;
float Vertical_Kp =-53 , Vertical_Kd =-0.071; // 直立环 
float Velocity_Kp = 0, Velocity_Ki = 0;	// 速度环 


float Turn_Kp=0, Turn_Kd = 0;					// 转向环

uint8_t stop;

#define SPEED_Y 30	// 俯仰(前后)最大设定速度
#define SPEED_Z 150 // 偏航(左右)最大设定速度


int Gyro_Loop(int target_gyro, float actual_gyro)
{
    static float integral = 0; // 积分项
    float error = target_gyro - actual_gyro; // 角速度偏差
    float p_term = Gyro_Kp * error;          // 比例项
    
    // 积分项计算与限幅
    integral += error;
    integral = integral > 1000 ? 1000 : (integral < -1000 ? -1000 : integral); // 积分限幅
    float i_term = Gyro_Ki * integral;      // 积分项，需要添加Gyro_Ki参数
    
    float output = p_term + i_term;
    return (int)output;
}



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

int Turn(int Bias, int Last_Bias) // 转向控制
{
	float temp;
	temp = Bias * Turn_Kp + Last_Bias * Turn_Kd;
	return temp;
}

void Control(void) 
{

	int PWM_out;
	// 1、读取传感器的数据
	pit_encoder_handler();//编码器数据采集
	first_order_filtering();//仅计算roll值
	
	if(car_run ==1)
	{
	//2.平衡控制
	Velocity_out = Velocity(Target_Speed, Encoder_Left, Encoder_Right);
	Vertical_out = Vertical(Velocity_out + Med_Angle, roll_offset, imu963ra_gyro_x);
	Gyro_Out = Gyro_Loop(Vertical_out, imu963ra_gyro_x);
	PWM_out =  Gyro_Out ;
	
	//3.转向控制
	
	
	
	MOTO1 = PWM_out + Turn_out;
	MOTO2 = PWM_out - Turn_out;
	
	Limit(&MOTO1,&MOTO2);
	Load(MOTO1,MOTO2);
	//motor_protect(&MOTO1,&MOTO2);
	}
}
