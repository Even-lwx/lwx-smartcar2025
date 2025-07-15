#ifndef __ADC_H__
#define __ADC_H__

#include "zf_common_headfile.h"

#define  ADC_FILTER_TIME   5
#define  ADC_NUM           4

void  ADC_Get_Value(void);//读值
void  ADC_Get_Err(void);  //滤波，归一，差比和

#endif
