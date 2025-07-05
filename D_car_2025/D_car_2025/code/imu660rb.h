#ifndef _IMU660RB_H
#define _IMU660RB_H

#include "zf_common_typedef.h"


#define LED1                        (H2 )

#define IMU_PIT                         (TIM7_PIT )                                 // 使用的周期中断编号 如果修改 需要同步对应修改周期中断编号与 isr.c 中的调用
void imu_init(void);
void pit_imu_handler (void);
void get_attitude_angles(float* roll, float* pitch, float* yaw);



extern float roll, pitch, yaw;
extern float tau;//菜单调参




#endif
