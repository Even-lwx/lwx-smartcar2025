/*********************************************************************************************************************
* CH32V307VCT6 Opensourec Library ����CH32V307VCT6 ��Դ�⣩��һ�����ڹٷ� SDK �ӿڵĵ�������Դ��
* Copyright (c) 2022 SEEKFREE ��ɿƼ�
*
* ���ļ���CH32V307VCT6 ��Դ���һ����
*
* CH32V307VCT6 ��Դ�� ��������
* �����Ը��������������ᷢ���� GPL��GNU General Public License���� GNUͨ�ù������֤��������
* �� GPL �ĵ�3�棨�� GPL3.0������ѡ��ģ��κκ����İ汾�����·�����/���޸���
*
* ����Դ��ķ�����ϣ�����ܷ������ã�����δ�������κεı�֤
* ����û�������������Ի��ʺ��ض���;�ı�֤
* ����ϸ����μ� GPL
*
* ��Ӧ�����յ�����Դ���ͬʱ�յ�һ�� GPL �ĸ���
* ���û�У������<https://www.gnu.org/licenses/>
*
* ����ע����
* ����Դ��ʹ�� GPL3.0 ��Դ���֤Э�� �����������Ϊ���İ汾
* �������Ӣ�İ��� libraries/doc �ļ����µ� GPL3_permission_statement.txt �ļ���
* ���֤������ libraries �ļ����� �����ļ����µ� LICENSE �ļ�
* ��ӭ��λʹ�ò����������� ���޸�����ʱ���뱣����ɿƼ��İ�Ȩ����������������
*
* �ļ�����          isr
* ��˾����          �ɶ���ɿƼ����޹�˾
* �汾��Ϣ          �鿴 libraries/doc �ļ����� version �ļ� �汾˵��
* ��������          MounRiver Studio V1.8.1
* ����ƽ̨          CH32V307VCT6
* ��������          https://seekfree.taobao.com/
*
* �޸ļ�¼
* ����                                      ����                             ��ע
* 2022-09-15        ��W            first version
********************************************************************************************************************/

#include "zf_common_headfile.h"
void NMI_Handler(void)       __attribute__((interrupt()));
void HardFault_Handler(void) __attribute__((interrupt()));

void USART1_IRQHandler(void) __attribute__((interrupt()));
void USART2_IRQHandler(void) __attribute__((interrupt()));
void USART3_IRQHandler(void) __attribute__((interrupt()));
void UART4_IRQHandler (void) __attribute__((interrupt()));
void UART5_IRQHandler (void) __attribute__((interrupt()));
void UART6_IRQHandler (void) __attribute__((interrupt()));
void UART7_IRQHandler (void) __attribute__((interrupt()));
void UART8_IRQHandler (void) __attribute__((interrupt()));
void DVP_IRQHandler (void) __attribute__((interrupt()));
//void TIM1_BRK_IRQHandler        (void)  __attribute__((interrupt()));
void TIM1_UP_IRQHandler         (void)  __attribute__((interrupt()));
//void TIM1_TRG_COM_IRQHandler    (void)  __attribute__((interrupt()));
//void TIM1_CC_IRQHandler         (void)  __attribute__((interrupt()));
void TIM2_IRQHandler            (void)  __attribute__((interrupt()));
void TIM3_IRQHandler            (void)  __attribute__((interrupt()));
void TIM4_IRQHandler            (void)  __attribute__((interrupt()));
void TIM5_IRQHandler            (void)  __attribute__((interrupt()));
void TIM6_IRQHandler            (void)  __attribute__((interrupt()));
void TIM7_IRQHandler            (void)  __attribute__((interrupt()));
//void TIM8_BRK_IRQHandler        (void)  __attribute__((interrupt()));
void TIM8_UP_IRQHandler         (void)  __attribute__((interrupt()));
//void TIM8_TRG_COM_IRQHandler    (void)  __attribute__((interrupt()));
//void TIM8_CC_IRQHandler         (void)  __attribute__((interrupt()));
//void TIM9_BRK_IRQHandler        (void)  __attribute__((interrupt()));
void TIM9_UP_IRQHandler         (void)  __attribute__((interrupt()));
//void TIM9_TRG_COM_IRQHandler    (void)  __attribute__((interrupt()));
//void TIM9_CC_IRQHandler         (void)  __attribute__((interrupt()));
//void TIM10_BRK_IRQHandler       (void)  __attribute__((interrupt()));
void TIM10_UP_IRQHandler        (void)  __attribute__((interrupt()));
//void TIM10_TRG_COM_IRQHandler   (void)  __attribute__((interrupt()));
//void TIM10_CC_IRQHandler        (void)  __attribute__((interrupt()));

