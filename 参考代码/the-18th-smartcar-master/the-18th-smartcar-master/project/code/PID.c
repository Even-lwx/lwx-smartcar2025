#include "zf_common_headfile.h"
#include "PID.h"

extern volatile uint8 Go;             //�����׶εı�־λ
extern volatile int Island_State;     //����״̬��־
extern volatile int Search_Stop_Line;

float Island_P=1.46;
float Island_D=4.1;
float Global_D=4.56;
float P_L=30;
float I_L=1.6;
float P_R=30;
float I_R=1.6;

/*-------------------------------------------------------------------------------------------------------------------
  @brief     PD����ͷ����
  @param     err
  @return    ������ֵ
  Sample     Steer_Angle=PD_Camera(Err);//���PD��
  @note      null
-------------------------------------------------------------------------------------------------------------------*/
int PD_Camera(float err)//���PD����
{
   int  u;
   float  P=1.98; //p����
   float  D=1.632;//d����
   volatile static float error_current,error_last;
   float ek,ek1;
   error_current=err;
   ek=error_current;
   ek1=error_current-error_last;
   if(Go==7)//�������
   {
       DuoJi_GetP(&P ,MT9V03X_H-Search_Stop_Line, err);//����ģ��P,P�ĵ�ַ��������ֹ�е�λ�ã����
       D=Global_D;
       if(Island_State!=0)//����������pd�������׶ζ���ģ������
       {
           P=Island_P;
           D=Island_D;
       }
   }
    u=P*ek+D*ek1;
    error_last=error_current;

    if(u>=LEFT_MAX)//�޷�����
        u=LEFT_MAX;
    else if(u<=RIGHT_MAX)//�޷�����
        u=RIGHT_MAX;
    return (int)u;
}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     ���PD����
  @param     err
  @return    ������ֵ
  Sample     Steer_Angle=PD(Err);//���PD��
  @note      null
-------------------------------------------------------------------------------------------------------------------*/
int PD_ADC(float err)//���PD����
{
   int  u;
   float  P=1.18;//p����
   float  D=2.56;//d����
   volatile static float error_current,error_last;
   float ek,ek1;
   error_current=err;
   ek=error_current;
   ek1=error_current-error_last;
   P=Fuzzy_P(err,ek1);//��ͳģ��PID
   D=1.185;//d����
    u=P*ek+D*ek1;
    error_last=error_current;
    if(u>=LEFT_MAX)//�޷�����
        u=LEFT_MAX;
    else if(u<=RIGHT_MAX)//�޷�����
        u=RIGHT_MAX;
    return (int)u;
}
/*-------------------------------------------------------------------------------------------------------------------
  @brief     PID����
  @param     int set_speed ,int speed,����ֵ��ʵ��ֵ
  @return    ���ռ�ձ�
  Sample     pwm_R= PID_R(set_speed_right,right_wheel);//pid���Ƶ��ת��
             pwm_L= PID_L(set_speed_left,left_wheel );//pid���Ƶ��ת��
  @note      ������
-------------------------------------------------------------------------------------------------------------------*/
int PID_L(int set_speed ,int speed)//pid���Ƶ��ת��
{
    volatile static int out;
    volatile static int out_increment;
    volatile static int ek,ek1;
    float kp=1.46,ki=2.3;

    if(Go==7)
    {
        kp = P_L;//һ��pi���ӣ��ٶȲ�������̫��
        ki = I_L;
    }
    else//�����׶��ٶȻ�ҪӲ
    {
        kp = 20.0;//һ��pi���ӣ��ٶȲ�������̫��
        ki = 0.9;
    }
    ek1 = ek;
    ek = set_speed - speed;
    out_increment= (int)(kp*(ek-ek1) + ki*ek);
    out+= out_increment;

    if(out>=SPEED_MAX)//�޷�����
        out=SPEED_MAX;
    else if(out<=SPEED_MIN)
        out=SPEED_MIN;
    return (int) out;
}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     PID����
  @param     int set_speed ,int speed,����ֵ��ʵ��ֵ
  @return    ���ռ�ձ�
  Sample     pwm_R= PID_R(set_speed_right,right_wheel);//pid���Ƶ��ת��
             pwm_L= PID_L(set_speed_left,left_wheel );//pid���Ƶ��ת��
  @note      ������
-------------------------------------------------------------------------------------------------------------------*/
int PID_R(int set_speed ,int speed)//pid���Ƶ��ת��
{
    volatile static int  out;
    volatile static int  out_increment;
    volatile static int  ek,ek1;
    float kp=1.56,ki=2.1;
    if(Go==7)//����Ѱ��
    {
        kp = P_R;//һ��pi���ӣ��ٶȲ�������̫��
        ki = I_R;
    }
    else//�����׶��ٶȻ�ҪӲ
    {
        kp = 20.0;//һ��pi���ӣ��ٶȲ�������̫��
        ki = 0.9;
    }
    ek1 = ek;
    ek = set_speed - speed;

    out_increment= (int)(kp*(ek-ek1) + ki*ek);
    out+= out_increment;

    if(out>=SPEED_MAX)//�޷�����
        out=SPEED_MAX;
    else if(out<=SPEED_MIN)
        out=SPEED_MIN;
    return (int) out;
}
