#include "zf_common_headfile.h"
#include <math.h>
#include "imu963ra.h"

void imu_init(void)
{

    // ��ʼ��IMU������
   while(1)
    {
        if(imu963ra_init())
        {
           ips200_show_string(10,140,"imu init error");                            // IMU963RA ��ʼ��ʧ��
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
float filtering_angle = 0; // ������ĽǶ�
float angle_temp;          // �Ƕȼ����м����
float call_cycle = 0.002f;

int16_t gx;
int16_t gy;
int16_t gz;
int16_t ax;
int16_t ay;
int16_t az;

void first_order_complementary_filtering(void)
{
    // ��ȡ����������
    imu963ra_get_acc();
    imu963ra_get_gyro();

    // ��ȡ IMU ����
    gx = imu963ra_gyro_x-3 ;
    gy = imu963ra_gyro_y+12 ;
    gz = imu963ra_gyro_z+4 ;
    ax = imu963ra_acc_x;
    ay = imu963ra_acc_y;
    az = imu963ra_acc_z;
		

    // ���������ݴ���ȥ��Сֵ����
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
// float roll = 0.0f;   // �����
// float roll_offset=0.0f;
// float yaw =0.0f;
// float angle_roll_temp = 0.0f;
// float angle_yaw_temp = 0.0f;
// float gyro_ration = 0.0174533f; // ��ת����
// float acc_ration = 0.85f;       // ���ٶ�Ȩ��(һ�׻����˲�ϵ��) 0.7~0.9
// float cycle_T = 0.002f;          // ��������(5ms)
//  /**
//   * @brief   һ�׻����˲�������̬������ Roll yaw �ǣ�
//   * @param   ��
//   */
//  void first_order_filtering(void)
//  { // ��ȡ����������
//      imu963ra_get_acc();
//      imu963ra_get_gyro();

//     // ��ȡ IMU ����
//     int16_t gx = imu963ra_gyro_x - 5;
//     int16_t gy = imu963ra_gyro_y + 9;
//     int16_t gz = imu963ra_gyro_z + 8;
//     int16_t ax = imu963ra_acc_x;
//     int16_t ay = imu963ra_acc_y;
//     int16_t az = imu963ra_acc_z;

//     // ���������ݴ���ȥ��Сֵ����
//     if (abs(gx) < 5)
//         gx = 0;
//     if (abs(gy) < 5)
//         gy = 0;
//     if (abs(gz) < 5)
//         gz = 0;

//
//     // ������ٶȼƵ� Roll �ǣ�����ǣ�
//     float acc_roll = atan2(ay, az) * 180.0f / PI;

//     // ���������ǵĽǶ�����������ת�Ƕȣ�
//     float gyro_roll_rate = gy * gyro_ration;  // Y�������Ƕ�ӦRoll������
//     float gyro_yaw_rate = gz * gyro_ration;   // Z�������Ƕ�ӦYaw������
//
//     float gyro_roll_delta = gyro_roll_rate * cycle_T;  // Roll������
//     float gyro_yaw_delta = gyro_yaw_rate * cycle_T;   // Yaw������

//     // һ�׻����˲��ں� - Roll��
//     angle_roll_temp += gyro_roll_delta;
//     roll = acc_ration * acc_roll + (1.0f - acc_ration) * angle_roll_temp;

//     // ������ٶȼƵĳ�ʼYaw�ǣ���Ҫ��ϴ��������ݲ�׼ȷ��
//     float acc_yaw = atan2(-ax, ay) * 180.0f / PI;
//
//     // һ�׻����˲��ں� - Yaw��
//     angle_yaw_temp += gyro_yaw_delta;
//     yaw = acc_ration * acc_yaw + (1.0f - acc_ration) * angle_yaw_temp;

//     // �Ƕȹ�һ���� [-180, 180) ��
//     while (roll >= 180.0f) roll -= 360.0f;
//     while (roll < -180.0f) roll += 360.0f;
//
//     while (yaw >= 180.0f) yaw -= 360.0f;
//     while (yaw < -180.0f) yaw += 360.0f;

//     // Ӧ��ƫ����
//     roll_offset = roll +OFFSET;
// }
