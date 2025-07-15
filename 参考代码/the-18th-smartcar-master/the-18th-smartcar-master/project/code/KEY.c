#include "zf_common_headfile.h"
#include "KEY.h"

/*-------------------------------------------------------------------------------------------------------------------
  @brief     ������ʼ��
  @param     null
  @return    null
  Sample     Key_Init������
  @note      ע��Ӳ��������������
-------------------------------------------------------------------------------------------------------------------*/
void Key_Init(void)       //��ʼ��
{
    gpio_init(A2, GPI, 1, GPI_PULL_UP);//
    gpio_init(A8, GPI, 1, GPI_PULL_UP);
    gpio_init(D8, GPI, 1, GPI_PULL_UP);//
    gpio_init(B0, GPI, 1, GPI_PULL_UP);
    gpio_init(B12,GPI, 1, GPI_PULL_UP);//

    gpio_init(D1, GPI, 1, GPI_PULL_UP);//��������
    gpio_init(D3, GPI, 1, GPI_PULL_UP);
}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     ����ɨ��
  @param     mode ��0��1
  @return    ��������
  Sample     key=Key_Scan��0����
  @note      null
-------------------------------------------------------------------------------------------------------------------*/
uint8 Key_Scan(uint8 mode)//mode:0��֧��������1֧������
{
    static uint8 key_up=1;  //�����ɿ���־λ
    if(mode)
        key_up=1;       //֧������
    if(key_up&&(KEY1==0||KEY2==0||KEY3==0||KEY4==0||KEY5==0))
    {
        system_delay_ms(10);//��ʱ10MS�������˲�
        key_up=0;
        if(KEY1==0)
            return KEY1_PRES;
        else if(KEY2==0)
            return KEY2_PRES;
        else if(KEY3==0)
            return KEY3_PRES;
        else if(KEY4==0)
            return KEY4_PRES;
        else if(KEY5==0)
            return KEY5_PRES;
    }
    else if(KEY1==1&&KEY2==1&&KEY3==1&&KEY4==1&&KEY5==1)
        key_up=1;
    return 0;//�ް�������
}
