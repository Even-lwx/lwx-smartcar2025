// #include "zf_common_headfile.h"
// #include "Image Binarization.h"

//// 使用摄像头实际尺寸定义
// #define IMAGE_HEIGHT MT9V03X_H
// #define IMAGE_WIDTH MT9V03X_W

// extern uint8 image_copy[MT9V03X_H][MT9V03X_W];
// unsigned char binaryImage[IMAGE_HEIGHT][IMAGE_WIDTH]; // 最终输出结果的数组

//// 优化版大津法二值化函数（使用单峰特性提前终止）
// int otsuThreshold(unsigned char image[IMAGE_HEIGHT][IMAGE_WIDTH])
//{
//     unsigned int histogram[256] = {0}; // 灰度直方图
//     unsigned int total_pixels = (unsigned int)IMAGE_HEIGHT * IMAGE_WIDTH;
//     unsigned long long sum = 0;         // 总灰度值
//     unsigned long long sumB = 0;        // 背景类灰度总和
//     unsigned int wB = 0;                // 背景类像素数
//     unsigned long long maxVariance = 0; // 最大类间方差
//     int threshold = 0;                  // 最佳阈值
//     int declineCount = 0;               // 方差下降计数器
//     const int maxDecline = 2;           // 最大允许连续下降次数

//    // 步骤1: 计算灰度直方图和总灰度值
//    for (int i = 0; i < IMAGE_HEIGHT; i++)
//    {
//        for (int j = 0; j < IMAGE_WIDTH; j++)
//        {
//            unsigned char pixel = image[i][j];
//            histogram[pixel]++;
//            sum += pixel;
//        }
//    }

//    // 步骤2: 遍历可能的阈值（利用单峰特性提前终止）
//    for (int t = 0; t < 256; t++)
//    {
//        if (histogram[t] == 0)
//            continue;

//        wB += histogram[t];
//        if (wB == 0)
//            continue;

//        unsigned int wF = total_pixels - wB;
//        if (wF == 0)
//            break;

//        sumB += (unsigned long long)t * histogram[t];

//        // 计算背景和前景的平均灰度
//        unsigned long long mB = sumB / wB;
//        unsigned long long mF = (sum - sumB) / wF;

//        // 计算类间方差
//        unsigned long long variance = (unsigned long long)wB * wF * (mB - mF) * (mB - mF);

//        // 检测方差下降趋势（利用单峰特性）
//        if (variance >= maxVariance)
//        {
//            // 发现更大的方差，更新最大值
//            maxVariance = variance;
//            threshold = t;
//            declineCount = 0; // 重置下降计数器
//        }
//        else
//        {
//            // 方差开始下降
//            declineCount++;

//            // 如果连续下降次数超过阈值，提前终止循环
//            if (declineCount > maxDecline)
//            {
//                break;
//            }
//        }
//    }

//    return threshold;
//}

//// 应用阈值进行二值化
// void applyThreshold(unsigned char input[IMAGE_HEIGHT][IMAGE_WIDTH],
//                     unsigned char output[IMAGE_HEIGHT][IMAGE_WIDTH],
//                     int threshold)
//{
//     for (int i = 0; i < IMAGE_HEIGHT; i++)
//     {
//         for (int j = 0; j < IMAGE_WIDTH; j++)
//         {
//             // 使用条件表达式优化二值化
//             output[i][j] = (input[i][j] > threshold) ? 255 : 0;
//         }
//     }
// }

// void image_output()
//{
//     // 1. 计算大津法阈值（直接使用原始图像）
//     int threshold = otsuThreshold(image_copy);

//    // 2. 应用阈值进行二值化
//    applyThreshold(image_copy, binaryImage, threshold);

//    // 3. 现在binaryImage包含处理结果
//    // 可以用于显示或其他处理
//}

// #include "zf_common_headfile.h"
// #include "Image Binarization.h"

// 使用摄像头实际尺寸定义
// #define IMAGE_HEIGHT MT9V03X_H
// #define IMAGE_WIDTH MT9V03X_W

// extern uint8 image_copy[MT9V03X_H][MT9V03X_W];
// unsigned char binaryImage[IMAGE_HEIGHT][IMAGE_WIDTH]; // 最终输出结果的数组

