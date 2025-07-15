#ifndef _IMU963RA_H
#define _IMU963RA_H

#include "zf_common_typedef.h"
#define IMU_PIT                         (TIM7_PIT )                                 // ʹ�õ������жϱ�� ����޸� ��Ҫͬ����Ӧ�޸������жϱ���� isr.c �еĵ���
#define gx_error -3
#define gy_error 12
#define gz_error 4
#define machine_mid -3850//��е��ֵ
extern float roll,roll_offset,yaw;
extern float filtering_angle;
extern float yaw;
extern int16_t gx   ;
extern int16_t gy   ;
extern int16_t gz   ;
extern int16_t ax   ;
extern int16_t ay   ;
extern int16_t az   ;
void imu_init(void);
void first_order_complementary_filtering(void);
//void first_order_filtering(void);
#endif 