void EXTI0_IRQHandler(void) __attribute__((interrupt()));
void EXTI1_IRQHandler(void) __attribute__((interrupt()));
void EXTI2_IRQHandler(void) __attribute__((interrupt()));
void EXTI3_IRQHandler(void) __attribute__((interrupt()));
void EXTI4_IRQHandler(void) __attribute__((interrupt()));
void EXTI9_5_IRQHandler(void) __attribute__((interrupt()));
void EXTI15_10_IRQHandler(void) __attribute__((interrupt()));

void USART1_IRQHandler(void)
{
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {

        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    }
}
void USART2_IRQHandler(void)
{
    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
    {


        USART_ClearITPendingBit(USART2, USART_IT_RXNE);
    }
}
void USART3_IRQHandler(void)
{
    if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
    {
#if DEBUG_UART_USE_INTERRUPT                                                    // ������� debug �����ж�
        debug_interrupr_handler();                                              // ���� debug ���ڽ��մ����� ���ݻᱻ debug ���λ�������ȡ
#endif                                                                          // ����޸��� DEBUG_UART_INDEX ����δ�����Ҫ�ŵ���Ӧ�Ĵ����ж�ȥ
        USART_ClearITPendingBit(USART3, USART_IT_RXNE);
    }
}
void UART4_IRQHandler (void)
{
    if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)
    {

        USART_ClearITPendingBit(UART4, USART_IT_RXNE);
    }
}
void UART5_IRQHandler (void)
{
    if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET)
    {
        camera_uart_handler();
        USART_ClearITPendingBit(UART5, USART_IT_RXNE);
    }
}
void UART6_IRQHandler (void)
{
    if(USART_GetITStatus(UART6, USART_IT_RXNE) != RESET)
    {

        USART_ClearITPendingBit(UART6, USART_IT_RXNE);
    }
}

fifo_struct uart_data_fifo;
uint8       fifo_get_data[64];
uint32      fifo_data_count = 0;
extern volatile uint8 Go;         //�����׶εı�־λ


//�ٶȱ���
volatile int Speed_Left_Real=0;   //����ʵ��ֵ
volatile int Speed_Right_Real=0;
//Ԫ�ر�־λ
volatile int Zebra_State=0;
extern volatile int Zebra_Stripes_Flag;//�����߱�־λ
extern volatile int Island_State; //����״̬��־
extern volatile int Ramp_Flag;    //�µ���־
extern volatile uint8 Stop_Flag;
extern volatile uint8 Barricade_Flag;//
extern volatile uint8 Go_Direction;   //0��,1����
extern volatile int Left_Island_Flag; //���һ�����־
extern volatile int Right_Island_Flag;//���һ�����־

void UART7_IRQHandler (void)
{
    if(USART_GetITStatus(UART7, USART_IT_RXNE) != RESET)
    {                                             // fifo ���ݸ���
        uint8 dat;
        uart_query_byte(UART_7, &dat);
        if(dat==0x01)
        {
            uart_write_byte(UART_7, 0x41);        //����7����0x41������ǰ�����յ�������Ϣ��׼������
            gpio_set_level(CHARGE_PIN, 0);//�����磬�ϵ�
            dat=0;
            Go=7;//go��7��ֱ������Ѱ����
        }
        USART_ClearITPendingBit(UART7, USART_IT_RXNE);
    }
}
void UART8_IRQHandler (void)
{
    if(USART_GetITStatus(UART8, USART_IT_RXNE) != RESET)
    {
        gps_uart_callback();
        USART_ClearITPendingBit(UART8, USART_IT_RXNE);
    }
}



void DVP_IRQHandler(void)
{
    if (DVP->IFR & RB_DVP_IF_FRM_DONE)
    {
        camera_dvp_handler();
        DVP->IFR &= ~RB_DVP_IF_FRM_DONE;
    }
}
void EXTI0_IRQHandler(void)
{
    if(SET == EXTI_GetITStatus(EXTI_Line0))
    {
        EXTI_ClearITPendingBit(EXTI_Line0);

    }
}

void EXTI1_IRQHandler(void)
{
    if(SET == EXTI_GetITStatus(EXTI_Line1))
    {
        EXTI_ClearITPendingBit(EXTI_Line1);

    }
}

