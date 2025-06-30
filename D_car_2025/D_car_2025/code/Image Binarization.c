#include "zf_common_headfile.h"
#include "Image Binarization.h"

// 使用摄像头实际尺寸定义
#define IMAGE_HEIGHT MT9V03X_H
#define IMAGE_WIDTH MT9V03X_W

extern uint8 image_copy[MT9V03X_H][MT9V03X_W];
unsigned char binaryImage[IMAGE_HEIGHT][IMAGE_WIDTH]; // 最终输出结果的数组

// 优化版大津法二值化函数（使用单峰特性提前终止）
int otsuThreshold(unsigned char image[IMAGE_HEIGHT][IMAGE_WIDTH])
{
    unsigned int histogram[256] = {0}; // 灰度直方图
    unsigned int total_pixels = (unsigned int)IMAGE_HEIGHT * IMAGE_WIDTH;
    unsigned long long sum = 0;         // 总灰度值
    unsigned long long sumB = 0;        // 背景类灰度总和
    unsigned int wB = 0;                // 背景类像素数
    unsigned long long maxVariance = 0; // 最大类间方差
    int threshold = 0;                  // 最佳阈值
    int declineCount = 0;               // 方差下降计数器
    const int maxDecline = 2;           // 最大允许连续下降次数

    // 步骤1: 计算灰度直方图和总灰度值
    for (int i = 0; i < IMAGE_HEIGHT; i++)
    {
        for (int j = 0; j < IMAGE_WIDTH; j++)
        {
            unsigned char pixel = image[i][j];
            histogram[pixel]++;
            sum += pixel;
        }
    }

    // 步骤2: 遍历可能的阈值（利用单峰特性提前终止）
    for (int t = 0; t < 256; t++)
    {
        if (histogram[t] == 0)
            continue;

        wB += histogram[t];
        if (wB == 0)
            continue;

        unsigned int wF = total_pixels - wB;
        if (wF == 0)
            break;

        sumB += (unsigned long long)t * histogram[t];

        // 计算背景和前景的平均灰度
        unsigned long long mB = sumB / wB;
        unsigned long long mF = (sum - sumB) / wF;

        // 计算类间方差
        unsigned long long variance = (unsigned long long)wB * wF * (mB - mF) * (mB - mF);

        // 检测方差下降趋势（利用单峰特性）
        if (variance >= maxVariance)
        {
            // 发现更大的方差，更新最大值
            maxVariance = variance;
            threshold = t;
            declineCount = 0; // 重置下降计数器
        }
        else
        {
            // 方差开始下降
            declineCount++;

            // 如果连续下降次数超过阈值，提前终止循环
            if (declineCount > maxDecline)
            {
                break;
            }
        }
    }

    return threshold;
}

// 应用阈值进行二值化
void applyThreshold(unsigned char input[IMAGE_HEIGHT][IMAGE_WIDTH],
                    unsigned char output[IMAGE_HEIGHT][IMAGE_WIDTH],
                    int threshold)
{
    for (int i = 0; i < IMAGE_HEIGHT; i++)
    {
        for (int j = 0; j < IMAGE_WIDTH; j++)
        {
            // 使用条件表达式优化二值化
            output[i][j] = (input[i][j] > threshold) ? 255 : 0;
        }
    }
}

void image_output()
{
    // 1. 计算大津法阈值（直接使用原始图像）
    int threshold = otsuThreshold(image_copy);

    // 2. 应用阈值进行二值化
    applyThreshold(image_copy, binaryImage, threshold);

    // 3. 现在binaryImage包含处理结果
    // 可以用于显示或其他处理
}