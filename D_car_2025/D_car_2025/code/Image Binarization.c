#include "Image Binarization.h"

#include "zf_device_mt9v03x.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>


extern uint8 mt9v03x_image[MT9V03X_H][MT9V03X_W];


// 图像尺寸定义（根据实际摄像头分辨率修改）
#define IMAGE_HEIGHT 120
#define IMAGE_WIDTH  188

// 大津法二值化函数
int otsuThreshold(unsigned char image[IMAGE_HEIGHT][IMAGE_WIDTH]) {
    int histogram[256] = {0};  // 灰度直方图
    int total_pixels = IMAGE_HEIGHT * IMAGE_WIDTH;
    double sum = 0.0;          // 总灰度值
    double sumB = 0.0;         // 背景类灰度总和
    int wB = 0;                // 背景类像素数
    int wF = 0;                // 前景类像素数
    double maxVariance = 0.0;   // 最大类间方差
    int threshold = 0;          // 最佳阈值
    
    // 步骤1: 计算灰度直方图
    for (int i = 0; i < IMAGE_HEIGHT; i++) {
        for (int j = 0; j < IMAGE_WIDTH; j++) {
            unsigned char pixel = image[i][j];
            histogram[pixel]++;
            sum += (double)pixel;
        }
    }
    
    // 步骤2: 遍历所有可能的阈值
    for (int t = 0; t < 256; t++) {
        wB += histogram[t];  // 背景类像素数增加
        if (wB == 0) continue;
        
        wF = total_pixels - wB;  // 前景类像素数
        if (wF == 0) break;
        
        sumB += (double)(t * histogram[t]);  // 背景类灰度总和
        
        double mB = sumB / wB;              // 背景类平均灰度
        double mF = (sum - sumB) / wF;      // 前景类平均灰度
        
        // 计算类间方差
        double variance = (double)wB * (double)wF * (mB - mF) * (mB - mF);
        
        // 更新最佳阈值
        if (variance > maxVariance) {
            maxVariance = variance;
            threshold = t;
        }
    }
    
    return threshold;
}

// 应用阈值进行二值化
void applyThreshold(unsigned char input[IMAGE_HEIGHT][IMAGE_WIDTH], 
                   unsigned char output[IMAGE_HEIGHT][IMAGE_WIDTH], 
                   int threshold) {
    for (int i = 0; i < IMAGE_HEIGHT; i++) {
        for (int j = 0; j < IMAGE_WIDTH; j++) {
            output[i][j] = (input[i][j] > threshold) ? 255 : 0;
        }
    }
}

// 简单的3x3均值滤波（可选）
void meanFilter(unsigned char input[IMAGE_HEIGHT][IMAGE_WIDTH], 
               unsigned char output[IMAGE_HEIGHT][IMAGE_WIDTH]) {
    for (int i = 1; i < IMAGE_HEIGHT - 1; i++) {
        for (int j = 1; j < IMAGE_WIDTH - 1; j++) {
            int sum = 0;
            // 3x3邻域求和
            for (int m = -1; m <= 1; m++) {
                for (int n = -1; n <= 1; n++) {
                    sum += input[i + m][j + n];
                }
            }
            output[i][j] = sum / 9;
        }
    }
}