// 1. 预处理函数 (大津法前)
//--------------------------------------------------------------
// 快速中值滤波 (3x3窗口)
// void medianFilter3x3(uint8 input[IMAGE_HEIGHT][IMAGE_WIDTH],
//                     uint8 output[IMAGE_HEIGHT][IMAGE_WIDTH])
//{
//    // 只处理内部像素，边缘保留原值
//    for (int i = 1; i < IMAGE_HEIGHT - 1; i++) {
//        for (int j = 1; j < IMAGE_WIDTH - 1; j++) {
//            // 收集3x3窗口的像素值
//            uint8 window[9];
//            int idx = 0;
//            for (int di = -1; di <= 1; di++) {
//                for (int dj = -1; dj <= 1; dj++) {
//                    window[idx++] = input[i + di][j + dj];
//                }
//            }
//
//            // 使用插入排序找中值 (比完全排序更快)
//            for (int k = 1; k < 9; k++) {
//                uint8 key = window[k];
//                int l = k - 1;
//                while (l >= 0 && window[l] > key) {
//                    window[l + 1] = window[l];
//                    l--;
//                }
//                window[l + 1] = key;
//            }
//
//            output[i][j] = window[4]; // 中值
//        }
//    }
//
//    // 复制边缘像素
//    for (int i = 0; i < IMAGE_HEIGHT; i++) {
//        output[i][0] = input[i][0];
//        output[i][IMAGE_WIDTH - 1] = input[i][IMAGE_WIDTH - 1];
//    }
//    for (int j = 0; j < IMAGE_WIDTH; j++) {
//        output[0][j] = input[0][j];
//        output[IMAGE_HEIGHT - 1][j] = input[IMAGE_HEIGHT - 1][j];
//    }
//}

// 对比度拉伸
// void contrastStretch(uint8 input[IMAGE_HEIGHT][IMAGE_WIDTH],
//                     uint8 output[IMAGE_HEIGHT][IMAGE_WIDTH])
//{
//    // 寻找最小和最大灰度值
//    uint8 min_val = 255;
//    uint8 max_val = 0;
//
//    for (int i = 0; i < IMAGE_HEIGHT; i++) {
//        for (int j = 0; j < IMAGE_WIDTH; j++) {
//            if (input[i][j] < min_val) min_val = input[i][j];
//            if (input[i][j] > max_val) max_val = input[i][j];
//        }
//    }
//
//    // 避免除零错误
//    if (max_val == min_val) max_val = min_val + 1;
//
//    // 应用线性拉伸
//    const uint8 range = max_val - min_val;
//    for (int i = 0; i < IMAGE_HEIGHT; i++) {
//        for (int j = 0; j < IMAGE_WIDTH; j++) {
//            // 使用整数运算优化
//            int32_t value = ((int32_t)(input[i][j] - min_val) * 255) / range;
//            output[i][j] = (value > 255) ? 255 : (value < 0) ? 0 : value;
//        }
//    }
//}

// 2. 大津法核心函数 (已优化)
//--------------------------------------------------------------
// int otsuThreshold(uint8 image[IMAGE_HEIGHT][IMAGE_WIDTH])
//{
//    uint32 histogram[256] = {0};       // 灰度直方图
//    uint32 total_pixels = (uint32_t)IMAGE_HEIGHT * IMAGE_WIDTH;
//    uint64 sum = 0;                    // 总灰度值
//    uint64 sumB = 0;                   // 背景类灰度总和
//    uint32 wB = 0;                     // 背景类像素数
//    uint64 maxVariance = 0;            // 最大类间方差
//    int threshold = 0;                 // 最佳阈值
//    int declineCount = 0;              // 方差下降计数器
//    const int maxDecline = 2;          // 最大允许连续下降次数

//    // 计算灰度直方图和总灰度值
//    for (int i = 0; i < IMAGE_HEIGHT; i++) {
//        for (int j = 0; j < IMAGE_WIDTH; j++) {
//            uint8 pixel = image[i][j];
//            histogram[pixel]++;
//            sum += pixel;
//        }
//    }

//    // 遍历可能的阈值（利用单峰特性提前终止）
//    for (int t = 0; t < 256; t++) {
//        if (histogram[t] == 0) continue;

//        wB += histogram[t];
//        if (wB == 0) continue;

//        uint32 wF = total_pixels - wB;
//        if (wF == 0) break;

//        sumB += (uint64)t * histogram[t];

//        // 计算背景和前景的平均灰度
//        uint64 mB = sumB / wB;
//        uint64 mF = (sum - sumB) / wF;

//        // 计算类间方差
//        uint64 variance = (uint64)wB * wF * (mB - mF) * (mB - mF);

//        // 检测方差下降趋势
//        if (variance >= maxVariance) {
//            maxVariance = variance;
//            threshold = t;
//            declineCount = 0; // 重置下降计数器
//        } else {
//            declineCount++;
//            if (declineCount > maxDecline) {
//                break;
//            }
//        }
//    }

//    return threshold;
//}

