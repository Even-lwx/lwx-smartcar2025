#include "zf_common_headfile.h"
#include "Image Binarization.h"

extern uint8 image_copy[MT9V03X_H][MT9V03X_W];
uint8 binaryImage[IMAGE_HEIGHT][IMAGE_WIDTH];      // 最终输出结果的数组
uint8 boundary_image[IMAGE_HEIGHT][IMAGE_WIDTH];   // 边界图像
uint8 centerline_image[IMAGE_HEIGHT][IMAGE_WIDTH]; // 中心线图像
int center_points[IMAGE_HEIGHT];                   // 存储每行的中心点坐标

// 大津法（返回阈值）
//--------------------------------------------------------------
int otsuThreshold(uint8 image[][IMAGE_WIDTH])
{
    uint32 histogram[256] = {0}; // 灰度直方图
    uint32 total_pixels = (uint32_t)IMAGE_HEIGHT * IMAGE_WIDTH;
    uint64 sum = 0;           // 总灰度值
    uint64 sumB = 0;          // 背景类灰度总和
    uint32 wB = 0;            // 背景类像素数
    uint64 maxVariance = 0;   // 最大类间方差
    int threshold = 0;        // 最佳阈值
    int declineCount = 0;     // 方差下降计数器
    const int maxDecline = 2; // 最大允许连续下降次数

    // 计算灰度直方图和总灰度值
    for (int i = 0; i < IMAGE_HEIGHT; i++)
    {
        for (int j = 0; j < IMAGE_WIDTH; j++)
        {
            uint8 pixel = image[i][j];
            histogram[pixel]++;
            sum += pixel;
        }
    }

    // 遍历可能的阈值（利用单峰特性提前终止）
    for (int t = 0; t < 256; t += 2) // 间隔2个计算，提高运行速度
    {
        if (histogram[t] == 0)
            continue;

        wB += histogram[t];
        if (wB == 0)
            continue;

        uint32 wF = total_pixels - wB;
        if (wF == 0)
            break;

        sumB += (uint64)t * histogram[t];

        // 计算背景和前景的平均灰度
        uint64 mB = sumB / wB;
        uint64 mF = (sum - sumB) / wF;

        // 计算类间方差
        uint64 variance = (uint64)wB * wF * (mB - mF) * (mB - mF);

        // 检测方差下降趋势
        if (variance >= maxVariance)
        {
            maxVariance = variance;
            threshold = t;
            declineCount = 0; // 重置下降计数器
        }
        else
        {
            declineCount++;
            if (declineCount > maxDecline)
            {
                break;
            }
        }
    }

    return threshold;
}

//  应用阈值函数，生成二值化图像
//--------------------------------------------------------------
void applyThreshold(uint8 input[][IMAGE_WIDTH],
                    uint8 output[][IMAGE_WIDTH],
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






