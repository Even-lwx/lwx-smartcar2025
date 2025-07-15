#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "zf_common_headfile.h"

#define IMG_BLACK     0X00      //0x00是黑
#define IMG_WHITE     0Xff      //0xff为白

int  my_adapt_threshold(uint8*image,uint16 col, uint16 row);
int  My_Adapt_Threshold(uint8*image,uint16 width, uint16 height);
void Longest_White_Column(void);
void Image_Binarization(int threshold);//图像二值化
void Ramp_Detect(void);
void Stop_Detect(void);
void Zebra_Detect(void);
void Cross_Detect(void);
void Straight_Detect(void);
void Barricade_Detect(void);
void Break_Road_Detect(void);
void Zebra_Stripes_Detect(void);
void Img_Disappear_Detect(void);
void Show_Boundry(void);
void Find_Up_Point(int start,int end);
void Find_Down_Point(int start,int end);
void Lengthen_Left_Boundry(int start,int end);
void Lengthen_Right_Boundry(int start,int end);
void Left_Add_Line(int x1,int y1,int x2,int y2);
void Right_Add_Line(int x1,int y1,int x2,int y2);
void Draw_Line(int startX, int startY, int endX, int endY);
void Flag_Show_101(uint8 MT9V03XW,uint8(*InImg)[MT9V03XW],uint8 image_flag);
void Flag_Show_102(uint8 MT9V03XW,uint8(*InImg)[MT9V03XW],uint8 image_flag);
void Flag_Show_201(uint8 MT9V03XW,uint8(*InImg)[MT9V03XW],uint8 image_flag);
void Flag_Show_202(uint8 MT9V03XW,uint8(*InImg)[MT9V03XW],uint8 image_flag);

#endif
