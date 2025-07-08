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

// 八邻域边界检测与中心线计算函数
void find_boundaries_and_centerline()
{
    // 初始化图像
    for (int y = 0; y < IMAGE_HEIGHT; y++)
    {
        for (int x = 0; x < IMAGE_WIDTH; x++)
        {
            boundary_image[y][x] = 0;
            centerline_image[y][x] = 0;
        }
        center_points[y] = -1; // 初始化为无效值
    }

    // 定义八邻域相对坐标
    const int dx[8] = {-1, -1, 0, 1, 1, 1, 0, -1};
    const int dy[8] = {0, -1, -1, -1, 0, 1, 1, 1};

    // 只处理图像中间区域，避免边界检查
    for (int y = 1; y < IMAGE_HEIGHT - 1; y++)
    {
        int left_boundary = -1;
        int right_boundary = -1;

        // 寻找左边界
        for (int x = 1; x < IMAGE_WIDTH - 1; x++)
        {
            if (binaryImage[y][x] == 255)
            {
                // 检查八邻域中是否有背景点
                for (int i = 0; i < 8; i++)
                {
                    int nx = x + dx[i];
                    int ny = y + dy[i];
                    if (binaryImage[ny][nx] == 0)
                    {
                        left_boundary = x;
                        boundary_image[y][x] = 255; // 标记为边界
                        break;
                    }
                }
                if (left_boundary != -1)
                    break;
            }
        }

        // 寻找右边界
        for (int x = IMAGE_WIDTH - 2; x > 0; x--)
        {
            if (binaryImage[y][x] == 255)
            {
                // 检查八邻域中是否有背景点
                for (int i = 0; i < 8; i++)
                {
                    int nx = x + dx[i];
                    int ny = y + dy[i];
                    if (binaryImage[ny][nx] == 0)
                    {
                        right_boundary = x;
                        boundary_image[y][x] = 255; // 标记为边界
                        break;
                    }
                }
                if (right_boundary != -1)
                    break;
            }
        }

        // 计算中心线
        if (left_boundary != -1 && right_boundary != -1)
        {
            int center_x = (left_boundary + right_boundary) / 2;
            center_points[y] = center_x; // 存储中心点

            // 在中心线图像上标记中心点（±2像素范围内）
            for (int offset = -2; offset <= 2; offset++)
            {
                int cx = center_x + offset;
                if (cx >= 0 && cx < IMAGE_WIDTH)
                {
                    centerline_image[y][cx] = 128; // 灰色标记
                }
            }
        }
    }
}



/*=====================================================================================*/


uint8 Reference_Point = 0;           // 动态参考点
uint8 Reference_Col = 0;             // 动态参考列
uint8 White_Max_Point = 0;           // 动态白点最大值
uint8 White_Min_Point = 0;           // 动态白点最小值
uint8 Reference_Contrast_Ratio = 20; // 参考对比度

uint8 Remote_Distance[Search_Image_W] = {0}; // 白点远端距离
uint8 Left_Edge_Line[Search_Image_H] = {0};  // 左边界
uint8 Right_Edge_Line[Search_Image_H] = {0}; // 右边界

/*******************************************************************************
 * 函 数 名 * : LimitDouMid
 * 函数功能 * :  将value限幅到limit1与limit2之间
 *******************************************************************************/
int32 LimitDouMid(int32 value, int32 limit1, int32 limit2)
{
    int32 buf = 0;

    if (limit1 > limit2) // 要求limit1 < limit2
    {
        buf = limit1;
        limit1 = limit2;
        limit2 = buf;
    }

    if (value < limit1)
        return limit1;
    else if (value > limit2)
        return limit2;
    else
        return value;
}

/*******************************************************************************
 * 函 数 名 * : Find_extreme_value
 * 函数功能 * :  返回一维数组最大值或最小值
 *  *va     :数组指针
 *  num0    :起始点
 *  num1    :结束点
 *  model   : 1 返回最大值    0  返回最小值
 *******************************************************************************/
