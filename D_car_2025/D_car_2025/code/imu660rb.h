#ifndef _IMU660RB_H
#define _IMU660RB_H

#include "zf_common_typedef.h"


/*
		�����ǵ�һ����루��Ԫ�����㣩������
*/
#define LED1                        (H2 )
void imu660rb_init(void);
void pit_imu660rb_handler (void);
void get_attitude_angles(float* roll, float* pitch, float* yaw);
extern float roll, pitch, yaw,roll_offset;
extern float tau;//�˵�����


/*
		�����ǵڶ�����루һ�׻����˲���������
*/
extern float filtering_angle;
void first_order_filtering(void);

#endif
