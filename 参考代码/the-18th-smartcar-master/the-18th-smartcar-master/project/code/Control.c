#include "zf_common_headfile.h"
#include "Control.h"
#include "math.h"
#include "PID.h"

//电磁，摄像头标志位，默认摄像头
volatile uint8 Electromagnet_Flag=0;//0是摄像头，1是电磁
extern volatile uint8 Stop_Flag;

//常规信息类
extern volatile int Search_Stop_Line;     //搜索截止行
extern volatile int Left_Line[MT9V03X_H];  //左边线数组
extern volatile int Right_Line[MT9V03X_H]; //右边线数组

//发车、停止、方向类
volatile uint8 Go=8;             //发车阶段的标志位，默认不动
volatile uint8 Speed_Mode=0;     //速度模式,0低速,1中等,2高速
volatile uint8 Go_Direction=1;   //0右,1左打角，出库
extern volatile uint8 Straight_Flag;//长直道识别

//方向变量
int Steer_Angle=0;        //舵机打角值
volatile float Err=0;     //摄像头误差
volatile extern float ADC_Err;//电磁误差

//速度类
int ADC_Speed=180;     //电磁速度
int Base_Speed=330;    //基准速度
int Speed_Left_Set=300;
int Speed_Right_Set=300;
int Ramp_Speed=200;    //坡道速度0
int Island_Speed=300;   //环岛速度
int Straight_Speed=400; //直道高速
int Barricade_Speed=250;//横断定死速度，防止直道打角来出现问题
float Island_Diff=0.4;  //环岛差速
float Err_Diff=1.1;     //常规差速系数，差速过大容易导致侧翻
float ADC_Diff=0.4;     //电磁差速
float Shift_Ratio=1.5;  //变速系数
float Island_Ratio=0;   //环岛变速

//环岛控制
extern volatile int Island_State;     //环岛状态标志
extern volatile int Ramp_Flag;        //坡道

//横断控制
extern volatile uint8 Barricade_Flag; //横断
extern uint8 Obstacle_Dir; //0右拐，1左拐

//加权控制
const uint8 Weight[MT9V03X_H]=
{
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,              //图像最远端00 ——09 行权重
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,              //图像最远端10 ——19 行权重
        1, 1, 1, 1, 1, 1, 1, 3, 4, 5,              //图像最远端20 ——29 行权重
        6, 7, 9,11,13,15,17,19,20,20,              //图像最远端30 ——39 行权重
       19,17,15,13,11, 9, 7, 5, 3, 1,              //图像最远端40 ——49 行权重
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,              //图像最远端50 ——59 行权重
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,              //图像最远端60 ——69 行权重

};
/*-------------------------------------------------------------------------------------------------------------------
  @brief     利用Vofa发数据
  @param     data1，data2，data3，data4，data5，data6，要发的数据放进入参即可
  @return    null
  Sample     Vofa_data(Steer_Angle,Err,Speed_Left_Real,Speed_Left_Set,Speed_Right_Real,Speed_Right_Set,dl1a_distance_mm);
  @note      如果不够用，继续往后加入参即可，继续加参数参考原则见下注释
-------------------------------------------------------------------------------------------------------------------*/
void Vofa_data(int data1,int data2,int data3,int data4,int data5,int data6,int data7)
{
    int data[7];
    float tempFloat[7];   //定义的临时变量
    uint8 tempData[32];   //定义的传输Buffer，一个int数字占4个字节，如果传7个，4*7=28，后面还有四个校验位，28+4=32

    data[0] = data1;
    data[1] = data2;
    data[2] = data3;
    data[3] = data4;
    data[4] = data5;
    data[5] = data6;
    data[6] = data7;

    tempFloat[0] = (float)data[0];     //转成浮点数
    tempFloat[1] = (float)data[1];
    tempFloat[2] = (float)data[2];
    tempFloat[3] = (float)data[3];
    tempFloat[4] = (float)data[4];
    tempFloat[5] = (float)data[5];
    tempFloat[6] = (float)data[6];

    memcpy(tempData, (uint8 *)tempFloat, sizeof(tempFloat));  //通过拷贝把数据重新整理

    tempData[28] = 0x00;//写入结尾数据
    tempData[39] = 0x00;
    tempData[30] = 0x80;
    tempData[31] = 0x7f;

    uart_write_buffer(UART_7, tempData, 32);//通过串口传输数据，前面开多大的数组，后面占多大长度
}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     摄像头误差获取
  @param     null
  @return    获取到的误差
  Sample     err=Err_Sum();
  @note      加权取平均
-------------------------------------------------------------------------------------------------------------------*/
float Err_Sum(void)
{
    int i;
    float err=0;
    float weight_count=0;
    //常规误差
    for(i=MT9V03X_H-1;i>=MT9V03X_H-Search_Stop_Line-1;i--)//常规误差计算
    {
        err+=(MT9V03X_W/2-((Left_Line[i]+Right_Line[i])>>1))*Weight[i];//右移1位，等效除2
        weight_count+=Weight[i];
    }
    err=err/weight_count;
    
    return err;
}


