#ifndef _EDGE_EXTRACTION_H
#define _EDGE_EXTRACTION_H

#include "zf_common_typedef.h"
#include "zf_common_headfile.h"

extern volatile int Left_Line[MT9V03X_H];                     // 左边线数组
extern volatile int Right_Line[MT9V03X_H];                    // 右边线数组

void Longest_White_Column(void);
void Cross_Detect(void);
void Show_Boundry(void);
void Find_Up_Point(int start, int end);
void Find_Down_Point(int start, int end);
void Lengthen_Left_Boundry(int start, int end);
void Lengthen_Right_Boundry(int start, int end);
void Left_Add_Line(int x1, int y1, int x2, int y2);
void Right_Add_Line(int x1, int y1, int x2, int y2);








#endif