void EXTI2_IRQHandler(void)
{
    if(SET == EXTI_GetITStatus(EXTI_Line2))
    {
        EXTI_ClearITPendingBit(EXTI_Line2);

    }
}

void EXTI3_IRQHandler(void)
{
    if(SET == EXTI_GetITStatus(EXTI_Line3))
    {
        EXTI_ClearITPendingBit(EXTI_Line3);

    }
}

void EXTI4_IRQHandler(void)
{
    if(SET == EXTI_GetITStatus(EXTI_Line4))
    {
        EXTI_ClearITPendingBit(EXTI_Line4);

    }
}


void EXTI9_5_IRQHandler(void)
{
    if(SET == EXTI_GetITStatus(EXTI_Line5))
    {
        EXTI_ClearITPendingBit(EXTI_Line5);

    }
    if(SET == EXTI_GetITStatus(EXTI_Line6))
    {
        EXTI_ClearITPendingBit(EXTI_Line6);

    }
    if(SET == EXTI_GetITStatus(EXTI_Line7))
    {
        EXTI_ClearITPendingBit(EXTI_Line7);

    }
    if(SET == EXTI_GetITStatus(EXTI_Line8))
    {
        EXTI_ClearITPendingBit(EXTI_Line8);

    }
    if(SET == EXTI_GetITStatus(EXTI_Line9))
    {
        EXTI_ClearITPendingBit(EXTI_Line9);

    }

}

void EXTI15_10_IRQHandler(void)
{
    if(SET == EXTI_GetITStatus(EXTI_Line10))
    {
        EXTI_ClearITPendingBit(EXTI_Line10);

    }
    if(SET == EXTI_GetITStatus(EXTI_Line11))
    {
        EXTI_ClearITPendingBit(EXTI_Line11);

    }
    if(SET == EXTI_GetITStatus(EXTI_Line12))
    {
        EXTI_ClearITPendingBit(EXTI_Line12);

    }
    if(SET == EXTI_GetITStatus(EXTI_Line13))
    {
        EXTI_ClearITPendingBit(EXTI_Line13);

    }
    if(SET == EXTI_GetITStatus(EXTI_Line14))
    {
        // -----------------* DM1XA ���ź� Ԥ���жϴ����� *-----------------
        dm1xa_light_callback();
        // -----------------* DM1XA ���ź� Ԥ���жϴ����� *-----------------
        EXTI_ClearITPendingBit(EXTI_Line14);
    }
    if(SET == EXTI_GetITStatus(EXTI_Line15))
    {
        // -----------------* DM1XA ��/�����ź� Ԥ���жϴ����� *-----------------
        dm1xa_sound_callback();
        // -----------------* DM1XA ��/�����ź� Ԥ���жϴ����� *-----------------
        EXTI_ClearITPendingBit(EXTI_Line15);
    }
}



void TIM1_UP_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM1, TIM_IT_Update);

    }
}


void TIM2_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
    {
       TIM_ClearITPendingBit(TIM2, TIM_IT_Update );
    }
}

void TIM3_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
    {
        if(SWITCH_1==1&&SWITCH_2==0)
        {
            Menu_Select();
        }
        ADC_Get_Value();//��ֵ
        ADC_Get_Err();//�˲�����һ����Ⱥ�
        Gyroscope_GetData();
        Get_Gyroscope_Pitch(); //������ʵʱ�ж�
        if(Island_State!=0)//�������ͻ���
        {
            Get_Gyroscope_Angle();
             if(Island_State==9)//��9״̬�����������
             {
                 Clear_Gyroscope_Angle();
             }
        }
        else//�������������
        {
            Clear_Gyroscope_Angle();
        }
       TIM_ClearITPendingBit(TIM3, TIM_IT_Update );
    }
}

void TIM4_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
    {
       TIM_ClearITPendingBit(TIM4, TIM_IT_Update );
    }
}

