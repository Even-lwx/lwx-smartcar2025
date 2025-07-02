#include "zf_common_headfile.h"

#include "imu660rb.h"

// 全局滤波器实例
IMUFilter imu_filter;

// 校准状态标志
static uint8_t calibration_phase = 1; // 1表示正在校准

void imu_init(void)
{
    gpio_init(LED1, GPO, GPIO_HIGH, GPO_PUSH_PULL); // 初始化 LED1
    
    // 初始化IMU传感器
    while(1) {
        if(imu963ra_init()) {
            printf("\r\nIMU963RA init error.");
        } else {
            break;
        }
        gpio_toggle_level(LED1); // 翻转LED
    }
    
    // 初始化姿态滤波器
    IMUFilter_Init(&imu_filter, 0.98f);
    
    // 初始化5ms定时器
    pit_ms_init(IMU_PIT, 5);
    
    printf("IMU system initialized.\n");
}






// 5ms定时器中断处理函数
void pit_imu_handler(void)
{
    // 获取传感器数据
    imu963ra_get_acc();  // 更新全局变量: imu963ra_acc_x, y, z
    imu963ra_get_gyro(); // 更新全局变量: imu963ra_gyro_x, y, z
    
    // 校准阶段（设备需保持静止）
    if (calibration_phase) {
        IMUFilter_CalibrateGyro(&imu_filter, 
                               imu963ra_gyro_x, 
                               imu963ra_gyro_y, 
                               imu963ra_gyro_z);
        
        // 检查校准是否完成
        if (imu_filter.is_calibrated) {
            calibration_phase = 0;
            printf("IMU calibration complete. Starting attitude tracking.\n");
        }
    } 
    // 正常姿态更新阶段
    else {
        IMUFilter_Update(&imu_filter,
                        imu963ra_acc_x, imu963ra_acc_y, imu963ra_acc_z,
                        imu963ra_gyro_x, imu963ra_gyro_y, imu963ra_gyro_z);
    }
}


// 获取当前欧拉角（主循环中调用）
void get_attitude_angles(float* roll, float* pitch, float* yaw)
{
    IMUFilter_GetEulerAngles(&imu_filter, roll, pitch, yaw);
}

















// 常数定义
#define DT 0.005f        // 5ms时间间隔 (固定值)
#define GRAVITY 9.80665f // 重力加速度
#define DEG2RAD 0.01745329252f  // 度转弧度
#define RAD2DEG 57.2957795131f  // 弧度转度

// 初始化滤波器
void IMUFilter_Init(IMUFilter* filter, float alpha) {
    filter->alpha = alpha;
    filter->roll = 0.0f;
    filter->pitch = 0.0f;
    filter->yaw = 0.0f;
    filter->gx_bias = 0.0f;
    filter->gy_bias = 0.0f;
    filter->gz_bias = 0.0f;
    filter->is_calibrated = 0;
    filter->is_first_update = 1;
}

// 陀螺仪校准函数（在静止状态下调用）
void IMUFilter_CalibrateGyro(IMUFilter* filter, 
                            float gyro_x, float gyro_y, float gyro_z) {
    // 累加校准数据
    static int sample_count = 0;
    static float sum_x = 0, sum_y = 0, sum_z = 0;
    
    if (sample_count < 500) { // 500次采样（2.5秒）
        sum_x += gyro_x;
        sum_y += gyro_y;
        sum_z += gyro_z;
        sample_count++;
    } else if (!filter->is_calibrated) {
        // 计算平均值作为零偏
        filter->gx_bias = sum_x / sample_count;
        filter->gy_bias = sum_y / sample_count;
        filter->gz_bias = sum_z / sample_count;
        filter->is_calibrated = 1;
        
        // 重置累加器
        sum_x = 0;
        sum_y = 0;
        sum_z = 0;
        sample_count = 0;
        
        // 可选：打印校准结果
        printf("Gyro Calibration Complete:\n");
        printf("X bias: %.4f\n", filter->gx_bias);
        printf("Y bias: %.4f\n", filter->gy_bias);
        printf("Z bias: %.4f\n", filter->gz_bias);
    }
}

// 快速atan2近似函数（优化计算速度）
static float fast_atan2(float y, float x) {
    // 使用有理函数近似，精度约0.005rad
    //const float PI = 3.14159265359f;
    const float PI_2 = 1.57079632679f;
    const float PI_4 = 0.78539816339f;
    
    float abs_y = fabsf(y) + 1e-10f; // 避免除零
    float r, angle;
    
    if (x >= 0) {
        r = (x - abs_y) / (x + abs_y);
        angle = PI_4 - r*(PI_4 + 0.273f*r*r);
    } else {
        r = (x + abs_y) / (abs_y - x);
        angle = PI_2 + PI_4 - r*(PI_4 + 0.273f*r*r);
    }
    return y < 0 ? -angle : angle;
}

// 姿态更新函数（在5ms定时器中调用）
void IMUFilter_Update(IMUFilter* filter, 
                     float acc_x, float acc_y, float acc_z,
                     float gyro_x, float gyro_y, float gyro_z) {
    // 单位转换 (加速度g→m/s2, 陀螺仪deg/s→rad/s)
    float ax = acc_x * GRAVITY;
    float ay = acc_y * GRAVITY;
    float az = acc_z * GRAVITY;
    
    // 应用陀螺仪校准
    float gx = (gyro_x - filter->gx_bias) * DEG2RAD;
    float gy = (gyro_y - filter->gy_bias) * DEG2RAD;
    float gz = (gyro_z - filter->gz_bias) * DEG2RAD;
    
    if (filter->is_first_update) {
        // 初次更新使用加速度计初始化姿态
        filter->roll = fast_atan2(ay, az);
        filter->pitch = fast_atan2(-ax, sqrtf(ay*ay + az*az));
        filter->is_first_update = 0;
    } else {
        // 陀螺仪积分
        float roll_gyro = filter->roll + gx * DT;
        float pitch_gyro = filter->pitch + gy * DT;
        filter->yaw += gz * DT;

        // 加速度计测量
        float roll_acc = fast_atan2(ay, az);
        float pitch_acc = fast_atan2(-ax, sqrtf(ay*ay + az*az));

        // 互补滤波融合
        filter->roll = filter->alpha * roll_gyro + (1 - filter->alpha) * roll_acc;
        filter->pitch = filter->alpha * pitch_gyro + (1 - filter->alpha) * pitch_acc;
    }
}

// 获取欧拉角（角度制）
void IMUFilter_GetEulerAngles(IMUFilter* filter, 
                             float* roll, float* pitch, float* yaw) {
    *roll = filter->roll * RAD2DEG;
    *pitch = filter->pitch * RAD2DEG;
    *yaw = filter->yaw * RAD2DEG;
}


