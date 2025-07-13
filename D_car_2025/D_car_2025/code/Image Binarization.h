#ifndef _IMAGE_BINARIZATION_H
#define _IMAGE_BINARIZATION_H
#include "zf_common_headfile.h"
#include "zf_common_typedef.h"

// 图像尺寸定义（根据实际摄像头分辨率修改）
#define IMAGE_HEIGHT MT9V03X_H
#define IMAGE_WIDTH MT9V03X_W

extern uint8 binaryImage[IMAGE_HEIGHT][IMAGE_WIDTH];      // 最终输出结果的数组
extern uint8 boundary_image[IMAGE_HEIGHT][IMAGE_WIDTH];   // 边界图像
extern uint8 centerline_image[IMAGE_HEIGHT][IMAGE_WIDTH]; // 中心线图像
extern int center_points[IMAGE_HEIGHT];                   // 存储每行的中心点坐标

int otsu_get_threshold(uint8 *image, uint16 col, uint16 row);

void applyThreshold(uint8 input[][IMAGE_WIDTH], uint8 output[][IMAGE_WIDTH], int threshold);

void image_output();

#endif
