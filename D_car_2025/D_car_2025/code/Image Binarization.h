#ifndef _IMAGE_BINARIZATION_H
#define _IMAGE_BINARIZATION_H
#include "zf_common_headfile.h"
#include "zf_common_typedef.h"


// 图像尺寸定义（根据实际摄像头分辨率修改）
#define IMAGE_HEIGHT MT9V03X_H
#define IMAGE_WIDTH  MT9V03X_W

extern uint8 binaryImage[IMAGE_HEIGHT][IMAGE_WIDTH]; // 最终输出结果的数组
extern uint8 boundary_image[IMAGE_HEIGHT][IMAGE_WIDTH];   // 边界图像
extern uint8 centerline_image[IMAGE_HEIGHT][IMAGE_WIDTH]; // 中心线图像
extern int center_points[IMAGE_HEIGHT];                   // 存储每行的中心点坐标






#define Search_Image_W  MT9V03X_W     //搜线宽度
#define Search_Image_H  MT9V03X_H     //搜线高度
#define BLACKPOINT      90        //黑点值
#define WHITEMAXMUL     11        //白点最大值基于参考点的放大倍数
#define WHITEMINMUL     7         //白点最小值基于参考点的放大倍数

#define REFRENCEROW     5         //参考点统计行数
#define SEARCHRANGE     20        //搜线半径
#define STOPROW         3         //搜线停止行
#define CONTRASTOFFSET  3         //搜线对比偏移






extern uint8 Reference_Point;                //动态参考点
extern uint8 Reference_Col  ;                //动态参考列
extern uint8 White_Max_Point;                //动态白点最大值
extern uint8 White_Min_Point;                //动态白点最小值
extern uint8 Reference_Contrast_Ratio;       //参考对比度
extern uint8 Contrast_Ratio[Search_Image_H];       //各行动态对比度
extern uint8 Left_Edge_Line[Search_Image_H];              //左右边界
extern uint8 Right_Edge_Line[Search_Image_H];

extern uint8 Remote_Distance[Search_Image_W];      //白点远端距离


void Get_Reference_Point(void);
void Search_Reference_Col(void);
void Search_line(void);
















void image_output();











#endif