int Find_extreme_value(uint8 *va, uint8 num0, uint8 num1, uint8 model)
{
    uint8 i = 0, temp = 0, temp1 = 0, temp2 = 0, value = 0;
    if (num0 > num1)
    {
        temp1 = num0 - num1;
        temp2 = num1;
        va += num0;
        value = *va;
        if (model)
        {
            for (i = 0; i <= temp1; i++)
            {
                temp = *(va - i);
                if (temp > value)
                {
                    temp2 = num0 - i;
                    value = temp;
                }
            }
        }
        else
        {
            for (i = 0; i <= temp1; i++)
            {
                temp = *(va - i);
                if (temp < value)
                {
                    temp2 = num0 - i;
                    value = temp;
                }
            }
        }
    }
    else
    {
        temp1 = num1 - num0;
        temp2 = num0;
        va += num0;
        value = *va;
        if (model)
        {
            for (i = 0; i <= temp1; i++)
            {
                temp = *(va + i);
                if (temp > value)
                {
                    temp2 = i + num0;
                    value = temp;
                }
            }
        }
        else
        {
            for (i = 0; i <= temp1; i++)
            {
                temp = *(va + i);
                if (temp < value)
                {
                    temp2 = i + num0;
                    value = temp;
                }
            }
        }
    }
    return temp2;
}

// 获取图像参考点，白点最大值，白点最小值
void Get_Reference_Point(void)
{
    uint8 *p = mt9v03x_image[Search_Image_H - REFRENCEROW]; // 图像数组指针指向待统计的起始点
    uint16 temp = 0;                                        // 保存统计点总数量
    uint32 temp1 = 0;                                       // 保存所有统计点加起来的和
    temp = REFRENCEROW * Search_Image_W;                    // 计算待统计点总数量
    for (int i = 0; i < temp; i++)
        temp1 += *(p + i);                   // 统计点求和
    Reference_Point = (uint8)(temp1 / temp); // 计算点平均值，作为本幅图像的参考点

    // 根据参考点计算白点最大值、最小值（限幅在黑点阈值和255之间）
    White_Max_Point = (uint8)LimitDouMid((int32)Reference_Point * WHITEMAXMUL / 10, BLACKPOINT, 255);
    White_Min_Point = (uint8)LimitDouMid((int32)Reference_Point * WHITEMINMUL / 10, BLACKPOINT, 255);
}

// 搜索每列白点距离及图像参考列，便于搜索边线
void Search_Reference_Col(void)
{
    int col, row;
    uint8 temp1 = 0, temp2 = 0;
    int temp3 = 0;
    uint8 *p = mt9v03x_image[0]; // 图像数组指针
    for (col = 0; col < Search_Image_W; col++)
    {
        for (row = Search_Image_H - 1; row > STOPROW; row--)
        {

            temp1 = *(p + row * Search_Image_W + col);             // 获取当前点灰度值
            temp2 = *(p + (row - STOPROW) * Search_Image_W + col); // 获取对比点灰度值

            if (temp2 > White_Max_Point)
                continue; // 判断对比点是否为白点 若为白点则直接跳过

            if (temp1 < White_Min_Point)
            {
                Remote_Distance[col] = row - 1;
                break;
            } // 判断当前点是否为黑点 若为黑点则不进行对比直接赋值

            temp3 = (temp1 - temp2) * 200 / (temp1 + temp2); // 计算对比度
            // 判断对比度是否大于阈值 如果大于阈值则认为搜索到边界 或者已经搜索到该行最边界，则直接赋值边界
            if (temp3 > Reference_Contrast_Ratio || row == STOPROW + 1)
            {
                Remote_Distance[col] = row - 1;
                break;
            }
        }
    }
    Reference_Col = (uint8)Find_extreme_value(Remote_Distance, 10, Search_Image_W - 10, 0); // 求出最远的白点列作为参考列
    Reference_Col = (uint8)LimitDouMid(Reference_Col, 1, Search_Image_W - 2);               // 参考列限幅 防止数据溢出
}

