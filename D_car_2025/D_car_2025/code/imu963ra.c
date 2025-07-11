#include "zf_common_headfile.h"
#include <math.h>
#include "imu963ra.h"

void imu_init(void)
{

    // 初始化IMU传感器
   while(1)
    {
        if(imu963ra_init())
        {
           ips200_show_string(10,140,"imu init error");                            // IMU963RA 初始化失败
        }
        else
        {
						ips200_show_string(10,140,"imu init success");  
            break;
        }
                                                  
    }
		
}

uint8 gyro_ration = 4;
uint8 acc_ration = 4;
float filtering_angle = 0; // 解算出的角度
float angle_temp;          // 角度计算中间变量
float call_cycle = 0.002f;

int16_t gx;
int16_t gy;
int16_t gz;
int16_t ax;
int16_t ay;
int16_t az;

void first_order_complementary_filtering(void)
{
    // 获取传感器数据
    imu963ra_get_acc();
    imu963ra_get_gyro();

    // 读取 IMU 数据
    gx = imu963ra_gyro_x-3 ;
    gy = imu963ra_gyro_y+12 ;
    gz = imu963ra_gyro_z+4 ;
    ax = imu963ra_acc_x;
    ay = imu963ra_acc_y;
    az = imu963ra_acc_z;
		

    // 陀螺仪数据处理：去除小值噪声
    if (abs(gx) < 5)
        gx = 0;
    if (abs(gy) < 5)
        gy = 0;
    if (abs(gz) < 5)
        gz = 0;
		
    float gyro_temp;
    float acc_temp;
    gyro_temp = gx * gyro_ration;
    acc_temp = (ay - angle_temp) * acc_ration;
    angle_temp += ((gyro_temp + acc_temp) * call_cycle);
    filtering_angle = angle_temp-3900 ;



}

// #define OFFSET -24
// float roll = 0.0f;   // 横滚角
// float roll_offset=0.0f;
// float yaw =0.0f;
// float angle_roll_temp = 0.0f;
// float angle_yaw_temp = 0.0f;
// float gyro_ration = 0.0174533f; // 度转弧度
// float acc_ration = 0.85f;       // 加速度权重(一阶互补滤波系数) 0.7~0.9
// float cycle_T = 0.002f;          // 采样周期(5ms)
//  /**
//   * @brief   一阶互补滤波解算姿态（计算 Roll yaw 角）
//   * @param   无
//   */
//  void first_order_filtering(void)
//  { // 获取传感器数据
//      imu963ra_get_acc();
//      imu963ra_get_gyro();

//     // 读取 IMU 数据
//     int16_t gx = imu963ra_gyro_x - 5;
//     int16_t gy = imu963ra_gyro_y + 9;
//     int16_t gz = imu963ra_gyro_z + 8;
//     int16_t ax = imu963ra_acc_x;
//     int16_t ay = imu963ra_acc_y;
//     int16_t az = imu963ra_acc_z;

//     // 陀螺仪数据处理：去除小值噪声
//     if (abs(gx) < 5)
//         gx = 0;
//     if (abs(gy) < 5)
//         gy = 0;
//     if (abs(gz) < 5)
//         gz = 0;

//
//     // 计算加速度计的 Roll 角（横滚角）
//     float acc_roll = atan2(ay, az) * 180.0f / PI;

//     // 计算陀螺仪的角度增量（弧度转角度）
//     float gyro_roll_rate = gy * gyro_ration;  // Y轴陀螺仪对应Roll角速率
//     float gyro_yaw_rate = gz * gyro_ration;   // Z轴陀螺仪对应Yaw角速率
//
//     float gyro_roll_delta = gyro_roll_rate * cycle_T;  // Roll角增量
//     float gyro_yaw_delta = gyro_yaw_rate * cycle_T;   // Yaw角增量

//     // 一阶互补滤波融合 - Roll角
//     angle_roll_temp += gyro_roll_delta;
//     roll = acc_ration * acc_roll + (1.0f - acc_ration) * angle_roll_temp;

//     // 计算加速度计的初始Yaw角（需要结合磁力计数据才准确）
//     float acc_yaw = atan2(-ax, ay) * 180.0f / PI;
//
//     // 一阶互补滤波融合 - Yaw角
//     angle_yaw_temp += gyro_yaw_delta;
//     yaw = acc_ration * acc_yaw + (1.0f - acc_ration) * angle_yaw_temp;

//     // 角度归一化到 [-180, 180) 度
//     while (roll >= 180.0f) roll -= 360.0f;
//     while (roll < -180.0f) roll += 360.0f;
//
//     while (yaw >= 180.0f) yaw -= 360.0f;
//     while (yaw < -180.0f) yaw += 360.0f;

//     // 应用偏移量
//     roll_offset = roll +OFFSET;
// }
