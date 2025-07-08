#ifndef _IMAGE_BINARIZATION_H
#define _IMAGE_BINARIZATION_H
#include "zf_common_headfile.h"
#include "zf_common_typedef.h"


// ͼ��ߴ綨�壨����ʵ������ͷ�ֱ����޸ģ�
#define IMAGE_HEIGHT MT9V03X_H
#define IMAGE_WIDTH  MT9V03X_W

extern uint8 binaryImage[IMAGE_HEIGHT][IMAGE_WIDTH]; // ����������������
extern uint8 boundary_image[IMAGE_HEIGHT][IMAGE_WIDTH];   // �߽�ͼ��
extern uint8 centerline_image[IMAGE_HEIGHT][IMAGE_WIDTH]; // ������ͼ��
extern int center_points[IMAGE_HEIGHT];                   // �洢ÿ�е����ĵ�����






#define Search_Image_W  MT9V03X_W     //���߿��
#define Search_Image_H  MT9V03X_H     //���߸߶�
#define BLACKPOINT      90        //�ڵ�ֵ
#define WHITEMAXMUL     11        //�׵����ֵ���ڲο���ķŴ���
#define WHITEMINMUL     7         //�׵���Сֵ���ڲο���ķŴ���

#define REFRENCEROW     5         //�ο���ͳ������
#define SEARCHRANGE     20        //���߰뾶
#define STOPROW         3         //����ֹͣ��
#define CONTRASTOFFSET  3         //���߶Ա�ƫ��






extern uint8 Reference_Point;                //��̬�ο���
extern uint8 Reference_Col  ;                //��̬�ο���
extern uint8 White_Max_Point;                //��̬�׵����ֵ
extern uint8 White_Min_Point;                //��̬�׵���Сֵ
extern uint8 Reference_Contrast_Ratio;       //�ο��Աȶ�
extern uint8 Contrast_Ratio[Search_Image_H];       //���ж�̬�Աȶ�
extern uint8 Left_Edge_Line[Search_Image_H];              //���ұ߽�
extern uint8 Right_Edge_Line[Search_Image_H];

extern uint8 Remote_Distance[Search_Image_W];      //�׵�Զ�˾���


void Get_Reference_Point(void);
void Search_Reference_Col(void);
void Search_line(void);
















void image_output();











#endif
