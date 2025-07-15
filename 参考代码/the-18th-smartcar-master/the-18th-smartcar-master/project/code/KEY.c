#include "zf_common_headfile.h"
#include "KEY.h"

/*-------------------------------------------------------------------------------------------------------------------
  @brief     按键初始化
  @param     null
  @return    null
  Sample     Key_Init（）；
  @note      注意硬件上拉还是下拉
-------------------------------------------------------------------------------------------------------------------*/
void Key_Init(void)       //初始化
{
    gpio_init(A2, GPI, 1, GPI_PULL_UP);//
    gpio_init(A8, GPI, 1, GPI_PULL_UP);
    gpio_init(D8, GPI, 1, GPI_PULL_UP);//
    gpio_init(B0, GPI, 1, GPI_PULL_UP);
    gpio_init(B12,GPI, 1, GPI_PULL_UP);//

    gpio_init(D1, GPI, 1, GPI_PULL_UP);//两个拨码
    gpio_init(D3, GPI, 1, GPI_PULL_UP);
}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     按键扫描
  @param     mode ：0或1
  @return    按键号码
  Sample     key=Key_Scan（0）；
  @note      null
-------------------------------------------------------------------------------------------------------------------*/
uint8 Key_Scan(uint8 mode)//mode:0不支持连按，1支持连按
{
    static uint8 key_up=1;  //按键松开标志位
    if(mode)
        key_up=1;       //支持连按
    if(key_up&&(KEY1==0||KEY2==0||KEY3==0||KEY4==0||KEY5==0))
    {
        system_delay_ms(10);//延时10MS，消抖滤波
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
    return 0;//无按键按下
}