void TIM5_IRQHandler(void)
{
    volatile static int count_50ms=0;
    volatile static int encoder_accu=0;
    if(TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET)
    {
        //�����ߴ�������
        if(Zebra_Stripes_Flag!=0)
        {
            if(Zebra_Stripes_Flag==1)
            {
                Stop_Flag=1;//����������ֹͣ���1
                Zebra_Stripes_Flag=2;
            }
        }

        //���紦����
        if(Stop_Flag!=0)
        {
            if(Stop_Flag==1)//ֹͣ����1��������ڰ����ڼ��ٵ�200��Ȼ�����2״̬���ٶ�����
            {
                count_50ms++;
                if(count_50ms>=10)//500ms��ʱ
                {
                    Stop_Flag=2;
                    count_50ms=0;
                }
            }
            else if(Stop_Flag==2)//ɲ��ʱ��ɲ��250ms��Ȼ�󿪻����㣬��Ϊ��������bug
            {
                count_50ms++;
                if(count_50ms>=5)
                {
                    Stop_Flag=3;
                    count_50ms=0;
                }
            }
        }


        if(Go==1)//1�Ƿ�����˫���복��ֹͣ������
        {
            count_50ms++;
            if(count_50ms>=20)//1s���Զ���2
            {
                Go=2;
                count_50ms=0;
                encoder_accu=0;
                gpio_set_level(CHARGE_PIN, 1);//����ʱ��Ϳ�ʼ���
            }
        }
        else if(Go==2)//������ǻ���
        {
            if(Go_Direction==0)//���ҳ���ѡ��
            {
                encoder_accu+=Speed_Right_Real;//2��ʼ��������
                if(encoder_accu<=-800)
                {
                    Go=3;
                    encoder_accu=0;
                }
            }
            else
            {
                encoder_accu+=Speed_Left_Real;//2��ʼ��������
                if(abs(encoder_accu)>=800)
                {
                    Go=3;
                    encoder_accu=0;
                }
            }
        }
        else if(Go==3)//ɲ��
        {
           count_50ms++;
            if(count_50ms>=10)//ɲ��
            {
                Go=4;
                encoder_accu=0;
            }
        }
        else if(Go==4)//΢΢����
        {
            count_50ms++;
            if(count_50ms>=20)
            {
                Go=5;
                encoder_accu=0;
            }
        }
//debugʹ��
//        ips200_show_int(50,10*16,encoder_accu,5);
//        ips200_show_int(50,12*16,Go,5);

        //�µ�����
        if(Ramp_Flag!=0)
        {
            if(Ramp_Flag==1)//�����ȸ�2
            {//2300��1m
                Ramp_Flag=2;
                count_50ms=0;
                encoder_accu=0;
            }
            else if(Ramp_Flag==2)//250ms�˲�
            {
                count_50ms++;
                if(count_50ms>=5)
                {
                    Ramp_Flag=3;
                    count_50ms=0;
                }
            }
            else if(Ramp_Flag==3)//3״̬��̫�ã�ǿ�Ƴ���
            {
                encoder_accu+=Speed_Right_Real;
                if(encoder_accu>=2500)
                {
                    Ramp_Flag=0;
                    encoder_accu=0;
                }
            }
            if(Ramp_Flag==4)//���º�1s��������
            {
                count_50ms++;
                if(count_50ms>=10)
                {
                    Ramp_Flag=0;
                    count_50ms=0;
                    encoder_accu=0;
                }
            }
//            ips200_show_int(50,10*16,Ramp_Flag,5);
//            ips200_show_int(50,11*16,encoder_accu,5);
        }


        //���·�ϴ�����
        if(Barricade_Flag!=0)
        {
            if(Barricade_Flag==1)
            {
                count_50ms=0;
                encoder_accu=0;//��ɾ���Ϊ��������׼��
                Barricade_Flag=2;
            }
            else if(Barricade_Flag==2)//2״̬���Ҵ�ǣ�ͬʱ��ʱ
            {
                count_50ms++;//����ʱ��
                if(count_50ms>=5)//��Ǽ�ʱ�ͻ��ֶ����ԣ����Լ�ѡ��
                {
                   count_50ms=0;
                   Barricade_Flag=3;
                }
            }
            else if(Barricade_Flag==3)
            {
                count_50ms++;
                //encoder_accu+=Speed_Right_Real;
                if(count_50ms>=10)
                {
                    count_50ms=0;
                    Barricade_Flag=4;
                }
            }
            else if(Barricade_Flag==4)//4����Ѱ������
            {
                encoder_accu+=Speed_Right_Real;
                if(encoder_accu>=1500)
                {
                    encoder_accu=0;
                    Barricade_Flag=0;
                }
            }
//       ips200_show_int(50,70+16*1,Barricade_Flag,5);
//       ips200_show_int(50,70+16*3,encoder_accu,5);
        }


        if(Island_State==9)
        {
            encoder_accu+=Speed_Right_Real;
            if(encoder_accu>=1000)//2300ԼΪ1�ף�����9״̬�����ڲ��жϻ���
            {
                encoder_accu=0;
                Island_State=0;
                Left_Island_Flag=0;
                Right_Island_Flag=0;
            }
        }

       TIM_ClearITPendingBit(TIM5, TIM_IT_Update );
    }
}

