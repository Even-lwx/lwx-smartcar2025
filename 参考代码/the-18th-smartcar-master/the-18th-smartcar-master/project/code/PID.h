#ifndef __PID_H__
#define __PID_H__

#include "zf_common_headfile.h"

#define SPEED_MAX   4000  //����ٶ��޷�����
#define SPEED_MIN  -4000  //����ٶ��޷�����

int PD_Camera(float err);//���PD����
int PD_ADC(float err);//���PD����
int PID_L(int set_speed ,int speed);//pid���Ƶ��ת��
int PID_R(int set_speed ,int speed);//pid���Ƶ��ת��

#endif
