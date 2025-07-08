#include "zf_common_headfile.h"
#include <math.h>
#include "imu963ra.h"



#define OFFSET 0
float roll = 0.0f;   // 横滚角
float roll_offset=0.0f;
float angle_temp = 0.0f;



float gyro_ration = 0.0174533f; // 度转弧度
float acc_ration = 0.95f;       // 加速度权重(一阶互补滤波系数)
float cycle_T = 0.005f;          // 采样周期(5ms)


void imu_init(void)
{

    // 初始化IMU传感器
    while (1)
    {
        if (imu963ra_init())
        {
            ips200_show_string(0,0,"IMU init error");
					  ips200_clear();
        }
        else
        {
				
            break;
        }
    }

}



/**
 * @brief   一阶互补滤波解算姿态（计算 Roll 角）
 * @param   无
 */
void first_order_filtering(void)
{
    // 获取传感器数据
    imu963ra_get_acc();
    imu963ra_get_gyro();

    // 读取 IMU 数据
    int16_t gx = imu963ra_gyro_x;
    int16_t gy = imu963ra_gyro_y;
    int16_t gz = imu963ra_gyro_z;
    int16_t ax = imu963ra_acc_x;
    int16_t ay = imu963ra_acc_y;
    int16_t az = imu963ra_acc_z;

    // 陀螺仪数据处理：去除小值噪声
    if (abs(gx) < 5) gx = 0;
    if (abs(gy) < 5) gy = 0;
    if (abs(gz) < 5) gz = 0;

    // 计算加速度计的 Roll 角（横滚角）
    // atan2(ay, az)：根据 Y 轴和 Z 轴加速度计算 Roll 角
    float acc_angle = atan2(ay, az) * 180.0f / PI;

    // 计算陀螺仪的角度增量（弧度转角度）
    float gyro_rate = gy * gyro_ration;  // 陀螺仪原始数据转弧度/秒
    float gyro_delta = gyro_rate * cycle_T;  // 积分得到角度增量（弧度转角度）

    // 一阶互补滤波融合
    angle_temp += gyro_delta;
    roll = acc_ration * acc_angle + (1.0f - acc_ration) * angle_temp;

    // 角度归一化到 [-180, 180) 度
    while (roll >= 180.0f) roll -= 360.0f;
    while (roll < -180.0f) roll += 360.0f;
		roll_offset=roll+OFFSET;
}