void TIM6_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET)
    {
        static int count_10ms=0;//������ȥ������ʹ���źſ�
        count_10ms++;
        if(count_10ms==2)
        {
            count_10ms=0;
            dm1xa_transmitter_ranging();
        }


        Speed_Left_Real=(-encoder_get_count(ENCODER_L_TIM));//������
        if(Speed_Left_Real<0)//���ֱ�������bug�������ڴ���bug����ʱ��������ת���Ǹ�ֵ������ֻ��ȡ��ֵ
        {
            Speed_Left_Real=-Speed_Left_Real;
        }
        Speed_Right_Real=(encoder_get_count(ENCODER_R_TIM));
        encoder_clear_count(ENCODER_R_TIM);
        encoder_clear_count(ENCODER_L_TIM);
        Velocity_Control(Speed_Left_Real,Speed_Right_Real);//�ջ�����
       TIM_ClearITPendingBit(TIM6, TIM_IT_Update );
    }
}

void TIM7_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM7, TIM_IT_Update) != RESET)
    {

        TIM_ClearITPendingBit(TIM7, TIM_IT_Update );
    }
}


void TIM8_UP_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM8, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM8, TIM_IT_Update);
    }
}


void TIM9_UP_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM9, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM9, TIM_IT_Update);
    }
}


void TIM10_UP_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM10, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM10, TIM_IT_Update);
    }
}



//.section    .text.vector_handler, "ax", @progbits

