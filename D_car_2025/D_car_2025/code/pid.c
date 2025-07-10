#include "zf_common_headfile.h"
#include <math.h>
#include "pid.h"


uint8_t car_run=1; //发车标志位



// 传感器数据变量
extern int16 Encoder_Left;
extern int16 Encoder_Right;


// 闭环控制中间变量
int Vertical_out, Velocity_out,Gyro_Out, Turn_out, Target_Speed = 180, Target_turn, MOTO1, MOTO2;
int turn_offset,turn_offset_last;
float Med_Angle = 0; // 平衡时角度值偏移量（机械中值）
// PID参数
float Gyro_Kp=-0.5,Gyro_Ki=-0.9;
float Vertical_Kp =-90, Vertical_Kd =-0.2; // 直立环 
float Velocity_Kp = -0.019, Velocity_Ki = -0;	// 速度环 
float Turn_kp1=0,Turn_kp2=0,Turn_kd1=0,Turn_kd2=0;					// ppdd转向环


uint32 pid_count=0;
uint8_t stop;




int Gyro_Loop(int target_gyro, float actual_gyro)
{
    static float integral = 0; // 积分项
    float error = target_gyro - actual_gyro; // 角速度偏差
    float p_term = Gyro_Kp * error;          // 比例项
    
    // 积分项计算与限幅
		//积分清零
		if(pid_count%10==0)
		{
			integral=0;
		}
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
	//积分项定时清零
	if(pid_count%10==0)
	{
			Encoder_S=0;
	}
	Encoder_S += Err_LowOut;
	// 4、积分限幅(-20000~20000)
	Encoder_S = Encoder_S > 10000 ? 10000 : (Encoder_S < (-10000) ? (-10000) : Encoder_S);
	if (stop == 1)
		Encoder_S = 0, stop = 0;
	// 5、速度环计算
	temp = Velocity_Kp * Err_LowOut + Velocity_Ki * Encoder_S;
	return temp;
}



#define TURN_STANDARD 80 //前瞻
// 转向环PD控制器
int Turn(int turn_offset, int gyro_z) // 转向控制
{
	 float temp;
	temp=turn_offset*Turn_kp1+turn_offset*abs(turn_offset)*Turn_kp2+(turn_offset-turn_offset_last)*Turn_kd1+gyro_z*Turn_kd2;
	turn_offset_last=turn_offset;
	return temp;
}


void Control(void) 
{

	int PWM_out;
	// 1、读取传感器的数据
	pit_encoder_handler();//编码器数据采集
	first_order_complementary_filtering();
	turn_offset=((Left_Line[TURN_STANDARD]+Right_Line[TURN_STANDARD])>>1)-MT9V03X_W/2;//转向偏差（左负右正）
	
	if(car_run ==1)
	{
	 pid_count++;
		//2.平衡控制
		
		if(pid_count%5==0)
	{
		Velocity_out = Velocity(Target_Speed, Encoder_Left, Encoder_Right);
	}
	if(pid_count%3==0)
	{
		
		Vertical_out = Vertical(Velocity_out + Med_Angle,filtering_angle,gx);
	}
	
	
	Gyro_Out = Gyro_Loop(Vertical_out, gx);
	PWM_out =Gyro_Out ;
	
	//3.转向控制
	//Turn_out=Turn(turn_offset, imu963ra_gyro_z);
	
	
	MOTO1 = PWM_out + Turn_out;
	MOTO2 = PWM_out - Turn_out;
	
	Limit(&MOTO1,&MOTO2);
	Load(MOTO1,MOTO2);
	//motor_protect(&MOTO1,&MOTO2);//电机保护
	}
}


