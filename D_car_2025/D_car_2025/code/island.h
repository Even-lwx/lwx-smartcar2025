#ifndef _ISLAND_H
#define _ISLAND_H


#include "zf_common_headfile.h"

void Island_Detect(void);
void Image_Flag_Show(uint8 MT9V03XW,uint8(*InImg)[MT9V03XW],uint8 image_flag);
int Monotonicity_Change_Right(int start,int end);
int Monotonicity_Change_Left(int start,int end);
int Continuity_Change_Right(int start,int end);
int Continuity_Change_Left(int start,int end);
int Find_Left_Down_Point(int start,int end);
int Find_Left_Up_Point(int start,int end);
int Find_Right_Down_Point(int start,int end);
int Find_Right_Up_Point(int start,int end);
int Get_Road_Wide(int start_line,int end_line);
void K_Add_Boundry_Left(float k,int startX,int startY,int endY);
void K_Add_Boundry_Right(float k,int startX,int startY,int endY);
void K_Draw_Line(float k, int startX, int startY,int endY);
float Get_Right_K(int start_line,int end_line);
float Get_Left_K(int start_line,int end_line);


















#endif