// 搜索赛道边界
void Search_line(void)
{
    uint8 *p = mt9v03x_image[0];                    // 图像数组指针
    uint8 row_max = Search_Image_H - STOPROW;       // 行最大值
    uint8 row_min = STOPROW;                        // 行最小值
    uint8 col_max = Search_Image_W - 3;             // 列最大值
    uint8 col_min = 3;                              // 列最小值
    int16 leftstartcol = Reference_Col;             // 搜线左起始列
    int16 rightstartcol = Reference_Col;            // 搜线右起始列
    int16 leftendcol = 0;                           // 搜线左终止列
    int16 rightendcol = Search_Image_W;             // 搜线右终止列
    uint8 temp1 = 0, temp2 = 0;                     // 临时变量  用于存储图像数据
    int temp3 = 0;                                  // 临时变量  用于存储对比度
    int leftstop = 0, rightstop = 0, stoppoint = 0; // 搜线自锁变量

    int col, row;

    for (row = row_max; row > row_min; row--)
    {
        Left_Edge_Line[row] = col_min - CONTRASTOFFSET;
        Right_Edge_Line[row] = col_max + CONTRASTOFFSET;
    }

    for (row = row_max; row > row_min; row--)
    {
        p = &mt9v03x_image[row][0]; // 获取本行起点位置指针
			
			
        if (!leftstop)
        {
            for (col = leftstartcol; col >= leftendcol; col--)
            {
                temp1 = *(p + col);                  // 获取当前点灰度值
                temp2 = *(p + col - CONTRASTOFFSET); // 获取对比点灰度值
                // 判断参考列是否为黑点 若为黑点则放弃搜线
                if (temp1 < White_Min_Point && col == leftstartcol && leftstartcol == Reference_Col)
                {
                    leftstop = 1; // 线搜索自锁 不在进行左边线搜索
                    for (stoppoint = row; stoppoint >= 0; stoppoint--)
                        Left_Edge_Line[stoppoint] = col_min; // 清除剩余的无效左边线
                    break;
                }
                // 判断行搜索起始点是否为黑点 若为黑点则重新确立起始范围
                if ((temp1 < White_Min_Point && col == leftstartcol) || (leftstartcol != Reference_Col && col == col_min + 1))
                {
                    if (leftstartcol < Reference_Col)
                    {
                        col = Reference_Col;
                        leftstartcol = Reference_Col;
                    }
                }
                // 判断行搜索终止点是否为白点 若为白点则重新确立结束范围
                if (temp1 > White_Min_Point && col == leftendcol)
                {
                    if (leftendcol > col_min)
                    {
                        leftendcol = col_min;
                    }
                }
                // 判断当前点是否为黑点 若为黑点则不进行对比直接赋值
                if (temp1 < White_Min_Point && leftstartcol == Reference_Col)
                {
                    Left_Edge_Line[row] = (uint8)col;
                    break;
                }
                // 判断对比点是否为白点 若为白点则直接跳过
                if (temp2 > White_Max_Point)
                    continue;
                // 计算对比度
                temp3 = (temp1 - temp2) * 200 / (temp1 + temp2);
                // 判断对比度是否大于阈值 如果大于阈值则认为是行边界  或者已经搜索到该行最边界，则直接赋值行边界
                if (temp3 > Reference_Contrast_Ratio || col == col_min + 1)
                {
                    Left_Edge_Line[row] = col - CONTRASTOFFSET + 1; // 保存当前行左边界
                    // 刷新搜线范围
                    // leftstartcol = (uint8)LimitDouMid(col + SEARCHRANGE, col, Reference_Col);
                    leftendcol = (uint8)LimitDouMid(col - SEARCHRANGE, col_min, col);
                    break;
                }
            }
        }
        if (!rightstop)
        {
            for (col = rightstartcol; col <= rightendcol; col++)
            {
                temp1 = *(p + col);                  // 获取当前点灰度值
                temp2 = *(p + col + CONTRASTOFFSET); // 获取对比点灰度值
                // 判断参考列是否为黑点 若为黑点则放弃搜线
                if (temp1 < White_Min_Point && col == rightstartcol && rightstartcol == Reference_Col)
                {
                    rightstop = 1; // 线搜索自锁 不再进行右边线搜索
                    for (stoppoint = row; stoppoint >= 0; stoppoint--)
                        Right_Edge_Line[stoppoint] = col_max; // 清除剩余的无效右边线
                    break;
                }
                // 判断行搜索起始点是否为黑点 若为黑点则重新确立搜线范围
                if ((temp1 < White_Min_Point && col == rightstartcol) || (rightstartcol != Reference_Col && col == col_max - 1))
                {
                    if (rightstartcol < Reference_Col)
                    {
                        col = Reference_Col;
                        rightstartcol = Reference_Col;
                    }
                }
								
                // 判断行搜索终止点是否为白点 若为白点则重新确立结束范围
                if (temp1 < White_Max_Point && col == rightendcol)
                {
                    if (rightendcol > col_max)
                        rightendcol = col_max;
                }
								
                // 判断当前点是否为黑点 若为黑点则不进行对比直接赋值
                if (temp1 < White_Min_Point && rightstartcol == Reference_Col)
                {
                    Right_Edge_Line[row] = (uint8)col;
                    break;
                }
								
                // 判断待对比点是否为白点 若为白点则直接跳过
                if (temp2 > White_Max_Point)
								{
                    continue;
								}
                // 计算对比度
                temp3 = (temp1 - temp2) * 200 / (temp1 + temp2);
                // 判断对比度是否大于阈值 如果大于阈值则认为是行边界  或者已经搜索到该行最边界，则直接赋值行边界
                if (temp3 > Reference_Contrast_Ratio || col == col_max - 1)
                {
                    Right_Edge_Line[row] = col + CONTRASTOFFSET - 1; // 保存当前行右边界
                    // 刷新搜线范围
                    // rightstartcol = (uint8)LimitDouMid(col - SEARCHRANGE, Reference_Col, col);
                    rightendcol = (uint8)LimitDouMid(col + SEARCHRANGE, col, col_max);
                    break;
                }
            }
        }
    }
}




