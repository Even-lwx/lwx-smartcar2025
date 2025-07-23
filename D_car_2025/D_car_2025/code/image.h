#ifndef _IMAGE_H
#define _IMAGE_H

#include "zf_common_typedef.h"
#include "zf_common_headfile.h"
#define TURN_STANDARD_START turn_start // 前瞻
#define TURN_STANDARD_END turn_end	   // 前瞻

extern int turn_start;
extern int turn_end;
extern volatile int Cross_Flag ;// 十字
extern volatile int Island_State; // 环岛状态标志
extern volatile int Ramp_Flag;    // 坡道



extern volatile int Left_Line[MT9V03X_H];  // 左边线数组
extern volatile int Right_Line[MT9V03X_H]; // 右边线数组
extern const uint8 Road_Standard_Wide[MT9V03X_H];
extern volatile int Search_Stop_Line;
extern int Longest_White_Column_Left[2];  // 最长白列,[0]是最长白列的长度，也就是Search_Stop_Line搜索截止行，[1】是第某列
extern int Longest_White_Column_Right[2]; // 最长白列,[0]是最长白列的长度，也就是Search_Stop_Line搜索截止行，[1】是第某列
extern volatile int Zebra_Stripes_Flag;   // 斑马线标志位
void Longest_White_Column(void);
/*元素*/
void Cross_Detect(void);
int Zebra_Detect(void);
void Ramp_Detect(void);




void Show_Boundry(void);
void Find_Up_Point(int start, int end);
void Find_Down_Point(int start, int end);
void Lengthen_Left_Boundry(int start, int end);
void Lengthen_Right_Boundry(int start, int end);
void Left_Add_Line(int x1, int y1, int x2, int y2);
void Right_Add_Line(int x1, int y1, int x2, int y2);
void Draw_Line(int startX, int startY, int endX, int endY);
float err_sum_average(uint8 start_point, uint8 end_point);
uint8 image_out_of_bounds(uint8 binaryImage[IMAGE_HEIGHT][IMAGE_WIDTH]);

#endif
