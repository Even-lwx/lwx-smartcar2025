#ifndef _IMU660RB_H
#define _IMU660RB_H

#include "zf_common_typedef.h"


#define LED1                        (H2 )

#define IMU_PIT                         (TIM7_PIT )                                 // ʹ�õ������жϱ�� ����޸� ��Ҫͬ����Ӧ�޸������жϱ���� isr.c �еĵ���
void imu_init(void);
void pit_imu_handler (void);
void get_attitude_angles(float* roll, float* pitch, float* yaw);



extern float roll, pitch, yaw;


#include <math.h>

// IMU�˲����ṹ��
typedef struct {
    float alpha;         // �����˲�ϵ��
    float roll, pitch, yaw; // ŷ����(����)
    float gx_bias, gy_bias, gz_bias; // ��������ƫ
    int is_calibrated;   // У׼��־
    int is_first_update; // �״θ��±�־
} IMUFilter;

// ��������
void IMUFilter_Init(IMUFilter* filter, float alpha);
void IMUFilter_CalibrateGyro(IMUFilter* filter, 
                            float gyro_x, float gyro_y, float gyro_z);
void IMUFilter_Update(IMUFilter* filter, 
                     float acc_x, float acc_y, float acc_z,
                     float gyro_x, float gyro_y, float gyro_z);
void IMUFilter_GetEulerAngles(IMUFilter* filter, 
                             float* roll, float* pitch, float* yaw);


#endif
