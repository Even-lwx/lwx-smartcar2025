#ifndef _IMAGE_H
#define _IMAGE_H

#include "zf_common_typedef.h"
#include "zf_common_headfile.h"
#define TURN_STANDARD_START turn_start // ǰհ
#define TURN_STANDARD_END turn_end	   // ǰհ

extern int turn_start;
extern int turn_end;
extern volatile int Cross_Flag ;// ʮ��
extern volatile int Island_State; // ����״̬��־
extern volatile int Ramp_Flag;    // �µ�



extern volatile int Left_Line[MT9V03X_H];  // ���������
extern volatile int Right_Line[MT9V03X_H]; // �ұ�������
extern const uint8 Road_Standard_Wide[MT9V03X_H];
extern volatile int Search_Stop_Line;
extern int Longest_White_Column_Left[2];  // �����,[0]������еĳ��ȣ�Ҳ����Search_Stop_Line������ֹ�У�[1���ǵ�ĳ��
extern int Longest_White_Column_Right[2]; // �����,[0]������еĳ��ȣ�Ҳ����Search_Stop_Line������ֹ�У�[1���ǵ�ĳ��
extern volatile int Zebra_Stripes_Flag;   // �����߱�־λ
void Longest_White_Column(void);
/*Ԫ��*/
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
