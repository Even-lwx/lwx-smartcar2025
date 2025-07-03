#ifndef _IMU660RB_H
#define _IMU660RB_H

#include "zf_common_typedef.h"


#define LED1                        (H2 )

#define IMU_PIT                         (TIM7_PIT )                                 // 使用的周期中断编号 如果修改 需要同步对应修改周期中断编号与 isr.c 中的调用
void imu_init(void);
void pit_imu_handler (void);
void get_attitude_angles(float* roll, float* pitch, float* yaw);



extern float roll, pitch, yaw;


#include <math.h>

// IMU滤波器结构体
typedef struct {
    float alpha;         // 互补滤波系数
    float roll, pitch, yaw; // 欧拉角(弧度)
    float gx_bias, gy_bias, gz_bias; // 陀螺仪零偏
    int is_calibrated;   // 校准标志
    int is_first_update; // 首次更新标志
} IMUFilter;

// 函数声明
void IMUFilter_Init(IMUFilter* filter, float alpha);
void IMUFilter_CalibrateGyro(IMUFilter* filter, 
                            float gyro_x, float gyro_y, float gyro_z);
void IMUFilter_Update(IMUFilter* filter, 
                     float acc_x, float acc_y, float acc_z,
                     float gyro_x, float gyro_y, float gyro_z);
void IMUFilter_GetEulerAngles(IMUFilter* filter, 
                             float* roll, float* pitch, float* yaw);


#endif
