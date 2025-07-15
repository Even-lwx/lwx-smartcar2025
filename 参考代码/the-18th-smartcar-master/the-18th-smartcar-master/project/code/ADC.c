#include "zf_common_headfile.h"
#include "ADC.h"

volatile uint16 ADC_Raw_Value[ADC_NUM][ADC_FILTER_TIME];//adcԭʼ����,����һά��ָ��ĳ����У��ڶ�γ����ε�����
volatile uint16 ADC_Sum_Value[ADC_NUM];//5��������ͣ�ȥ�������Сֵ
volatile uint16 ADC_Max_Value[ADC_NUM];//adcÿ��ͨ�����ֵ�������˲�
volatile uint16 ADC_Min_Value[ADC_NUM];//adcÿ��ͨ����Сֵ�������˲�
volatile float ADC_Fil_Value[ADC_NUM]; //adc�˲����ֵ
volatile float ADC_Nor_Value[ADC_NUM];//adcÿ��ͨ����һ�����ֵ
volatile float ADC_Sum=0;
volatile float ADC_Dif=0;
volatile float ADC_Err=0;//����ʹ�õĵ�����

extern volatile uint8 Stop_Flag;
extern volatile uint8 Barricade_Flag;     //��ϱ�־λ���ں��ʱ������磬��ȥ���г��籣��
extern volatile uint8 Img_Disappear_Flag; //0ͼ����ʧ��־λ
extern volatile int Search_Stop_Line;     //������ֹ��,ֻ��¼���ȣ���Ҫ������Ҫ����Ұ�߶ȼ�ȥ��ֵ

/*-------------------------------------------------------------------------------------------------------------------
  @brief     ������ݻ�ȡ
  @param     null
  @return    null
  Sample     ADC_Get_Value������
  @note      ֱ�Ӷ���,���ڶ�ά�������һά�ǵ�ĳ����У��ڶ�γ��һ���Զ�5�����ݣ��������
-------------------------------------------------------------------------------------------------------------------*/
void  ADC_Get_Value(void)//��ֵ
{
    int i=0;
    for(i=0;i<ADC_FILTER_TIME;i++)
    {
        ADC_Raw_Value[0][i]=adc_convert(ADC1_IN12_C2);//����
        ADC_Raw_Value[1][i]=adc_convert(ADC1_IN15_C5);//��2
        ADC_Raw_Value[2][i]=adc_convert(ADC1_IN13_C3);
        ADC_Raw_Value[3][i]=adc_convert(ADC1_IN14_C4);//����
    }
}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     �˲�����һ����Ⱥͣ�����һ��ƫ��
  @param     null
  @return    ��ȡ�������
  Sample     ADC_Get_Err();
  @note      ֱ�ӵ��ñ�������ȥ��ȡADC_Err��Ϊ�������
                                            ������ͬʱ�ж��˳������
-------------------------------------------------------------------------------------------------------------------*/
void ADC_Get_Err(void)//�˲�����Ⱥ�
{
    static int count=0;
    int i=0,j=0;
    double sum=0;
    for(i=0;i<ADC_NUM;i++)//��������׼��
    {
        ADC_Max_Value[i]=ADC_Raw_Value[i][0];
        ADC_Min_Value[i]=ADC_Raw_Value[i][0];
        ADC_Sum_Value[i]=0;
    }
    for(i=0;i<ADC_NUM;i++)//����ѭ����ȡ��ÿ��adc�����ֵ����Сֵ
    {
        for(j=0;j<ADC_FILTER_TIME;j++)
        {
            ADC_Sum_Value[i]+=ADC_Raw_Value[i][j];
            if(ADC_Max_Value[i]<ADC_Raw_Value[i][j])
                ADC_Max_Value[i]=ADC_Raw_Value[i][j];
            if(ADC_Min_Value[i]>ADC_Raw_Value[i][j])
                ADC_Min_Value[i]=ADC_Raw_Value[i][j];
        }//Ȼ����ͣ�ȥ�������С��ȡƽ��
        ADC_Fil_Value[i]=((float)(ADC_Sum_Value[i]-ADC_Max_Value[i]-ADC_Min_Value[i])/((float)ADC_FILTER_TIME-2));
    }//��ʱ��ֵ�����˲����ֵ
    for(i=0;i<ADC_NUM;i++)//��һ������
    {
        ADC_Nor_Value[i]=(ADC_Fil_Value[i]/3500)*100;//��һ���󣬷Ŵ�100��,���ݴ���0~100
    }//�����3500��ʵ�⣬�ҵ�����Ӳ������12bit������adc������3500
    sum=ADC_Nor_Value[0]+ADC_Nor_Value[1]+ADC_Nor_Value[2]+ADC_Nor_Value[3];//�����sum�������籣��
    if(Stop_Flag==0)//�����������
    {
        if(Barricade_Flag==0)//�Ҳ����ں�Ϲ�����
        {
            if(sum<5&&Img_Disappear_Flag==1)
            {//��й�С��ͼ��ʧ��������·��
                count++;
                if(count>=10)//��10�λ��ᣬ���ۼ�10���ĸ��������֮��С��ĳһ��ֵ��ֱ��ͣ��
                {
                    count=0;
                    Stop_Flag=2;//�����־λ��2������ͣ�£���control�ļ����в���
                }
            }
            else if(Search_Stop_Line>=60&&sum<=5)//��������кܳ������ǵ��ֵ��С
            {//��֮�����籣�������ڵ��ֵ�ǳ�С��ͼ��Ҫô����Ҫô�����ף���ôֱ��ɲ������
                Stop_Flag=2;
            }
        }
    }
    //����adc��Ⱥͼ���
    ADC_Sum=ADC_Nor_Value[0]+ADC_Nor_Value[3];
    ADC_Dif=ADC_Nor_Value[0]-ADC_Nor_Value[3];
    if(ADC_Sum>10)
    {
        ADC_Err=(ADC_Dif/ADC_Sum)*100;//���ڵ�ſ��Ƶ�err��������������
    }
}