// 3. 后处理函数 (二值化后)
//--------------------------------------------------------------
// 快速腐蚀操作 (3x3结构元素)
// void erode3x3(uint8 input[IMAGE_HEIGHT][IMAGE_WIDTH],
//              uint8 output[IMAGE_HEIGHT][IMAGE_WIDTH])
//{
//    // 只处理内部像素
//    for (int i = 1; i < IMAGE_HEIGHT - 1; i++) {
//        for (int j = 1; j < IMAGE_WIDTH - 1; j++) {
//            // 如果中心像素是前景(255)，检查所有邻域像素
//            if (input[i][j] == 255) {
//                // 检查3x3邻域是否有背景(0)
//                uint8 isEroded = 0;
//                for (int di = -1; di <= 1 && !isEroded; di++) {
//                    for (int dj = -1; dj <= 1; dj++) {
//                        if (input[i + di][j + dj] == 0) {
//                            isEroded = 1;
//                            break;
//                        }
//                    }
//                }
//                output[i][j] = isEroded ? 0 : 255;
//            } else {
//                output[i][j] = 0; // 背景保持不变
//            }
//        }
//    }
//
//    // 处理边缘像素 (直接复制)
//    for (int i = 0; i < IMAGE_HEIGHT; i++) {
//        output[i][0] = input[i][0];
//        output[i][IMAGE_WIDTH - 1] = input[i][IMAGE_WIDTH - 1];
//    }
//    for (int j = 0; j < IMAGE_WIDTH; j++) {
//        output[0][j] = input[0][j];
//        output[IMAGE_HEIGHT - 1][j] = input[IMAGE_HEIGHT - 1][j];
//    }
//}

// 快速膨胀操作 (3x3结构元素)
// void dilate3x3(uint8 input[IMAGE_HEIGHT][IMAGE_WIDTH],
//               uint8 output[IMAGE_HEIGHT][IMAGE_WIDTH])
//{
//    // 只处理内部像素
//    for (int i = 1; i < IMAGE_HEIGHT - 1; i++) {
//        for (int j = 1; j < IMAGE_WIDTH - 1; j++) {
//            // 如果中心像素是背景(0)，检查所有邻域像素
//            if (input[i][j] == 0) {
//                // 检查3x3邻域是否有前景(255)
//                uint8 isDilated = 0;
//                for (int di = -1; di <= 1 && !isDilated; di++) {
//                    for (int dj = -1; dj <= 1; dj++) {
//                        if (input[i + di][j + dj] == 255) {
//                            isDilated = 1;
//                            break;
//                        }
//                    }
//                }
//                output[i][j] = isDilated ? 255 : 0;
//            } else {
//                output[i][j] = 255; // 前景保持不变
//            }
//        }
//    }
//
//    // 处理边缘像素 (直接复制)
//    for (int i = 0; i < IMAGE_HEIGHT; i++) {
//        output[i][0] = input[i][0];
//        output[i][IMAGE_WIDTH - 1] = input[i][IMAGE_WIDTH - 1];
//    }
//    for (int j = 0; j < IMAGE_WIDTH; j++) {
//        output[0][j] = input[0][j];
//        output[IMAGE_HEIGHT - 1][j] = input[IMAGE_HEIGHT - 1][j];
//    }
//}

// 开运算 (先腐蚀后膨胀)
// void morphologicalOpen(uint8 input[IMAGE_HEIGHT][IMAGE_WIDTH],
//                      uint8 output[IMAGE_HEIGHT][IMAGE_WIDTH])
//{
//    uint8 temp[IMAGE_HEIGHT][IMAGE_WIDTH];
//    erode3x3(input, temp);
//    dilate3x3(temp, output);
//}

// 闭运算 (先膨胀后腐蚀)
// void morphologicalClose(uint8 input[IMAGE_HEIGHT][IMAGE_WIDTH],
//                       uint8 output[IMAGE_HEIGHT][IMAGE_WIDTH])
//{
//    uint8 temp[IMAGE_HEIGHT][IMAGE_WIDTH];
//    dilate3x3(input, temp);
//    erode3x3(temp, output);
//}

// 4. 应用阈值函数
//--------------------------------------------------------------
// void applyThreshold(uint8 input[IMAGE_HEIGHT][IMAGE_WIDTH],
//                   uint8 output[IMAGE_HEIGHT][IMAGE_WIDTH],
//                   int threshold)
//{
//    for (int i = 0; i < IMAGE_HEIGHT; i++) {
//        for (int j = 0; j < IMAGE_WIDTH; j++) {
//            // 使用位操作优化二值化
//            output[i][j] = (input[i][j] > threshold) ? 255 : 0;
//        }
//    }
//}

