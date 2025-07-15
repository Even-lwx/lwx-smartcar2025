#include "zf_common_headfile.h"
#include "PID.h"

extern volatile uint8 Go;             //发车阶段的标志位
extern volatile int Island_State;     //环岛状态标志
extern volatile int Search_Stop_Line;

float Island_P=1.46;
float Island_D=4.1;
float Global_D=4.56;
float P_L=30;
float I_L=1.6;
float P_R=30;
float I_R=1.6;

/*-------------------------------------------------------------------------------------------------------------------
  @brief     PD摄像头控制
  @param     err
  @return    舵机打角值
  Sample     Steer_Angle=PD_Camera(Err);//舵机PD调
  @note      null
-------------------------------------------------------------------------------------------------------------------*/
int PD_Camera(float err)//舵机PD调节
{
   int  u;
   float  P=1.98; //p动的
   float  D=1.632;//d死的
   volatile static float error_current,error_last;
   float ek,ek1;
   error_current=err;
   ek=error_current;
   ek1=error_current-error_last;
   if(Go==7)//常规控制
   {
       DuoJi_GetP(&P ,MT9V03X_H-Search_Stop_Line, err);//新型模糊P,P的地址，搜索截止行的位置，误差
       D=Global_D;
       if(Island_State!=0)//环岛单独的pd，其他阶段都是模糊控制
       {
           P=Island_P;
           D=Island_D;
       }
   }
    u=P*ek+D*ek1;
    error_last=error_current;

    if(u>=LEFT_MAX)//限幅处理
        u=LEFT_MAX;
    else if(u<=RIGHT_MAX)//限幅处理
        u=RIGHT_MAX;
    return (int)u;
}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     电磁PD控制
  @param     err
  @return    舵机打角值
  Sample     Steer_Angle=PD(Err);//舵机PD调
  @note      null
-------------------------------------------------------------------------------------------------------------------*/
int PD_ADC(float err)//舵机PD调节
{
   int  u;
   float  P=1.18;//p动的
   float  D=2.56;//d死的
   volatile static float error_current,error_last;
   float ek,ek1;
   error_current=err;
   ek=error_current;
   ek1=error_current-error_last;
   P=Fuzzy_P(err,ek1);//传统模糊PID
   D=1.185;//d死的
    u=P*ek+D*ek1;
    error_last=error_current;
    if(u>=LEFT_MAX)//限幅处理
        u=LEFT_MAX;
    else if(u<=RIGHT_MAX)//限幅处理
        u=RIGHT_MAX;
    return (int)u;
}
/*-------------------------------------------------------------------------------------------------------------------
  @brief     PID控制
  @param     int set_speed ,int speed,期望值，实际值
  @return    电机占空比
  Sample     pwm_R= PID_R(set_speed_right,right_wheel);//pid控制电机转速
             pwm_L= PID_L(set_speed_left,left_wheel );//pid控制电机转速
  @note      调参呗
-------------------------------------------------------------------------------------------------------------------*/
int PID_L(int set_speed ,int speed)//pid控制电机转速
{
    volatile static int out;
    volatile static int out_increment;
    volatile static int ek,ek1;
    float kp=1.46,ki=2.3;

    if(Go==7)
    {
        kp = P_L;//一套pi足矣，速度拨动不会太大
        ki = I_L;
    }
    else//发车阶段速度环要硬
    {
        kp = 20.0;//一套pi足矣，速度拨动不会太大
        ki = 0.9;
    }
    ek1 = ek;
    ek = set_speed - speed;
    out_increment= (int)(kp*(ek-ek1) + ki*ek);
    out+= out_increment;

    if(out>=SPEED_MAX)//限幅处理
        out=SPEED_MAX;
    else if(out<=SPEED_MIN)
        out=SPEED_MIN;
    return (int) out;
}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     PID控制
  @param     int set_speed ,int speed,期望值，实际值
  @return    电机占空比
  Sample     pwm_R= PID_R(set_speed_right,right_wheel);//pid控制电机转速
             pwm_L= PID_L(set_speed_left,left_wheel );//pid控制电机转速
  @note      调参呗
-------------------------------------------------------------------------------------------------------------------*/
int PID_R(int set_speed ,int speed)//pid控制电机转速
{
    volatile static int  out;
    volatile static int  out_increment;
    volatile static int  ek,ek1;
    float kp=1.56,ki=2.1;
    if(Go==7)//常规寻迹
    {
        kp = P_R;//一套pi足矣，速度拨动不会太大
        ki = I_R;
    }
    else//发车阶段速度环要硬
    {
        kp = 20.0;//一套pi足矣，速度拨动不会太大
        ki = 0.9;
    }
    ek1 = ek;
    ek = set_speed - speed;

    out_increment= (int)(kp*(ek-ek1) + ki*ek);
    out+= out_increment;

    if(out>=SPEED_MAX)//限幅处理
        out=SPEED_MAX;
    else if(out<=SPEED_MIN)
        out=SPEED_MIN;
    return (int) out;
}
