#include "zf_common_headfile.h"

#include "imu660rb.h"

// ȫ���˲���ʵ��
IMUFilter imu_filter;

// У׼״̬��־
static uint8_t calibration_phase = 1; // 1��ʾ����У׼

void imu_init(void)
{
    gpio_init(LED1, GPO, GPIO_HIGH, GPO_PUSH_PULL); // ��ʼ�� LED1
    
    // ��ʼ��IMU������
    while(1) {
        if(imu963ra_init()) {
            printf("\r\nIMU963RA init error.");
        } else {
            break;
        }
        gpio_toggle_level(LED1); // ��תLED
    }
    
    // ��ʼ����̬�˲���
    IMUFilter_Init(&imu_filter, 0.98f);
    
    // ��ʼ��5ms��ʱ��
    pit_ms_init(IMU_PIT, 5);
    
    printf("IMU system initialized.\n");
}






// 5ms��ʱ���жϴ�����
void pit_imu_handler(void)
{
    // ��ȡ����������
    imu963ra_get_acc();  // ����ȫ�ֱ���: imu963ra_acc_x, y, z
    imu963ra_get_gyro(); // ����ȫ�ֱ���: imu963ra_gyro_x, y, z
    
    // У׼�׶Σ��豸�豣�־�ֹ��
    if (calibration_phase) {
        IMUFilter_CalibrateGyro(&imu_filter, 
                               imu963ra_gyro_x, 
                               imu963ra_gyro_y, 
                               imu963ra_gyro_z);
        
        // ���У׼�Ƿ����
        if (imu_filter.is_calibrated) {
            calibration_phase = 0;
            printf("IMU calibration complete. Starting attitude tracking.\n");
        }
    } 
    // ������̬���½׶�
    else {
        IMUFilter_Update(&imu_filter,
                        imu963ra_acc_x, imu963ra_acc_y, imu963ra_acc_z,
                        imu963ra_gyro_x, imu963ra_gyro_y, imu963ra_gyro_z);
    }
}


// ��ȡ��ǰŷ���ǣ���ѭ���е��ã�
void get_attitude_angles(float* roll, float* pitch, float* yaw)
{
    IMUFilter_GetEulerAngles(&imu_filter, roll, pitch, yaw);
}

















// ��������
#define DT 0.005f        // 5msʱ���� (�̶�ֵ)
#define GRAVITY 9.80665f // �������ٶ�
#define DEG2RAD 0.01745329252f  // ��ת����
#define RAD2DEG 57.2957795131f  // ����ת��

// ��ʼ���˲���
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

// ������У׼�������ھ�ֹ״̬�µ��ã�
void IMUFilter_CalibrateGyro(IMUFilter* filter, 
                            float gyro_x, float gyro_y, float gyro_z) {
    // �ۼ�У׼����
    static int sample_count = 0;
    static float sum_x = 0, sum_y = 0, sum_z = 0;
    
    if (sample_count < 500) { // 500�β�����2.5�룩
        sum_x += gyro_x;
        sum_y += gyro_y;
        sum_z += gyro_z;
        sample_count++;
    } else if (!filter->is_calibrated) {
        // ����ƽ��ֵ��Ϊ��ƫ
        filter->gx_bias = sum_x / sample_count;
        filter->gy_bias = sum_y / sample_count;
        filter->gz_bias = sum_z / sample_count;
        filter->is_calibrated = 1;
        
        // �����ۼ���
        sum_x = 0;
        sum_y = 0;
        sum_z = 0;
        sample_count = 0;
        
        // ��ѡ����ӡУ׼���
        printf("Gyro Calibration Complete:\n");
        printf("X bias: %.4f\n", filter->gx_bias);
        printf("Y bias: %.4f\n", filter->gy_bias);
        printf("Z bias: %.4f\n", filter->gz_bias);
    }
}

// ����atan2���ƺ������Ż������ٶȣ�
static float fast_atan2(float y, float x) {
    // ʹ�����������ƣ�����Լ0.005rad
    //const float PI = 3.14159265359f;
    const float PI_2 = 1.57079632679f;
    const float PI_4 = 0.78539816339f;
    
    float abs_y = fabsf(y) + 1e-10f; // �������
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

// ��̬���º�������5ms��ʱ���е��ã�
void IMUFilter_Update(IMUFilter* filter, 
                     float acc_x, float acc_y, float acc_z,
                     float gyro_x, float gyro_y, float gyro_z) {
    // ��λת�� (���ٶ�g��m/s2, ������deg/s��rad/s)
    float ax = acc_x * GRAVITY;
    float ay = acc_y * GRAVITY;
    float az = acc_z * GRAVITY;
    
    // Ӧ��������У׼
    float gx = (gyro_x - filter->gx_bias) * DEG2RAD;
    float gy = (gyro_y - filter->gy_bias) * DEG2RAD;
    float gz = (gyro_z - filter->gz_bias) * DEG2RAD;
    
    if (filter->is_first_update) {
        // ���θ���ʹ�ü��ٶȼƳ�ʼ����̬
        filter->roll = fast_atan2(ay, az);
        filter->pitch = fast_atan2(-ax, sqrtf(ay*ay + az*az));
        filter->is_first_update = 0;
    } else {
        // �����ǻ���
        float roll_gyro = filter->roll + gx * DT;
        float pitch_gyro = filter->pitch + gy * DT;
        filter->yaw += gz * DT;

        // ���ٶȼƲ���
        float roll_acc = fast_atan2(ay, az);
        float pitch_acc = fast_atan2(-ax, sqrtf(ay*ay + az*az));

        // �����˲��ں�
        filter->roll = filter->alpha * roll_gyro + (1 - filter->alpha) * roll_acc;
        filter->pitch = filter->alpha * pitch_gyro + (1 - filter->alpha) * pitch_acc;
    }
}

// ��ȡŷ���ǣ��Ƕ��ƣ�
void IMUFilter_GetEulerAngles(IMUFilter* filter, 
                             float* roll, float* pitch, float* yaw) {
    *roll = filter->roll * RAD2DEG;
    *pitch = filter->pitch * RAD2DEG;
    *yaw = filter->yaw * RAD2DEG;
}


