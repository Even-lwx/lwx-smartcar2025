#ifndef __KEY_H__
#define __KEY_H__

#include "zf_common_headfile.h"

#define  KEY1   gpio_get_level(A2)//上
#define  KEY2   gpio_get_level(A8)//左
#define  KEY3   gpio_get_level(D8)//中
#define  KEY4   gpio_get_level(B0)//右
#define  KEY5   gpio_get_level(B12)//下

#define SWITCH_1    gpio_get_level(D1)//正常是1，按下为0
#define SWITCH_2    gpio_get_level(D3)

#define KEY1_PRES  1//上
#define KEY2_PRES  2//左
#define KEY3_PRES  3//中
#define KEY4_PRES  4//右
#define KEY5_PRES  5//下

void  Key_Init(void);        //初始化
uint8 Key_Scan(uint8 mode);  //扫描按键mode:0不支持连按，1支持连按

#endif
