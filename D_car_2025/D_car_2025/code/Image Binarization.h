#ifndef _IMAGE_BINARIZATION_H
#define _IMAGE_BINARIZATION_H

#include "zf_common_typedef.h"

// ͼ��ߴ綨�壨����ʵ������ͷ�ֱ����޸ģ�
#define IMAGE_HEIGHT MT9V03X_H
#define IMAGE_WIDTH  MT9V03X_W



extern uint8 binaryImage[IMAGE_HEIGHT][IMAGE_WIDTH]; // ����������������
extern uint8 boundary_image[IMAGE_HEIGHT][IMAGE_WIDTH];   // �߽�ͼ��
extern uint8 centerline_image[IMAGE_HEIGHT][IMAGE_WIDTH]; // ������ͼ��
extern int center_points[IMAGE_HEIGHT];                   // �洢ÿ�е����ĵ�����

void image_output();











#endif
