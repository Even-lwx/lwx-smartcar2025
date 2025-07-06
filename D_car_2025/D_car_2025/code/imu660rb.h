#ifndef _IMU660RB_H
#define _IMU660RB_H

#include "zf_common_typedef.h"


/*
		以下是第一版代码（四元数解算）的声明
*/
#define LED1                        (H2 )
void imu660rb_init(void);
void pit_imu660rb_handler (void);
void get_attitude_angles(float* roll, float* pitch, float* yaw);
extern float roll, pitch, yaw,roll_offset;
extern float tau;//菜单调参


/*
		以下是第二版代码（一阶互补滤波）的声明
*/
extern float filtering_angle;
void first_order_filtering(void);

#endif