/*
  GO==1停车，双手离车
  GO==2打角倒退
  GO==3刹车，方向都是反的
  GO==4微微后退
  GO==5刹车充电
  GO==6无
  GO==7常规寻迹
 */
void Direction_Control(void)
{
    if(Go==1||Go==2)//1是双手离车，2是打角积分
    {
       if (Go_Direction==1)   //0右,1左
       {
           Steer_Angle=LEFT_MAX;//强制打角，强制给误差
       }
       else
       {
           Steer_Angle=RIGHT_MAX;//强制打角，强制给误差
       }
    }
    else if(Go==3||Go==4)//3是刹车，4是微微后退
    {
        Steer_Angle=-PD_Camera(Err);//摄像头舵机PD调
    }
    else if(Go==7)
    {
        gpio_set_level(CHARGE_PIN, 0);//后车满电，断电
        if(Electromagnet_Flag==0)//默认摄像头跑
        {
            if(Barricade_Flag!=0)//横断状态单独控制，这里是改误差，下面还有一块很像的，是改舵机
            {
                if (Obstacle_Dir==0) //过横断方向可选
                {
                    if(Barricade_Flag==2)
                    {
                        Err=-100;//改err是为了有差速辅助转弯
                    }
                    else if(Barricade_Flag==3)
                    {
                        Err=100;//先正打角，再反打角
                    }
                }
                else
                {
                    if(Barricade_Flag==2)
                    {
                        Err=100;//改err是为了有差速辅助转弯
                    }
                    else if(Barricade_Flag==3)
                    {
                        Err=-100;//先正打角，再反打角
                    }
                }
            }
            Steer_Angle=PD_Camera(Err);//摄像头舵机PD调
            if(Ramp_Flag==1||Ramp_Flag==2||Ramp_Flag==3)//坡道阶段用电磁判断方向
            {
                Steer_Angle=PD_ADC(ADC_Err);
            }
        }
        else//电磁跑
        {
            Steer_Angle=PD_ADC(ADC_Err);
        }
        if(Barricade_Flag!=0)//横断状态单独控制，这里强改舵机
        {
            if (Obstacle_Dir==0) //过横断方向可选
            {
                if(Barricade_Flag==2)
                {
                    Steer_Angle=RIGHT_MAX;//强制打角，强制给误差
                }
                else if(Barricade_Flag==3)
                {
                    Steer_Angle=LEFT_MAX;
                }
            }
            else
            {
                if(Barricade_Flag==2)
                {
                    Steer_Angle=LEFT_MAX;//强制打角，强制给误差
                }
                else if(Barricade_Flag==3)
                {
                    Steer_Angle=RIGHT_MAX;
                }
            }
        }
    }
    Steer(Steer_Angle);
}

/*
  GO==1停车，双手离车
  GO==2打角倒退
  GO==3刹车，方向都是反的
  GO==4微微后退
  GO==5刹车充电
  GO==6无
  GO==7常规寻迹
 */
