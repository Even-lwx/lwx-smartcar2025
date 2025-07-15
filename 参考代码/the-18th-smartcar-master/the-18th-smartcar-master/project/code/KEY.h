#ifndef __KEY_H__
#define __KEY_H__

#include "zf_common_headfile.h"

#define  KEY1   gpio_get_level(A2)//��
#define  KEY2   gpio_get_level(A8)//��
#define  KEY3   gpio_get_level(D8)//��
#define  KEY4   gpio_get_level(B0)//��
#define  KEY5   gpio_get_level(B12)//��

#define SWITCH_1    gpio_get_level(D1)//������1������Ϊ0
#define SWITCH_2    gpio_get_level(D3)

#define KEY1_PRES  1//��
#define KEY2_PRES  2//��
#define KEY3_PRES  3//��
#define KEY4_PRES  4//��
#define KEY5_PRES  5//��

void  Key_Init(void);        //��ʼ��
uint8 Key_Scan(uint8 mode);  //ɨ�谴��mode:0��֧��������1֧������

#endif
