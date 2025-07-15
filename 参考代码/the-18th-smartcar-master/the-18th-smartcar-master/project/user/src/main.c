#include "zf_common_headfile.h"

extern volatile uint8 Go;             //�����׶εı�־λ
extern uint8 image_two_value[MT9V03X_H][MT9V03X_W];//��ֵ�����ԭ����
extern volatile float Err;//����ͷ���
extern volatile int Steer_Angle;    //������ֵ
extern volatile int Speed_Left_Real;//������ʵ��ֵ
extern volatile int Speed_Right_Real;//������ʵ��ֵ
extern volatile int Speed_Left_Set; //�������趨ֵ
extern volatile int Speed_Right_Set;//�������趨ֵ
extern volatile uint8 Stop_Flag;    //ɲ����
extern volatile uint8 Img_Disappear_Flag; //0ͼ����ʧ��־λ
extern volatile float ADC_Nor_Value[ADC_NUM];//adcÿ��ͨ����һ�����ֵ
int Threshold=0;

//��������
extern volatile int Island_State;     //����״̬��־
extern volatile int Ramp_Flag;        //�µ�״̬

//��Ͽ���
extern volatile uint8 Barricade_Flag;
extern volatile uint8 Electromagnet_Flag;//0������ͷ��1�ǵ��

extern volatile int Left_Island_Flag; //���һ�����־
extern volatile int Right_Island_Flag;//���һ�����־

int main (void)
{
    clock_init(SYSTEM_CLOCK_144M);// ��ʼ��оƬʱ�� ����Ƶ��Ϊ 144MHz
    debug_init();
    InitMH();  //ǰհģ����ʼ��
    Key_Init();//������ʼ��
    ips200_init(IPS200_TYPE_PARALLEL8);
    pwm_init(STEER_PIN, 50, STEER_MID);
    adc_init(ADC1_IN12_C2, ADC_12BIT);
    adc_init(ADC1_IN13_C3, ADC_12BIT);
    adc_init(ADC1_IN14_C4, ADC_12BIT);
    adc_init(ADC1_IN15_C5, ADC_12BIT);
    encoder_dir_init(ENCODER_L_TIM,ENCODER_L_CH1,ENCODER_L_CH2);      //��������ʼ��  �����
    encoder_dir_init(ENCODER_R_TIM,ENCODER_R_CH1,ENCODER_R_CH2);      //��������ʼ��  �Һ���
    uart_init(UART_7,115200,UART7_MAP3_TX_E12,UART7_MAP3_RX_E13);//ͼ����������һ���ڣ�ʹ��ʱ�޸Ĳ����ʼ���
    uart_rx_interrupt(UART_7, ZF_ENABLE);// ���� UART_INDEX �Ľ����ж�
    mt9v03x_init();
    mt9v03x_set_exposure_time(512);
    imu660ra_init();
    dm1xa_init();//������ȥ���// ��ʼ��Ĭ�� Debug UART
    Zero_Point_Detect();
    Read_Flash();//��ȡ�ڲ�flash�е�����
    pwm_init(MOTOR01_SPEED_PIN, 17000, 0);//���ٶ�
    gpio_init(MOTOR01_DIR_PIN, GPO, 1, GPIO_PIN_CONFIG);//01������0�ǵ�ת��1����ת
    pwm_init(MOTOR02_SPEED_PIN, 17000, 0);//���ٶ�
    gpio_init(MOTOR02_DIR_PIN, GPO, 0, GPIO_PIN_CONFIG);//02�ҵ����1��ת��0��ת
    gpio_init(BEEP_PIN, GPO, 0, GPIO_PIN_CONFIG);  //��������1���죬0�ǲ���
    gpio_init(CHARGE_PIN, GPO, 0, GPIO_PIN_CONFIG);//���ʹ�ܿڣ�0�ǲ���磬1�ǳ��
    while( dl1a_init());//���
    ////TIM1���˱���������ʹ�ã�2�Ƕ��pwm������3�������ǡ���ţ�ui�����ڿ��� 4�ǵ��pwm������
    ////5�Ǳ�־λ���ƣ�6�ǳ��������ʹ���źţ��������������ٶȿ��ƣ�7��������ȥ�����Ҫʱ�ӣ�9�Ǳ�����
    pit_ms_init(TIM3_PIT, 10);//�����ǣ���ţ�ui 10�����ж�
    pit_ms_init(TIM5_PIT, 50);//��־λ����50����
    pit_ms_init(TIM6_PIT, 10);//������ʹ�ܣ��������������ٶȱջ� �ٶ�10����
    interrupt_set_priority(TIM3_IRQn, 2);//���������ȼ�2//����ԽС�����ȼ�Խ��
    interrupt_set_priority(TIM5_IRQn, 3);//��־λ���ȼ�3
    interrupt_set_priority(TIM6_IRQn, 1);//�ٶ����ȼ�1
    while(1)//һȦwhile����10ms���ң���������15�������ң����ڿɽ��ܷ�Χ֮��
    {
        if(mt9v03x_finish_flag)//ͼ���ȡ
        {
            if(Img_Disappear_Flag==0)//ͼû�ж�������������ֵ
            {
                Threshold=My_Adapt_Threshold(mt9v03x_image[0],MT9V03X_W, MT9V03X_H);//��������ֵ
            }
            else
            {
              //���������ֵ��ȷ����ȥ֮����Ļ�Ǻڵģ���ֹ����ѩ����
            }
            Image_Binarization(Threshold);//ͼ���ֵ��
            mt9v03x_finish_flag=0;//��־λ���������׼���ɼ���һ֡����
        }
        Longest_White_Column();//�����Ѳ��

        //Ԫ�ط�������
        if(Go==7)//GO==7���ǳ���Ѱ��������7���棬ֻ����򵥵�Ѳ�ߣ���ȥʶ��Ԫ��
        {
            Zebra_Stripes_Detect();
            Ramp_Detect();
            Barricade_Detect();
            Break_Road_Detect();
            Island_Detect();
            Cross_Detect();
            Img_Disappear_Detect();
            ////Straight_Detect();//�����Ϲ���û�г�ֱ��
        }
        //Ԫ�ط�������
        Err=Err_Sum();      //������
        Direction_Control();//�������

//        Image_Flag_Show(MT9V03X_W,image_two_value,Island_State);//��Ļ�ϴ������״̬
//        Flag_Show_101(MT9V03X_W,image_two_value,Ramp_Flag);//��Ļ�ϴ������״̬Ramp_Flag
//        Flag_Show_102(MT9V03X_W,image_two_value,Electromagnet_Flag);//��Ļ�ϴ������״̬
//        Flag_Show_202(MT9V03X_W,image_two_value,Barricade_Flag);//��Ļ�ϴ������״̬
//        Show_Boundry();
//        Zw_SendImage(image_two_value[0]);
        if(SWITCH_1==1&&SWITCH_2==0)//ͨ�����뿪����������Ļ����
        {
               User_Interface();
        }
    }
}

