#include "zf_common_headfile.h"
#include <math.h>
#include "imu963ra.h"



#define OFFSET 0
float roll = 0.0f;   // �����
float roll_offset=0.0f;
float angle_temp = 0.0f;



float gyro_ration = 0.0174533f; // ��ת����
float acc_ration = 0.95f;       // ���ٶ�Ȩ��(һ�׻����˲�ϵ��)
float cycle_T = 0.005f;          // ��������(5ms)


void imu_init(void)
{

    // ��ʼ��IMU������
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
 * @brief   һ�׻����˲�������̬������ Roll �ǣ�
 * @param   ��
 */
void first_order_filtering(void)
{
    // ��ȡ����������
    imu963ra_get_acc();
    imu963ra_get_gyro();

    // ��ȡ IMU ����
    int16_t gx = imu963ra_gyro_x;
    int16_t gy = imu963ra_gyro_y;
    int16_t gz = imu963ra_gyro_z;
    int16_t ax = imu963ra_acc_x;
    int16_t ay = imu963ra_acc_y;
    int16_t az = imu963ra_acc_z;

    // ���������ݴ���ȥ��Сֵ����
    if (abs(gx) < 5) gx = 0;
    if (abs(gy) < 5) gy = 0;
    if (abs(gz) < 5) gz = 0;

    // ������ٶȼƵ� Roll �ǣ�����ǣ�
    // atan2(ay, az)������ Y ��� Z ����ٶȼ��� Roll ��
    float acc_angle = atan2(ay, az) * 180.0f / PI;

    // ���������ǵĽǶ�����������ת�Ƕȣ�
    float gyro_rate = gy * gyro_ration;  // ������ԭʼ����ת����/��
    float gyro_delta = gyro_rate * cycle_T;  // ���ֵõ��Ƕ�����������ת�Ƕȣ�

    // һ�׻����˲��ں�
    angle_temp += gyro_delta;
    roll = acc_ration * acc_angle + (1.0f - acc_ration) * angle_temp;

    // �Ƕȹ�һ���� [-180, 180) ��
    while (roll >= 180.0f) roll -= 360.0f;
    while (roll < -180.0f) roll += 360.0f;
		roll_offset=roll+OFFSET;
}


