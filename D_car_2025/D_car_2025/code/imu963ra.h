#ifndef _IMU963RA_H
#define _IMU963RA_H

#include "zf_common_typedef.h"
#define IMU_PIT                         (TIM7_PIT )                                 // ʹ�õ������жϱ�� ����޸� ��Ҫͬ����Ӧ�޸������жϱ���� isr.c �еĵ���

extern float roll,roll_offset;
void imu_init(void);
void first_order_filtering(void);
#endif 