// 5. 主处理流程
//--------------------------------------------------------------
// void image_output()
//{
//    // 内存优化：使用单一临时缓冲区
//    static uint8 tempBuffer[IMAGE_HEIGHT][IMAGE_WIDTH];
//
//    // 步骤1: 预处理 (中值滤波)
//    medianFilter3x3(image_copy, tempBuffer);
//
//    // 步骤2: 增强对比度 (可选，根据光照条件)
//    // 如果图像对比度低，启用对比度拉伸
//    #ifdef ENABLE_CONTRAST_STRETCH
//    contrastStretch(tempBuffer, tempBuffer);
//    #endif
//
//    // 步骤3: 计算大津法阈值
//    int threshold = otsuThreshold(tempBuffer);
//
//    // 步骤4: 应用阈值二值化
//    applyThreshold(tempBuffer, binaryImage, threshold);
//
//    // 步骤5: 后处理降噪
//    // 开运算去除小噪声点
//    morphologicalOpen(binaryImage, tempBuffer);
//
//    // 闭运算填充小孔洞
//    morphologicalClose(tempBuffer, binaryImage);
//
//    // 现在binaryImage包含最终处理结果
//    // 可用于显示或后续处理
//}

#include "zf_common_headfile.h"
#include "Image Binarization.h"

// 使用摄像头实际尺寸定义
#define IMAGE_HEIGHT MT9V03X_H
#define IMAGE_WIDTH MT9V03X_W

extern uint8 image_copy[MT9V03X_H][MT9V03X_W];
uint8 binaryImage[IMAGE_HEIGHT][IMAGE_WIDTH]; // 最终输出结果的数组
uint8 boundary_image[IMAGE_HEIGHT][IMAGE_WIDTH];   // 边界图像
uint8 centerline_image[IMAGE_HEIGHT][IMAGE_WIDTH]; // 中心线图像
int center_points[IMAGE_HEIGHT];                   // 存储每行的中心点坐标



// 大津法核心函数 (已优化)
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
    for (int t = 0; t < 256; t+=2)//间隔2个计算，提高运行速度
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






//  应用阈值函数
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


// 八邻域边界检测与中心线计算函数
void find_boundaries_and_centerline() {
    // 初始化图像
    for (int y = 0; y < IMAGE_HEIGHT; y++) {
        for (int x = 0; x < IMAGE_WIDTH; x++) {
            boundary_image[y][x] = 0;
            centerline_image[y][x] = 0;
        }
        center_points[y] = -1; // 初始化为无效值
    }

    // 定义八邻域相对坐标
    const int dx[8] = {-1, -1, 0, 1, 1, 1, 0, -1};
    const int dy[8] = {0, -1, -1, -1, 0, 1, 1, 1};

    // 只处理图像中间区域，避免边界检查
    for (int y = 1; y < IMAGE_HEIGHT - 1; y++) {
        int left_boundary = -1;
        int right_boundary = -1;

        // 寻找左边界
        for (int x = 1; x < IMAGE_WIDTH - 1; x++) {
            if (binaryImage[y][x] == 255) {
                // 检查八邻域中是否有背景点
                for (int i = 0; i < 8; i++) {
                    int nx = x + dx[i];
                    int ny = y + dy[i];
                    if (binaryImage[ny][nx] == 0) {
                        left_boundary = x;
                        boundary_image[y][x] = 255; // 标记为边界
                        break;
                    }
                }
                if (left_boundary != -1) break;
            }
        }

        // 寻找右边界
        for (int x = IMAGE_WIDTH - 2; x > 0; x--) {
            if (binaryImage[y][x] == 255) {
                // 检查八邻域中是否有背景点
                for (int i = 0; i < 8; i++) {
                    int nx = x + dx[i];
                    int ny = y + dy[i];
                    if (binaryImage[ny][nx] == 0) {
                        right_boundary = x;
                        boundary_image[y][x] = 255; // 标记为边界
                        break;
                    }
                }
                if (right_boundary != -1) break;
            }
        }

        // 计算中心线
        if (left_boundary != -1 && right_boundary != -1) {
            int center_x = (left_boundary + right_boundary) / 2;
            center_points[y] = center_x; // 存储中心点
            
            // 在中心线图像上标记中心点（±2像素范围内）
            for (int offset = -2; offset <= 2; offset++) {
                int cx = center_x + offset;
                if (cx >= 0 && cx < IMAGE_WIDTH) {
                    centerline_image[y][cx] = 128; // 灰色标记
                }
            }
        }
    }
}


//  主处理流程
//--------------------------------------------------------------
void image_output()
{

    // 步骤3: 计算大津法阈值
    int threshold = otsuThreshold(image_copy);

    // 步骤4: 应用阈值二值化
    applyThreshold(image_copy, binaryImage, threshold);
  find_boundaries_and_centerline();
}
