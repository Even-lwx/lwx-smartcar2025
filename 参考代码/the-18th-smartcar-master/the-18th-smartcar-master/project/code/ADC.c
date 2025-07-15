#include "zf_common_headfile.h"
#include "ADC.h"

volatile uint16 ADC_Raw_Value[ADC_NUM][ADC_FILTER_TIME];//adc原始数据,，第一维是指第某个电感，第二纬是五次的数据
volatile uint16 ADC_Sum_Value[ADC_NUM];//5个数据求和，去掉最大，最小值
volatile uint16 ADC_Max_Value[ADC_NUM];//adc每个通道最大值，用于滤波
volatile uint16 ADC_Min_Value[ADC_NUM];//adc每个通道最小值，用于滤波
volatile float ADC_Fil_Value[ADC_NUM]; //adc滤波后的值
volatile float ADC_Nor_Value[ADC_NUM];//adc每个通道归一化后的值
volatile float ADC_Sum=0;
volatile float ADC_Dif=0;
volatile float ADC_Err=0;//最终使用的电磁误差

extern volatile uint8 Stop_Flag;
extern volatile uint8 Barricade_Flag;     //横断标志位，在横断时，会出界，不去进行出界保护
extern volatile uint8 Img_Disappear_Flag; //0图像消失标志位
extern volatile int Search_Stop_Line;     //搜索截止行,只记录长度，想要坐标需要用视野高度减去该值

/*-------------------------------------------------------------------------------------------------------------------
  @brief     电感数据获取
  @param     null
  @return    null
  Sample     ADC_Get_Value（）；
  @note      直接读数,放在二维数组里，第一维是第某个电感，第二纬是一次性读5个数据，五次数据
-------------------------------------------------------------------------------------------------------------------*/
void  ADC_Get_Value(void)//读值
{
    int i=0;
    for(i=0;i<ADC_FILTER_TIME;i++)
    {
        ADC_Raw_Value[0][i]=adc_convert(ADC1_IN12_C2);//最左
        ADC_Raw_Value[1][i]=adc_convert(ADC1_IN15_C5);//左2
        ADC_Raw_Value[2][i]=adc_convert(ADC1_IN13_C3);
        ADC_Raw_Value[3][i]=adc_convert(ADC1_IN14_C4);//最右
    }
}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     滤波，归一，差比和，传出一个偏差
  @param     null
  @return    获取到的误差
  Sample     ADC_Get_Err();
  @note      直接调用本函数，去读取ADC_Err作为电磁误差即可
                                            在这里同时判断了出界情况
-------------------------------------------------------------------------------------------------------------------*/
void ADC_Get_Err(void)//滤波，差比和
{
    static int count=0;
    int i=0,j=0;
    double sum=0;
    for(i=0;i<ADC_NUM;i++)//数据清零准备
    {
        ADC_Max_Value[i]=ADC_Raw_Value[i][0];
        ADC_Min_Value[i]=ADC_Raw_Value[i][0];
        ADC_Sum_Value[i]=0;
    }
    for(i=0;i<ADC_NUM;i++)//两重循环，取出每组adc的最大值，最小值
    {
        for(j=0;j<ADC_FILTER_TIME;j++)
        {
            ADC_Sum_Value[i]+=ADC_Raw_Value[i][j];
            if(ADC_Max_Value[i]<ADC_Raw_Value[i][j])
                ADC_Max_Value[i]=ADC_Raw_Value[i][j];
            if(ADC_Min_Value[i]>ADC_Raw_Value[i][j])
                ADC_Min_Value[i]=ADC_Raw_Value[i][j];
        }//然后求和，去掉最大最小，取平均
        ADC_Fil_Value[i]=((float)(ADC_Sum_Value[i]-ADC_Max_Value[i]-ADC_Min_Value[i])/((float)ADC_FILTER_TIME-2));
    }//此时的值就是滤波后的值
    for(i=0;i<ADC_NUM;i++)//归一化处理
    {
        ADC_Nor_Value[i]=(ADC_Fil_Value[i]/3500)*100;//归一化后，放大100倍,数据处于0~100
    }//这里的3500是实测，我的这套硬件开的12bit，他的adc读满是3500
    sum=ADC_Nor_Value[0]+ADC_Nor_Value[1]+ADC_Nor_Value[2]+ADC_Nor_Value[3];//这里的sum用作出界保护
    if(Stop_Flag==0)//在正常情况下
    {
        if(Barricade_Flag==0)//且不是在横断过程中
        {
            if(sum<5&&Img_Disappear_Flag==1)
            {//电感过小，图像丢失，还不是路障
                count++;
                if(count>=10)//有10次机会，当累计10次四个电感数据之和小于某一阈值，直接停车
                {
                    count=0;
                    Stop_Flag=2;//这个标志位给2是立刻停下，在control文件中有阐述
                }
            }
            else if(Search_Stop_Line>=60&&sum<=5)//或者最长白列很长，但是电感值很小
            {//总之，出界保护建立在电感值非常小，图像要么丢，要么很离谱，那么直接刹车保护
                Stop_Flag=2;
            }
        }
    }
    //常规adc差比和计算
    ADC_Sum=ADC_Nor_Value[0]+ADC_Nor_Value[3];
    ADC_Dif=ADC_Nor_Value[0]-ADC_Nor_Value[3];
    if(ADC_Sum>10)
    {
        ADC_Err=(ADC_Dif/ADC_Sum)*100;//用于电磁控制的err从这里计算出来的
    }
}
