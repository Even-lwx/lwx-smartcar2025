#ifndef _IMAGE_BINARIZATION_H
#define _IMAGE_BINARIZATION_H



// 图像尺寸定义（根据实际摄像头分辨率修改）
#define IMAGE_HEIGHT MT9V03X_H
#define IMAGE_WIDTH  MT9V03X_W



extern unsigned char binaryImage[IMAGE_HEIGHT][IMAGE_WIDTH];//这是最终输出结果的数组
void image_output();











#endif
