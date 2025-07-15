#include "zf_common_headfile.h"

extern volatile uint8 Go;             //发车阶段的标志位
extern uint8 image_two_value[MT9V03X_H][MT9V03X_W];//二值化后的原数组
extern volatile float Err;//摄像头误差
extern volatile int Steer_Angle;    //舵机打角值
extern volatile int Speed_Left_Real;//左右轮实际值
extern volatile int Speed_Right_Real;//左右轮实际值
extern volatile int Speed_Left_Set; //左右轮设定值
extern volatile int Speed_Right_Set;//左右轮设定值
extern volatile uint8 Stop_Flag;    //刹车标
extern volatile uint8 Img_Disappear_Flag; //0图像消失标志位
extern volatile float ADC_Nor_Value[ADC_NUM];//adc每个通道归一化后的值
int Threshold=0;

//环岛控制
extern volatile int Island_State;     //环岛状态标志
extern volatile int Ramp_Flag;        //坡道状态

//横断控制
extern volatile uint8 Barricade_Flag;
extern volatile uint8 Electromagnet_Flag;//0是摄像头，1是电磁

extern volatile int Left_Island_Flag; //左右环岛标志
extern volatile int Right_Island_Flag;//左右环岛标志

int main (void)
{
    clock_init(SYSTEM_CLOCK_144M);// 初始化芯片时钟 工作频率为 144MHz
    debug_init();
    InitMH();  //前瞻模糊初始化
    Key_Init();//按键初始化
    ips200_init(IPS200_TYPE_PARALLEL8);
    pwm_init(STEER_PIN, 50, STEER_MID);
    adc_init(ADC1_IN12_C2, ADC_12BIT);
    adc_init(ADC1_IN13_C3, ADC_12BIT);
    adc_init(ADC1_IN14_C4, ADC_12BIT);
    adc_init(ADC1_IN15_C5, ADC_12BIT);
    encoder_dir_init(ENCODER_L_TIM,ENCODER_L_CH1,ENCODER_L_CH2);      //编码器初始化  左后轮
    encoder_dir_init(ENCODER_R_TIM,ENCODER_R_CH1,ENCODER_R_CH2);      //编码器初始化  右后轮
    uart_init(UART_7,115200,UART7_MAP3_TX_E12,UART7_MAP3_RX_E13);//图传蓝牙共用一个口，使用时修改波特率即可
    uart_rx_interrupt(UART_7, ZF_ENABLE);// 开启 UART_INDEX 的接收中断
    mt9v03x_init();
    mt9v03x_set_exposure_time(512);
    imu660ra_init();
    dm1xa_init();//有来有去测距// 初始化默认 Debug UART
    Zero_Point_Detect();
    Read_Flash();//读取内部flash中的数据
    pwm_init(MOTOR01_SPEED_PIN, 17000, 0);//左速度
    gpio_init(MOTOR01_DIR_PIN, GPO, 1, GPIO_PIN_CONFIG);//01左电机，0是倒转，1是正转
    pwm_init(MOTOR02_SPEED_PIN, 17000, 0);//右速度
    gpio_init(MOTOR02_DIR_PIN, GPO, 0, GPIO_PIN_CONFIG);//02右电机，1反转，0正转
    gpio_init(BEEP_PIN, GPO, 0, GPIO_PIN_CONFIG);  //蜂鸣器，1是响，0是不响
    gpio_init(CHARGE_PIN, GPO, 0, GPIO_PIN_CONFIG);//充电使能口，0是不充电，1是充电
    while( dl1a_init());//测距
    ////TIM1给了编码器计数使用，2是舵机pwm产生，3是陀螺仪、电磁，ui的周期控制 4是电机pwm产生，
    ////5是标志位控制，6是超声波测距使能信号，编码器读数，速度控制，7是有来有去测距需要时钟，9是编码器
    pit_ms_init(TIM3_PIT, 10);//陀螺仪，电磁，ui 10毫秒中断
    pit_ms_init(TIM5_PIT, 50);//标志位控制50毫秒
    pit_ms_init(TIM6_PIT, 10);//超声波使能，编码器读数，速度闭环 速度10毫秒
    interrupt_set_priority(TIM3_IRQn, 2);//陀螺仪优先级2//数字越小，优先级越高
    interrupt_set_priority(TIM5_IRQn, 3);//标志位优先级3
    interrupt_set_priority(TIM6_IRQn, 1);//速度优先级1
    while(1)//一圈while基本10ms左右，理论上在15毫秒左右，都在可接受范围之内
    {
        if(mt9v03x_finish_flag)//图像获取
        {
            if(Img_Disappear_Flag==0)//图没有丢，正常计算阈值
            {
                Threshold=My_Adapt_Threshold(mt9v03x_image[0],MT9V03X_W, MT9V03X_H);//大津计算阈值
            }
            else
            {
              //出界后不算阈值，确保出去之后屏幕是黑的，防止出现雪花屏
            }
            Image_Binarization(Threshold);//图像二值化
            mt9v03x_finish_flag=0;//标志位清除，自行准备采集下一帧数据
        }
        Longest_White_Column();//最长白列巡线

        //元素放在下面
        if(Go==7)//GO==7才是常规寻迹，不在7里面，只做最简单的巡线，不去识别元素
        {
            Zebra_Stripes_Detect();
            Ramp_Detect();
            Barricade_Detect();
            Break_Road_Detect();
            Island_Detect();
            Cross_Detect();
            Img_Disappear_Detect();
            ////Straight_Detect();//赛场上估计没有长直道
        }
        //元素放在上面
        Err=Err_Sum();      //误差计算
        Direction_Control();//方向控制

//        Image_Flag_Show(MT9V03X_W,image_two_value,Island_State);//屏幕上打出环岛状态
//        Flag_Show_101(MT9V03X_W,image_two_value,Ramp_Flag);//屏幕上打出环岛状态Ramp_Flag
//        Flag_Show_102(MT9V03X_W,image_two_value,Electromagnet_Flag);//屏幕上打出环岛状态
//        Flag_Show_202(MT9V03X_W,image_two_value,Barricade_Flag);//屏幕上打出环岛状态
//        Show_Boundry();
//        Zw_SendImage(image_two_value[0]);
        if(SWITCH_1==1&&SWITCH_2==0)//通过拨码开关来控制屏幕开关
        {
               User_Interface();
        }
    }
}