void Velocity_Control(int speed_left_real,int speed_right_real)//赛道类型判别，来选定速度
{
    int pwm_R=0,pwm_L=0;
    if(Stop_Flag!=0)//刹车区，不是启动区，刹车刹车！！！
    {
        if(Stop_Flag==1)//看到斑马线，走一小段，再停下
        {
            Speed_Left_Set=200;//这就是看到斑马线走的那一小段
            Speed_Right_Set=200;
        }
        else if(Stop_Flag==2)//立刻刹停
        {
            Speed_Left_Set=0;
            Speed_Right_Set=0;
        }
        else if(Stop_Flag==3)//刹车后开环置零
        {
            pwm_L=0;
            pwm_R=0;
            Motor_Left (pwm_L);
            Motor_Right(pwm_R);
            return;
        }
        pwm_L= PID_L(Speed_Left_Set ,speed_left_real );//pid控制电机转速
        pwm_R= PID_R(Speed_Right_Set,speed_right_real);//pid控制电机转速
    }
    else
    {
        if(Go==1)//1是发车后双手离车，停止不动
        {
            pwm_L=0;
            pwm_R=0;
        }
        else if(Go==2)//打角倒车
        {
            pwm_L=-1400;
            pwm_R=-1400;
        }
        else if(Go==3)//刹车
        {
            Speed_Right_Set=0;
            pwm_R= PID_R(Speed_Right_Set,speed_right_real);//pid控制电机转速
        }
        else if(Go==4)//微微后退
        {
            pwm_L=-700;
            pwm_R=-700;
        }
        else if(Go==5)//停车充电，此时闭环锁死轮胎，后车微微向前去，贴近线圈
        {
            Speed_Right_Set=0;
            pwm_R= PID_R(Speed_Right_Set,speed_right_real);
        }
        else if(Go==6)//
        {
            //  无
        }
        else if(Go==7)//当标志位被置7后，正常进行速度控制
        {
            if(Electromagnet_Flag==0)//默认摄像头跑
            {
                if(Island_State!=0)//环岛速度定值，改为统一控制，效果更好
                {
                    Speed_Left_Set =Island_Speed-Err*Island_Diff;
                    Speed_Right_Set=Island_Speed+Err*Island_Diff;
                }
                else if(Barricade_Flag!=0)//路障速度，差速
                {
                    if(Barricade_Flag==2)
                    {
                        Speed_Left_Set =230-Err*0.2;
                        Speed_Right_Set=230+Err*0.2;
                    }
                    else if(Barricade_Flag==3)
                    {
                        Speed_Left_Set =Barricade_Speed-Err*0.2;
                        Speed_Right_Set=Barricade_Speed+Err*0.2;
                    }
                }
                else //常规控制
                {
                    Speed_Left_Set =Base_Speed;
                    Speed_Right_Set=Base_Speed;
                    if(Straight_Flag==1)//直道高速冲刺
                    {
                        Speed_Left_Set=Straight_Speed;
                        Speed_Right_Set=Straight_Speed;
                    }
                    Speed_Left_Set =Speed_Left_Set -(MT9V03X_H-Search_Stop_Line)*Shift_Ratio;//变速
                    Speed_Right_Set=Speed_Right_Set-(MT9V03X_H-Search_Stop_Line)*Shift_Ratio;//

                    Speed_Left_Set =Speed_Left_Set -Err*Err_Diff;//差速
                    Speed_Right_Set=Speed_Right_Set+Err*Err_Diff;
                }
            }
            else//电磁跑
            {
                Speed_Left_Set =ADC_Speed-ADC_Err*ADC_Diff;//电磁误差*Err_Diff
                Speed_Right_Set=ADC_Speed+ADC_Err*ADC_Diff;
            }
            if(Ramp_Flag!=0)//坡道低速
            {
                 Speed_Left_Set=Ramp_Speed;
                 Speed_Right_Set=Ramp_Speed;
            }
            pwm_L= PID_L(Speed_Left_Set ,speed_left_real );//pid控制电机转速
            pwm_R= PID_R(Speed_Right_Set,speed_right_real);//pid控制电机转速
        }
    }
    Motor_Left (pwm_L);
    Motor_Right(pwm_R);
}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     电机输出函数
  @param     pwm_L 电机输入占空比，可正可负
  @return    null
  Sample     Motor_Left(pwm_L);
  @note      电机最终输出函数，由其他函数调用
                                      在给电机前有限幅处理的
-------------------------------------------------------------------------------------------------------------------*/
void Motor_Left(int pwm_L)
{
    if(pwm_L>=SPEED_MAX)//限幅处理
        pwm_L=SPEED_MAX;
    else if(pwm_L<=SPEED_MIN)
             pwm_L=SPEED_MIN;

    if(pwm_L>=0)//正转
    {
        pwm_set_duty(MOTOR01_SPEED_PIN, pwm_L);//左电机正转
        gpio_set_level(MOTOR01_DIR_PIN, 1);//01左电机，D15,0反转，1正转
    }
    else
    {
        pwm_set_duty(MOTOR01_SPEED_PIN, -pwm_L);//左电机正转
        gpio_set_level(MOTOR01_DIR_PIN, 0);//01左电机，D15,0反转，1正转
    }
}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     电机输出函数
  @param     pwm_R 电机输入占空比，可正可负
  @return    null
  Sample     Motor_Right(pwm_R);
  @note      电机最终输出函数，由其他函数调用
                                      在给电机前有限幅处理的
-------------------------------------------------------------------------------------------------------------------*/
void Motor_Right(int pwm_R)
{
    if(pwm_R>=SPEED_MAX)//限幅处理
        pwm_R=SPEED_MAX;
    else if(pwm_R<=SPEED_MIN)
            pwm_R=SPEED_MIN;

    if(pwm_R>=0)//正转
    {
        pwm_set_duty(MOTOR02_SPEED_PIN, pwm_R);//右电机正转
        gpio_set_level(MOTOR02_DIR_PIN, 0);//02左电机，D14,0反转，1正转
    }
    else
   {
        pwm_set_duty(MOTOR02_SPEED_PIN, -pwm_R);//右电机正转
        gpio_set_level(MOTOR02_DIR_PIN, 1);//02左电机，D14,0反转，1正转
   }
}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     舵机电机输出函数
  @param     angle 输入舵机方向信号，+-LEFT_MAX，RIGHT_MAX之间
  @return    null
  Sample     Steer(angle)；
  @note      舵机最终输出函数，由其他函数调用
                                           在给舵机前有限幅处理的
#define STEER_RIGHT  705  //舵机右打死,-
#define STEER_MID    782  //舵机归中
#define STEER_LEFT   857  //舵机左打死,+
-------------------------------------------------------------------------------------------------------------------*/
void Steer(int angle)
{
    if(angle>=LEFT_MAX)//限幅处理
       angle=LEFT_MAX;
   else if(angle<=RIGHT_MAX)
        angle=RIGHT_MAX;
    pwm_set_duty(STEER_PIN, STEER_MID+angle);//舵机调节
}
