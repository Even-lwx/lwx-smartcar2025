/*
		这部分代码是采用四元数解算姿态角的
*/

//#include "zf_common_headfile.h"
//#include <math.h>
//#include "imu660rb.h"

//// 全局变量和结构体定义
//#define RtA 57.2957795f
//#define Ki beta *beta
//#define Kp 2 * beta
//#define DT 0.005f // 采样时间
//#define beta 2.146 / TAU
//#define TAU tau
//#define OFFSET 0

float tau; // 此变量通过菜单设置

//typedef struct
//{
//    float q0, q1, q2, q3;
//    float exInt, eyInt, ezInt;
//} Quater;

//static Quater q = {1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
//float roll, pitch, yaw,roll_offset; // 欧拉角（弧度）

//// 校准相关变量
//static uint8_t calibration_phase = 1;
//static uint32_t calib_count = 0;
//static float gyro_bias[3] = {0};

//void imu660rb_init(void)
//{
//    gpio_init(LED1, GPO, GPIO_HIGH, GPO_PUSH_PULL);

//    // 初始化IMU传感器
//    while (1)
//    {
//        if (imu963ra_init())
//        {
//            printf("\r\nIMU963RA init error.");
//        }
//        else
//        {
//            break;
//        }
//        gpio_toggle_level(LED1);
//    }


//    // 重置校准状态
//    calibration_phase = 1;
//    calib_count = 0;
//    memset(gyro_bias, 0, sizeof(gyro_bias));
//}

//// Mahony滤波算法更新函数
//void MahonyUpdate(float ax, float ay, float az, float gx, float gy, float gz)
//{
//    // 归一化加速度
//    float norm = sqrtf(ax * ax + ay * ay + az * az);
//    if (norm > 1e-6f)
//    {
//        ax /= norm;
//        ay /= norm;
//        az /= norm;
//    }
//    else
//    {
//        return; // 无效数据跳过
//    }

//    // 计算重力方向
//    float gravity_x = 2.0f * (q.q1 * q.q3 - q.q0 * q.q2);
//    float gravity_y = 2.0f * (q.q0 * q.q1 + q.q2 * q.q3);
//    float gravity_z = q.q0 * q.q0 - q.q1 * q.q1 - q.q2 * q.q2 + q.q3 * q.q3;

//    // 计算误差（叉积）
//    float error_x = ay * gravity_z - az * gravity_y;
//    float error_y = az * gravity_x - ax * gravity_z;
//    float error_z = ax * gravity_y - ay * gravity_x;

//    // 积分误差
//    q.exInt += error_x * Ki;
//    q.eyInt += error_y * Ki;
//    q.ezInt += error_z * Ki;

//    // 修正陀螺仪数据
//    gx += Kp * error_x + q.exInt;
//    gy += Kp * error_y + q.eyInt;
//    // gz += Kp * error_z + q.ezInt;

//    // 四元数微分方程
//    float q0_dot = (-q.q1 * gx - q.q2 * gy - q.q3 * gz) * DT;
//    float q1_dot = (q.q0 * gx - q.q3 * gy + q.q2 * gz) * DT;
//    float q2_dot = (q.q3 * gx + q.q0 * gy - q.q1 * gz) * DT;
//    float q3_dot = (-q.q2 * gx + q.q1 * gy + q.q0 * gz) * DT;

//    // 更新四元数
//    q.q0 += q0_dot;
//    q.q1 += q1_dot;
//    q.q2 += q2_dot;
//    q.q3 += q3_dot;

//    // 四元数归一化
//    norm = sqrtf(q.q0 * q.q0 + q.q1 * q.q1 + q.q2 * q.q2 + q.q3 * q.q3);
//    if (norm > 1e-6f)
//    {
//        float inv_norm = 1.0f / norm;
//        q.q0 *= inv_norm;
//        q.q1 *= inv_norm;
//        q.q2 *= inv_norm;
//        q.q3 *= inv_norm;
//    }

//    // 计算欧拉角
//    float q0q0 = q.q0 * q.q0;
//    float q1q1 = q.q1 * q.q1;
//    float q2q2 = q.q2 * q.q2;
//    float q3q3 = q.q3 * q.q3;

//    roll = atan2f(2.0f * (q.q2 * q.q3 + q.q0 * q.q1),
//                  q0q0 - q1q1 - q2q2 + q3q3);
//    pitch = asinf(-2.0f * (q.q1 * q.q3 - q.q0 * q.q2));
//    yaw = atan2f(2.0f * (q.q1 * q.q2 + q.q0 * q.q3),
//                 q0q0 + q1q1 - q2q2 - q3q3);
//    // 转换为度
//    roll *= RtA;
//		roll_offset=roll+OFFSET;
//    pitch *= RtA;
//    yaw *= RtA;
//}

//// 5ms定时器中断处理函数
//void pit_imu660rb_handler(void)
//{
//    // 获取传感器数据
//    imu963ra_get_acc();
//    imu963ra_get_gyro();

//    if (calibration_phase)
//    {
//        // 校准阶段：累积陀螺仪数据
//        gyro_bias[0] += imu963ra_gyro_x;
//        gyro_bias[1] += imu963ra_gyro_y;
//        gyro_bias[2] += imu963ra_gyro_z;
//        calib_count++;

//        // 完成校准（假设0.5秒：100个样本）
//        if (calib_count >= 100)
//        {
//            gyro_bias[0] /= calib_count;
//            gyro_bias[1] /= calib_count;
//            gyro_bias[2] /= calib_count;
//            calibration_phase = 0;
//            // printf("IMU calibration complete.\n");
//        }
//    }
//    else
//    {
//        // 正常姿态更新
//        // 转换并校准陀螺仪数据 (单位: rad/s)
//        float gx = (imu963ra_gyro_x - gyro_bias[0]) / 14.3f / RtA;
//        float gy = (imu963ra_gyro_y - gyro_bias[1]) / 14.3f / RtA;
//        float gz = (imu963ra_gyro_z) / 14.3f / RtA;

//        // 转换加速度数据 (单位: g)
//        float ax = imu963ra_acc_x / 16393.0f;
//        float ay = imu963ra_acc_y / 16393.0f;
//        float az = imu963ra_acc_z / 16393.0f;

//        // 执行Mahony滤波更新
//        MahonyUpdate(ax, ay, az, gx, gy, gz);
//    }
//}


#include "zf_common_headfile.h"
#include <math.h>
#include "imu660rb.h"


#define OFFSET -30
float roll = 0.0f;   // 横滚角
float roll_offset=0.0f;
float angle_temp = 0.0f;



float gyro_ration = 0.0174533f; // 度转弧度
float acc_ration = 0.95f;       // 加速度权重(一阶互补滤波系数)
float cycle_T = 0.005f;          // 采样周期(5ms)


void imu660rb_init(void)
{

    // 初始化IMU传感器
    while (1)
    {
        if (imu963ra_init())
        {
            ips200_show_string(0,0,"IMU init error");
        }
        else
        {
						ips200_show_string(0,0,"IMU init success");
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