// 将左右边界数据整合到边界图像数组
// boundary_image中，边界用255标记，其他区域为0
void IntegrateBoundaryToImage(void)
{
    uint8 row, col;
    
    // 初始化边界图像为全黑（0）
    for (row = 0; row < IMAGE_HEIGHT; row++)
    {
        for (col = 0; col < IMAGE_WIDTH; col++)
        {
            boundary_image[row][col] = 0;
        }
    }
    
    // 标记左边界（使用灰度值128）
    for (row = 0; row < IMAGE_HEIGHT; row++)
    {
        // 确保边界列坐标在有效范围内
        if (Left_Edge_Line[row] >= 0 && Left_Edge_Line[row] < IMAGE_WIDTH)
        {
            // 在边界位置标记，可适当扩展为多个像素点增强显示
            boundary_image[row][Left_Edge_Line[row]] = 255;
            
            // 可选：向左右各扩展1个像素，让边界更明显
            if (Left_Edge_Line[row] + 1 < IMAGE_WIDTH)
                boundary_image[row][Left_Edge_Line[row] + 1] = 255;
            if (Left_Edge_Line[row] - 1 >= 0)
                boundary_image[row][Left_Edge_Line[row] - 1] = 255;
        }
    }
    
    // 标记右边界（使用灰度值255）
    for (row = 0; row < IMAGE_HEIGHT; row++)
    {
        // 确保边界列坐标在有效范围内
        if (Right_Edge_Line[row] >= 0 && Right_Edge_Line[row] < IMAGE_WIDTH)
        {
            // 在边界位置标记，可适当扩展为多个像素点增强显示
            boundary_image[row][Right_Edge_Line[row]] = 255;
            
            // 可选：向左右各扩展1个像素，让边界更明显
            if (Right_Edge_Line[row] + 1 < IMAGE_WIDTH)
                boundary_image[row][Right_Edge_Line[row] + 1] = 255;
            if (Right_Edge_Line[row] - 1 >= 0)
                boundary_image[row][Right_Edge_Line[row] - 1] = 255;
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
    //find_boundaries_and_centerline();
	
	
	
	Get_Reference_Point();
	Search_Reference_Col();
	Search_line();
	IntegrateBoundaryToImage();
}
