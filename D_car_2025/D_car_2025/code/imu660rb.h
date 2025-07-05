#ifndef _IMU660RB_H
#define _IMU660RB_H

#include "zf_common_typedef.h"


#define LED1                        (H2 )

#define IMU_PIT                         (TIM7_PIT )                                 // ʹ�õ������жϱ�� ����޸� ��Ҫͬ����Ӧ�޸������жϱ���� isr.c �еĵ���
void imu_init(void);
void pit_imu_handler (void);
void get_attitude_angles(float* roll, float* pitch, float* yaw);



extern float roll, pitch, yaw;
extern float tau;//�˵�����




#endif
