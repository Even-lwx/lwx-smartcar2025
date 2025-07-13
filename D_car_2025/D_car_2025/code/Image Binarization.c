#include "zf_common_headfile.h"
#include <math.h>
#include "Image Binarization.h"

uint8 binaryImage[IMAGE_HEIGHT][IMAGE_WIDTH];      // 最终输出结果的数组
uint8 boundary_image[IMAGE_HEIGHT][IMAGE_WIDTH];   // 边界图像
uint8 centerline_image[IMAGE_HEIGHT][IMAGE_WIDTH]; // 中心线图像
int center_points[IMAGE_HEIGHT];                   // 存储每行的中心点坐标

/**
 * @brief  otsu算法获取阈值
 * @param  *image 图像数组指针
 * @param  col 图像宽度
 * @param  row 图像高度
 * @retval 阈值
 */
int otsu_get_threshold(uint8 *image, uint16 col, uint16 row)
{
#define GrayScale 256
    uint16 width = col;
    uint16 height = row;
    int pixelCount[GrayScale];
    float pixelPro[GrayScale];
    int i, j;
    int pixelSum = width * height / 4;
    int threshold = 0;
    uint8 *data = image;
    for (i = 0; i < GrayScale; i++)
    {
        pixelCount[i] = 0;
        pixelPro[i] = 0;
    }
    uint32 gray_sum = 0;
    // 统计灰度级中每个像素在整幅图像中的个数
    for (i = 0; i < height; i += 2)
    {
        for (j = 0; j < width; j += 2)
        {
            pixelCount[(int)data[i * width + j]]++; // 将当前的点的像素值作为计数数组的下标
            gray_sum += (int)data[i * width + j];   // 灰度值总和
        }
    }
    // 计算每个像素值的点在整幅图像中的比例
    for (i = 0; i < GrayScale; i++)
    {
        pixelPro[i] = (float)pixelCount[i] / pixelSum;
    }
    // 遍历灰度级[0,255]
    float w0, w1, u0tmp, u1tmp, u0, u1, u, deltaTmp, deltaMax = 0;
    w0 = w1 = u0tmp = u1tmp = u0 = u1 = u = deltaTmp = 0;
    for (j = 0; j < GrayScale; j++)
    {
        w0 += pixelPro[j];        // 背景部分每个灰度值的像素点所占比例之和   即背景部分的比例
        u0tmp += j * pixelPro[j]; // 背景部分 每个灰度值的点的比例 *灰度值
        w1 = 1 - w0;
        u1tmp = gray_sum / pixelSum - u0tmp;
        u0 = u0tmp / w0;   // 背景平均灰度
        u1 = u1tmp / w1;   // 前景平均灰度
        u = u0tmp + u1tmp; // 全局平均灰度
        deltaTmp = w0 * pow((u0 - u), 2) + w1 * pow((u1 - u), 2);
        if (deltaTmp > deltaMax)
        {
            deltaMax = deltaTmp;
            threshold = j;
        }
        if (deltaTmp < deltaMax)
        {
            break;
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

// // 大津法（返回阈值）
// //--------------------------------------------------------------
// int otsuThreshold(uint8 image[][IMAGE_WIDTH])
// {
//     uint32 histogram[256] = {0}; // 灰度直方图
//     uint32 total_pixels = (uint32_t)IMAGE_HEIGHT * IMAGE_WIDTH;
//     uint64 sum = 0;           // 总灰度值
//     uint64 sumB = 0;          // 背景类灰度总和
//     uint32 wB = 0;            // 背景类像素数
//     uint64 maxVariance = 0;   // 最大类间方差
//     int threshold = 0;        // 最佳阈值
//     int declineCount = 0;     // 方差下降计数器
//     const int maxDecline = 2; // 最大允许连续下降次数

//     // 计算灰度直方图和总灰度值
//     for (int i = 0; i < IMAGE_HEIGHT; i++)
//     {
//         for (int j = 0; j < IMAGE_WIDTH; j++)
//         {
//             uint8 pixel = image[i][j];
//             histogram[pixel]++;
//             sum += pixel;
//         }
//     }

//     // 遍历可能的阈值（利用单峰特性提前终止）
//     for (int t = 0; t < 256; t += 2) // 间隔2个计算，提高运行速度
//     {
//         if (histogram[t] == 0)
//             continue;

//         wB += histogram[t];
//         if (wB == 0)
//             continue;

//         uint32 wF = total_pixels - wB;
//         if (wF == 0)
//             break;

//         sumB += (uint64)t * histogram[t];

//         // 计算背景和前景的平均灰度
//         uint64 mB = sumB / wB;
//         uint64 mF = (sum - sumB) / wF;

//         // 计算类间方差
//         uint64 variance = (uint64)wB * wF * (mB - mF) * (mB - mF);

//         // 检测方差下降趋势
//         if (variance >= maxVariance)
//         {
//             maxVariance = variance;
//             threshold = t;
//             declineCount = 0; // 重置下降计数器
//         }
//         else
//         {
//             declineCount++;
//             if (declineCount > maxDecline)
//             {
//                 break;
//             }
//         }
//     }

//     return threshold;
// }
