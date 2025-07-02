#ifndef _IMAGE_BINARIZATION_H
#define _IMAGE_BINARIZATION_H

#include "zf_common_typedef.h"

// 图像尺寸定义（根据实际摄像头分辨率修改）
#define IMAGE_HEIGHT MT9V03X_H
#define IMAGE_WIDTH  MT9V03X_W



extern uint8 binaryImage[IMAGE_HEIGHT][IMAGE_WIDTH]; // 最终输出结果的数组
void image_output();











#endif
