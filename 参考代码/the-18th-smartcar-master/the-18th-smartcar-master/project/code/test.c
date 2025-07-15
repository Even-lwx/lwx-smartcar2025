#include "zf_common_headfile.h"
#include "test.h"

/*-------------------------------------------------------------------------------------------------------------------
  @brief     舵机测试
  @param     null
  @return    null
  Sample     Steer_Test(void)
  @note      需要将工程清空，只运行这一段程序
             通过按键找出舵机左中右
-------------------------------------------------------------------------------------------------------------------*/
void  Steer_Test(void)
{//705  857
    static int steer=782;//50hz舵机中值基本上在700~800附近
    uint8 key=0;
    key=Key_Scan(0);
    if(KEY2_PRES==key)
    {
        steer++;
    }
    else if(KEY4_PRES==key)
    {
        steer--;
    }
    else if(KEY1_PRES==key)
    {
        steer+=10;
    }
    else if(KEY5_PRES==key)
    {
        steer-=10;
    }
    
    pwm_set_duty(STEER_PIN,steer);//舵机调节
    ips200_show_int(0,16,steer,3);
}