//   .weak   EXTI0_IRQHandler           /* EXTI Line 0 */
//   .weak   EXTI1_IRQHandler           /* EXTI Line 1 */
//   .weak   EXTI2_IRQHandler           /* EXTI Line 2 */
//   .weak   EXTI3_IRQHandler           /* EXTI Line 3 */
//   .weak   EXTI4_IRQHandler           /* EXTI Line 4 */
//   .weak   DMA1_Channel1_IRQHandler   /* DMA1 Channel 1 */
//   .weak   DMA1_Channel2_IRQHandler   /* DMA1 Channel 2 */
//   .weak   DMA1_Channel3_IRQHandler   /* DMA1 Channel 3 */
//   .weak   DMA1_Channel4_IRQHandler   /* DMA1 Channel 4 */
//   .weak   DMA1_Channel5_IRQHandler   /* DMA1 Channel 5 */
//   .weak   DMA1_Channel6_IRQHandler   /* DMA1 Channel 6 */
//   .weak   DMA1_Channel7_IRQHandler   /* DMA1 Channel 7 */
//   .weak   ADC1_2_IRQHandler          /* ADC1_2 */
//   .weak   USB_HP_CAN1_TX_IRQHandler  /* USB HP and CAN1 TX */
//   .weak   USB_LP_CAN1_RX0_IRQHandler /* USB LP and CAN1RX0 */
//   .weak   CAN1_RX1_IRQHandler        /* CAN1 RX1 */
//   .weak   CAN1_SCE_IRQHandler        /* CAN1 SCE */
//   .weak   EXTI9_5_IRQHandler         /* EXTI Line 9..5 */
//   .weak   TIM1_BRK_IRQHandler        /* TIM1 Break */
//   .weak   TIM1_UP_IRQHandler         /* TIM1 Update */
//   .weak   TIM1_TRG_COM_IRQHandler    /* TIM1 Trigger and Commutation */
//   .weak   TIM1_CC_IRQHandler         /* TIM1 Capture Compare */
//   .weak   TIM2_IRQHandler            /* TIM2 */
//   .weak   TIM3_IRQHandler            /* TIM3 */
//   .weak   TIM4_IRQHandler            /* TIM4 */
//   .weak   I2C1_EV_IRQHandler         /* I2C1 Event */
//   .weak   I2C1_ER_IRQHandler         /* I2C1 Error */
//   .weak   I2C2_EV_IRQHandler         /* I2C2 Event */
//   .weak   I2C2_ER_IRQHandler         /* I2C2 Error */
//   .weak   SPI1_IRQHandler            /* SPI1 */
//   .weak   SPI2_IRQHandler            /* SPI2 */
//   .weak   USART1_IRQHandler          /* USART1 */
//   .weak   USART2_IRQHandler          /* USART2 */
//   .weak   USART3_IRQHandler          /* USART3 */
//   .weak   EXTI15_10_IRQHandler       /* EXTI Line 15..10 */
//   .weak   RTCAlarm_IRQHandler        /* RTC Alarm through EXTI Line */
//   .weak   USBWakeUp_IRQHandler       /* USB Wakeup from suspend */
//   .weak   TIM8_BRK_IRQHandler        /* TIM8 Break */
//   .weak   TIM8_UP_IRQHandler         /* TIM8 Update */
//   .weak   TIM8_TRG_COM_IRQHandler    /* TIM8 Trigger and Commutation */
//   .weak   TIM8_CC_IRQHandler         /* TIM8 Capture Compare */
//   .weak   RNG_IRQHandler             /* RNG */
//   .weak   FSMC_IRQHandler            /* FSMC */
//   .weak   SDIO_IRQHandler            /* SDIO */
//   .weak   TIM5_IRQHandler            /* TIM5 */
//   .weak   SPI3_IRQHandler            /* SPI3 */
//   .weak   UART4_IRQHandler           /* UART4 */
//   .weak   UART5_IRQHandler           /* UART5 */
//   .weak   TIM6_IRQHandler            /* TIM6 */
//   .weak   TIM7_IRQHandler            /* TIM7 */
//   .weak   DMA2_Channel1_IRQHandler   /* DMA2 Channel 1 */
//   .weak   DMA2_Channel2_IRQHandler   /* DMA2 Channel 2 */
//   .weak   DMA2_Channel3_IRQHandler   /* DMA2 Channel 3 */
//   .weak   DMA2_Channel4_IRQHandler   /* DMA2 Channel 4 */
//   .weak   DMA2_Channel5_IRQHandler   /* DMA2 Channel 5 */
//   .weak   ETH_IRQHandler             /* ETH */
//   .weak   ETH_WKUP_IRQHandler        /* ETH WakeUp */
//   .weak   CAN2_TX_IRQHandler         /* CAN2 TX */
//   .weak   CAN2_RX0_IRQHandler        /* CAN2 RX0 */
//   .weak   CAN2_RX1_IRQHandler        /* CAN2 RX1 */
//   .weak   CAN2_SCE_IRQHandler        /* CAN2 SCE */
//   .weak   OTG_FS_IRQHandler          /* OTGFS */
//   .weak   USBHSWakeup_IRQHandler     /* USBHS Wakeup */
//   .weak   USBHS_IRQHandler           /* USBHS */
//   .weak   DVP_IRQHandler             /* DVP */
//   .weak   UART6_IRQHandler           /* UART6 */
//   .weak   UART7_IRQHandler           /* UART7 */
//   .weak   UART8_IRQHandler           /* UART8 */
//   .weak   TIM9_BRK_IRQHandler        /* TIM9 Break */
//   .weak   TIM9_UP_IRQHandler         /* TIM9 Update */
//   .weak   TIM9_TRG_COM_IRQHandler    /* TIM9 Trigger and Commutation */
//   .weak   TIM9_CC_IRQHandler         /* TIM9 Capture Compare */
//   .weak   TIM10_BRK_IRQHandler       /* TIM10 Break */
//   .weak   TIM10_UP_IRQHandler        /* TIM10 Update */
//   .weak   TIM10_TRG_COM_IRQHandler   /* TIM10 Trigger and Commutation */
//   .weak   TIM10_CC_IRQHandler        /* TIM10 Capture Compare */
//   .weak   DMA2_Channel6_IRQHandler   /* DMA2 Channel 6 */
//   .weak   DMA2_Channel7_IRQHandler   /* DMA2 Channel 7 */
//   .weak   DMA2_Channel8_IRQHandler   /* DMA2 Channel 8 */
//   .weak   DMA2_Channel9_IRQHandler   /* DMA2 Channel 9 */
//   .weak   DMA2_Channel10_IRQHandler  /* DMA2 Channel 10 */
//   .weak   DMA2_Channel11_IRQHandler  /* DMA2 Channel 11 */

/*******************************************************************************
* Function Name  : NMI_Handler
* Description    : This function handles NMI exception.
* Input          : None
* Return         : None
*******************************************************************************/
void NMI_Handler(void)
{
}

/*******************************************************************************
* Function Name  : HardFault_Handler
* Description    : This function handles Hard Fault exception.
* Input          : None
* Return         : None
*******************************************************************************/
void HardFault_Handler(void)
{
  while (1)
  {
  }
}


