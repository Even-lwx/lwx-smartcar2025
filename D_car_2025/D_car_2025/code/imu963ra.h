#ifndef _IMU963RA_H
#define _IMU963RA_H

#include "zf_common_typedef.h"
#define IMU_PIT                         (TIM7_PIT )                                 // 使用的周期中断编号 如果修改 需要同步对应修改周期中断编号与 isr.c 中的调用

extern float roll,roll_offset;
void imu_init(void);
void first_order_filtering(void);
#endif 




