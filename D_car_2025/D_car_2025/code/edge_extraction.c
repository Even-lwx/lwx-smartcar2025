#include "zf_common_headfile.h"
#include <math.h>
#include "edge_extraction.h"

#define IMG_BLACK 0X00 // 0x00是黑
#define IMG_WHITE 0Xff // 0xff为白

// 常用基本变量
extern const uint8 Image_Flags[][9][8];                // 放在图上的数字标记
extern uint8_t binaryImage[IMAGE_HEIGHT][IMAGE_WIDTH]; // 二值化后的原数组
volatile int Left_Line[MT9V03X_H];                     // 左边线数组
volatile int Right_Line[MT9V03X_H];                    // 右边线数组
volatile int Mid_Line[MT9V03X_H];                      // 中线数组
volatile int Road_Wide[MT9V03X_H];                     // 赛宽数组
volatile int White_Column[MT9V03X_W];                  // 每列白列长度
volatile int Search_Stop_Line;                         // 搜索截止行,只记录长度，想要坐标需要用视野高度减去该值
volatile int Boundry_Start_Left;                       // 左右边界起始点
volatile int Boundry_Start_Right;                      // 第一个非丢线点,常规边界起始点
volatile int Left_Lost_Time;                           // 边界丢线数
volatile int Right_Lost_Time;
volatile int Both_Lost_Time;       // 两边同时丢线数
int Longest_White_Column_Left[2];  // 最长白列,[0]是最长白列的长度，也就是Search_Stop_Line搜索截止行，[1】是第某列
int Longest_White_Column_Right[2]; // 最长白列,[0]是最长白列的长度，也就是Search_Stop_Line搜索截止行，[1】是第某列
int Left_Lost_Flag[MT9V03X_H];     // 左丢线数组，丢线置1，没丢线置0
int Right_Lost_Flag[MT9V03X_H];    // 右丢线数组，丢线置1，没丢线置0

/*特殊元素 */
// 十字
volatile int Cross_Flag = 0;
volatile int Left_Down_Find = 0; // 十字使用，找到被置行数，没找到就是0
volatile int Left_Up_Find = 0;   // 四个拐点标志
volatile int Right_Down_Find = 0;
volatile int Right_Up_Find = 0;

// 环岛
volatile int Island_State;      // 环岛状态标志
volatile int Left_Island_Flag;  // 左右环岛标志
volatile int Right_Island_Flag; // 左右环岛标志

// 坡道
volatile int Ramp_Flag = 0; // 坡道标志

/*-------------------------------------------------------------------------------------------------------------------
  @brief     双最长白列巡线
  @param     null
  @return    null
  Sample     Longest_White_Column_Left();
  @note      最长白列巡线，寻找初始边界，丢线，最长白列等基础元素，后续读取这些变量来进行赛道识别
-------------------------------------------------------------------------------------------------------------------*/
void Longest_White_Column() // 最长白列巡线
{
    int i, j;
    int start_column = 20;                 // 最长白列的搜索起始列
    int end_column = MT9V03X_W - 20;       // 最长白列的搜索终止列
    int left_border = 0, right_border = 0; // 临时存储赛道位置
    Longest_White_Column_Left[0] = 0;      // 最长白列,[0]是最长白列的长度，[1】是第某列
    Longest_White_Column_Left[1] = 0;      // 最长白列,[0]是最长白列的长度，[1】是第某列
    Longest_White_Column_Right[0] = 0;     // 最长白列,[0]是最长白列的长度，[1】是第某列
    Longest_White_Column_Right[1] = 0;     // 最长白列,[0]是最长白列的长度，[1】是第某列
    Right_Lost_Time = 0;                   // 边界丢线数
    Left_Lost_Time = 0;
    Boundry_Start_Left = 0; // 第一个非丢线点,常规边界起始点
    Boundry_Start_Right = 0;
    Both_Lost_Time = 0; // 两边同时丢线数

    /*数据清零*/
    for (i = 0; i <= MT9V03X_H - 1; i++) // 纵向遍历
    {
        Right_Lost_Flag[i] = 0;
        Left_Lost_Flag[i] = 0;
        Left_Line[i] = 0;
        Right_Line[i] = MT9V03X_W - 1;
    }
    for (i = 0; i <= MT9V03X_W - 1; i++) // 横向遍历
    {
        White_Column[i] = 0;
    }

    // 从左到右，从下往上，遍历全图记录范围内的每一列白点数量
    for (j = start_column; j <= end_column; j++)
    {
        for (i = MT9V03X_H - 1; i >= 0; i--)
        {
            if (binaryImage[i][j] == IMG_BLACK)
                break;
            else
                White_Column[j]++;
        }
    }

    // 从左到右找左边最长白列
    Longest_White_Column_Left[0] = 0;
    for (i = start_column; i <= end_column; i++)
    {
        if (Longest_White_Column_Left[0] < White_Column[i]) // 找最长的那一列
        {
            Longest_White_Column_Left[0] = White_Column[i]; // 【0】是白列长度
            Longest_White_Column_Left[1] = i;               // 【1】是下标，第i列
        }
    }
    // 从右到左找右左边最长白列
    Longest_White_Column_Right[0] = 0;           // 【0】是白列长度
    for (i = end_column; i >= start_column; i--) // 从右往左，注意条件，找到左边最长白列位置就可以停了  这里搜索范围可以优化？？？？
    {
        if (Longest_White_Column_Right[0] < White_Column[i]) // 找最长的那一列
        {
            Longest_White_Column_Right[0] = White_Column[i]; // 【0】是白列长度
            Longest_White_Column_Right[1] = i;               // 【1】是下标，第j列
        }
    }

    Search_Stop_Line = Longest_White_Column_Left[0];                // 搜索截止行选取左或者右区别不大，他们两个理论上是一样的
    for (i = MT9V03X_H - 1; i >= MT9V03X_H - Search_Stop_Line; i--) // 常规巡线
    {
        /*找右边界 */
        for (j = Longest_White_Column_Right[1]; j <= MT9V03X_W - 1 - 2; j++)
        {
            if (binaryImage[i][j] == IMG_WHITE && binaryImage[i][j + 1] == IMG_BLACK && binaryImage[i][j + 2] == IMG_BLACK) // 白黑黑，找到右边界
            {
                right_border = j;
                Right_Lost_Flag[i] = 0; // 右丢线数组，丢线置1，不丢线置0
                break;
            }
            else if (j >= MT9V03X_W - 1 - 2) // 没找到右边界，把屏幕最右赋值给右边界
            {
                right_border = j;
                Right_Lost_Flag[i] = 1; // 右丢线数组，丢线置1，不丢线置0
                break;
            }
        }
        /*找左边界 */
        for (j = Longest_White_Column_Left[1]; j >= 0 + 2; j--) // 往左边扫描
        {
            if (binaryImage[i][j] == IMG_WHITE && binaryImage[i][j - 1] == IMG_BLACK && binaryImage[i][j - 2] == IMG_BLACK) // 黑黑白认为到达左边界
            {
                left_border = j;
                Left_Lost_Flag[i] = 0; // 左丢线数组，丢线置1，不丢线置0
                break;
            }
            else if (j <= 0 + 2)
            {
                left_border = j;       // 找到头都没找到边，就把屏幕最左右当做边界
                Left_Lost_Flag[i] = 1; // 左丢线数组，丢线置1，不丢线置0
                break;
            }
        }
        Left_Line[i] = left_border;   // 左边线线数组
        Right_Line[i] = right_border; // 右边线线数组
    }

    for (i = MT9V03X_H - 1; i >= 0; i--) // 赛道数据初步分析
    {
        if (Left_Lost_Flag[i] == 1) // 单边丢线数
            Left_Lost_Time++;
        if (Right_Lost_Flag[i] == 1)
            Right_Lost_Time++;
        if (Left_Lost_Flag[i] == 1 && Right_Lost_Flag[i] == 1) // 双边丢线数
            Both_Lost_Time++;
        if (Boundry_Start_Left == 0 && Left_Lost_Flag[i] != 1) // 记录第一个非丢线点，边界起始点
            Boundry_Start_Left = i;
        if (Boundry_Start_Right == 0 && Right_Lost_Flag[i] != 1)
            Boundry_Start_Right = i;
        Road_Wide[i] = Right_Line[i] - Left_Line[i];
    }

    // debug使用，屏幕显示相关参数
    //    ips200_showint16(0,0, Longest_White_Column_Right[0]);//【0】是白列长度
    //    ips200_showint16(0,1, Longest_White_Column_Right[1]);//【1】是下标，第j列)
    //    ips200_showint16(0,2, Longest_White_Column_Left[0]);//【0】是白列长度
    //    ips200_showint16(0,3, Longest_White_Column_Left[1]);//【1】是下标，第j列)
}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     边界显示，用于图传，显示到屏幕上，
  @param     null
  @return    null
  Sample     直接调用
  @note      显示左中右边界，中线，
                                           正常情况下不要用，因为直接在原图上写入了边界信息
                                           会对元素判断造成干扰的，调试时候调用
-------------------------------------------------------------------------------------------------------------------*/
void Show_Boundry(void)
{
    int16 i;
    for (i = MT9V03X_H - 1; i >= MT9V03X_H - Search_Stop_Line; i--) // 从最底下往上扫描
    {
        binaryImage[i][Left_Line[i] + 1] = IMG_BLACK;
        binaryImage[i][(Left_Line[i] + Right_Line[i]) >> 1] = IMG_BLACK;
        binaryImage[i][Right_Line[i] - 1] = IMG_BLACK;
    }

    // 在屏幕理论中线处显示红线，用于调整摄像头
     
}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     左补线
  @param     补线的起点，终点
  @return    null
  Sample     Left_Add_Line(int x1,int y1,int x2,int y2);
  @note      补的直接是边界，点最好是可信度高的,不要乱补
-------------------------------------------------------------------------------------------------------------------*/
void Left_Add_Line(int x1, int y1, int x2, int y2) // 左补线,补的是边界
{
    int i, max, a1, a2;
    int hx;
    if (x1 >= MT9V03X_W - 1) // 起始点位置校正，排除数组越界的可能
        x1 = MT9V03X_W - 1;
    else if (x1 <= 0)
        x1 = 0;
    if (y1 >= MT9V03X_H - 1)
        y1 = MT9V03X_H - 1;
    else if (y1 <= 0)
        y1 = 0;
    if (x2 >= MT9V03X_W - 1)
        x2 = MT9V03X_W - 1;
    else if (x2 <= 0)
        x2 = 0;
    if (y2 >= MT9V03X_H - 1)
        y2 = MT9V03X_H - 1;
    else if (y2 <= 0)
        y2 = 0;
    a1 = y1;
    a2 = y2;
    if (a1 > a2) // 坐标互换
    {
        max = a1;
        a1 = a2;
        a2 = max;
    }
    for (i = a1; i <= a2; i++) // 根据斜率补线即可
    {
        hx = (i - y1) * (x2 - x1) / (y2 - y1) + x1;
        if (hx >= MT9V03X_W)
            hx = MT9V03X_W;
        else if (hx <= 0)
            hx = 0;
        Left_Line[i] = hx;
    }
}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     右补线
  @param     补线的起点，终点
  @return    null
  Sample     Right_Add_Line(int x1,int y1,int x2,int y2);
  @note      补的直接是边界，点最好是可信度高的，不要乱补
-------------------------------------------------------------------------------------------------------------------*/
void Right_Add_Line(int x1, int y1, int x2, int y2) // 右补线,补的是边界
{
    int i, max, a1, a2;
    int hx;
    if (x1 >= MT9V03X_W - 1) // 起始点位置校正，排除数组越界的可能
        x1 = MT9V03X_W - 1;
    else if (x1 <= 0)
        x1 = 0;
    if (y1 >= MT9V03X_H - 1)
        y1 = MT9V03X_H - 1;
    else if (y1 <= 0)
        y1 = 0;
    if (x2 >= MT9V03X_W - 1)
        x2 = MT9V03X_W - 1;
    else if (x2 <= 0)
        x2 = 0;
    if (y2 >= MT9V03X_H - 1)
        y2 = MT9V03X_H - 1;
    else if (y2 <= 0)
        y2 = 0;
    a1 = y1;
    a2 = y2;
    if (a1 > a2) // 坐标互换
    {
        max = a1;
        a1 = a2;
        a2 = max;
    }
    for (i = a1; i <= a2; i++) // 根据斜率补线即可
    {
        hx = (i - y1) * (x2 - x1) / (y2 - y1) + x1;
        if (hx >= MT9V03X_W)
            hx = MT9V03X_W;
        else if (hx <= 0)
            hx = 0;
        Right_Line[i] = hx;
    }
}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     找下面的两个拐点，供十字使用
  @param     搜索的范围起点，终点
  @return    修改两个全局变量
             Right_Down_Find=0;
             Left_Down_Find=0;
  Sample     Find_Down_Point(int start,int end)
  @note      运行完之后查看对应的变量，注意，没找到时对应变量将是0
-------------------------------------------------------------------------------------------------------------------*/
void Find_Down_Point(int start, int end)
{
    int i, t;
    Right_Down_Find = 0;
    Left_Down_Find = 0;
    if (start < end)
    {
        t = start;
        start = end;
        end = t;
    }
    if (start >= MT9V03X_H - 1 - 5) // 下面5行数据不稳定，不能作为边界点来判断，舍弃
        start = MT9V03X_H - 1 - 5;
    if (end <= MT9V03X_H - Search_Stop_Line)
        end = MT9V03X_H - Search_Stop_Line;
    if (end <= 5)
        end = 5;
    for (i = start; i >= end; i--)
    {
        if (Left_Down_Find == 0 &&                       // 只找第一个符合条件的点
            abs(Left_Line[i] - Left_Line[i + 1]) <= 5 && // 角点的阈值可以更改
            abs(Left_Line[i + 1] - Left_Line[i + 2]) <= 5 &&
            abs(Left_Line[i + 2] - Left_Line[i + 3]) <= 5 &&
            (Left_Line[i] - Left_Line[i - 2]) >= 8 &&
            (Left_Line[i] - Left_Line[i - 3]) >= 15 &&
            (Left_Line[i] - Left_Line[i - 4]) >= 15)
        {
            Left_Down_Find = i; // 获取行数即可
        }
        if (Right_Down_Find == 0 &&                        // 只找第一个符合条件的点
            abs(Right_Line[i] - Right_Line[i + 1]) <= 5 && // 角点的阈值可以更改
            abs(Right_Line[i + 1] - Right_Line[i + 2]) <= 5 &&
            abs(Right_Line[i + 2] - Right_Line[i + 3]) <= 5 &&
            (Right_Line[i] - Right_Line[i - 2]) <= -8 &&
            (Right_Line[i] - Right_Line[i - 3]) <= -15 &&
            (Right_Line[i] - Right_Line[i - 4]) <= -15)
        {
            Right_Down_Find = i;
        }
        if (Left_Down_Find != 0 && Right_Down_Find != 0) // 两个找到就退出
        {
            break;
        }
    }
}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     找上面的两个拐点，供十字使用
  @param     搜索的范围起点，终点
  @return    修改两个全局变量
             Left_Up_Find=0;
             Right_Up_Find=0;
  Sample     Find_Up_Point(int start,int end)
  @note      运行完之后查看对应的变量，注意，没找到时对应变量将是0
-------------------------------------------------------------------------------------------------------------------*/
void Find_Up_Point(int start, int end)
{
    int i, t;
    Left_Up_Find = 0;
    Right_Up_Find = 0;
    if (start < end)
    {
        t = start;
        start = end;
        end = t;
    }
    if (end <= MT9V03X_H - Search_Stop_Line)
        end = MT9V03X_H - Search_Stop_Line;
    if (end <= 5) // 及时最长白列非常长，也要舍弃部分点，防止数组越界
        end = 5;
    if (start >= MT9V03X_H - 1 - 5) // 下面5行数据不稳定，不能作为边界点来判断，舍弃
        start = MT9V03X_H - 1 - 5;
    for (i = start; i >= end; i--)
    {
        if (Left_Up_Find == 0 && // 只找第一个符合条件的点
            abs(Left_Line[i] - Left_Line[i - 1]) <= 5 &&
            abs(Left_Line[i - 1] - Left_Line[i - 2]) <= 5 &&
            abs(Left_Line[i - 2] - Left_Line[i - 3]) <= 5 &&
            (Left_Line[i] - Left_Line[i + 2]) >= 8 &&
            (Left_Line[i] - Left_Line[i + 3]) >= 15 &&
            (Left_Line[i] - Left_Line[i + 4]) >= 15)
        {
            Left_Up_Find = i; // 获取行数即可
        }
        if (Right_Up_Find == 0 &&                          // 只找第一个符合条件的点
            abs(Right_Line[i] - Right_Line[i - 1]) <= 5 && // 下面两行位置差不多
            abs(Right_Line[i - 1] - Right_Line[i - 2]) <= 5 &&
            abs(Right_Line[i - 2] - Right_Line[i - 3]) <= 5 &&
            (Right_Line[i] - Right_Line[i + 2]) <= -8 &&
            (Right_Line[i] - Right_Line[i + 3]) <= -15 &&
            (Right_Line[i] - Right_Line[i + 4]) <= -15)
        {
            Right_Up_Find = i; // 获取行数即可
        }
        if (Left_Up_Find != 0 && Right_Up_Find != 0) // 下面两个找到就出去
        {
            break;
        }
    }
    if (abs(Right_Up_Find - Left_Up_Find) >= 30) // 纵向撕裂过大，视为误判
    {
        Right_Up_Find = 0;
        Left_Up_Find = 0;
    }
}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     左边界延长
  @param     延长起始行数，延长到某行
  @return    null
  Sample     Stop_Detect(void)
  @note      从起始点向上找5个点，算出斜率，向下延长，直至结束点
-------------------------------------------------------------------------------------------------------------------*/
void Lengthen_Left_Boundry(int start, int end)
{
    int i, t;
    float k = 0;
    if (start >= MT9V03X_H - 1) // 起始点位置校正，排除数组越界的可能
        start = MT9V03X_H - 1;
    else if (start <= 0)
        start = 0;
    if (end >= MT9V03X_H - 1)
        end = MT9V03X_H - 1;
    else if (end <= 0)
        end = 0;
    if (end < start) //++访问，坐标互换
    {
        t = end;
        end = start;
        start = t;
    }

    if (start <= 5) // 因为需要在开始点向上找5个点，对于起始点过于靠上，不能做延长，只能直接连线
    {
        Left_Add_Line(Left_Line[start], start, Left_Line[end], end);
    }

    else
    {
        k = (float)(Left_Line[start] - Left_Line[start - 4]) / 5.0; // 这里的k是1/斜率
        for (i = start; i <= end; i++)
        {
            Left_Line[i] = (int)(i - start) * k + Left_Line[start]; //(x=(y-y1)*k+x1),点斜式变形
            if (Left_Line[i] >= MT9V03X_W - 1)
            {
                Left_Line[i] = MT9V03X_W - 1;
            }
            else if (Left_Line[i] <= 0)
            {
                Left_Line[i] = 0;
            }
        }
    }
}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     右左边界延长
  @param     延长起始行数，延长到某行
  @return    null
  Sample     Stop_Detect(void)
  @note      从起始点向上找3个点，算出斜率，向下延长，直至结束点
-------------------------------------------------------------------------------------------------------------------*/
void Lengthen_Right_Boundry(int start, int end)
{
    int i, t;
    float k = 0;
    if (start >= MT9V03X_H - 1) // 起始点位置校正，排除数组越界的可能
        start = MT9V03X_H - 1;
    else if (start <= 0)
        start = 0;
    if (end >= MT9V03X_H - 1)
        end = MT9V03X_H - 1;
    else if (end <= 0)
        end = 0;
    if (end < start) //++访问，坐标互换
    {
        t = end;
        end = start;
        start = t;
    }


    if (start <= 5) // 因为需要在开始点向上找5个点，对于起始点过于靠上，不能做延长，只能直接连线
    {
        Right_Add_Line(Right_Line[start], start, Right_Line[end], end);
    }
    else
    {
        k = (float)(Right_Line[start] - Right_Line[start - 4]) / 5.0; // 这里的k是1/斜率
        for (i = start; i <= end; i++)
        {
            Right_Line[i] = (int)(i - start) * k + Right_Line[start]; //(x=(y-y1)*k+x1),点斜式变形
            if (Right_Line[i] >= MT9V03X_W - 1)
            {
                Right_Line[i] = MT9V03X_W - 1;
            }
            else if (Right_Line[i] <= 0)
            {
                Right_Line[i] = 0;
            }
        }
    }
}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     十字检测
  @param     null
  @return    null
  Sample     Cross_Detect(void);
  @note      利用四个拐点判别函数，查找四个角点，根据找到拐点的个数决定是否补线
-------------------------------------------------------------------------------------------------------------------*/
void Cross_Detect()
{
    int down_search_start = 0; // 下点搜索开始行
    Cross_Flag = 0;
    if (Island_State == 0 && Ramp_Flag == 0) // 与环岛互斥开
    {
        Left_Up_Find = 0;
        Right_Up_Find = 0;
        if (Both_Lost_Time >= 10) // 十字必定有双边丢线，在有双边丢线的情况下再开始找角点
        {
            Find_Up_Point(MT9V03X_H - 1, 0);
            if (Left_Up_Find == 0 && Right_Up_Find == 0) // 只要没有同时找到两个上点，直接结束
            {
                return;
            }
        }
        if (Left_Up_Find != 0 && Right_Up_Find != 0) // 找到两个上点，就找到十字了
        {
            Cross_Flag = 1;                                                                  // 对应标志位，便于各元素互斥掉
            down_search_start = Left_Up_Find > Right_Up_Find ? Left_Up_Find : Right_Up_Find; // 用两个上拐点坐标靠下者作为下点的搜索上限
            Find_Down_Point(MT9V03X_H - 5, down_search_start + 2);                           // 在上拐点下2行作为下点的截止行
            if (Left_Down_Find <= Left_Up_Find)
            {
                Left_Down_Find = 0; // 下点不可能比上点还靠上
            }
            if (Right_Down_Find <= Right_Up_Find)
            {
                Right_Down_Find = 0; // 下点不可能比上点还靠上
            }
            if (Left_Down_Find != 0 && Right_Down_Find != 0)
            { // 四个点都在，无脑连线，这种情况显然很少
                Left_Add_Line(Left_Line[Left_Up_Find], Left_Up_Find, Left_Line[Left_Down_Find], Left_Down_Find);
                Right_Add_Line(Right_Line[Right_Up_Find], Right_Up_Find, Right_Line[Right_Down_Find], Right_Down_Find);
            }
            else if (Left_Down_Find == 0 && Right_Down_Find != 0) // 11//这里使用的都是斜率补线
            {                                                     // 三个点                                     //01
                Lengthen_Left_Boundry(Left_Up_Find - 1, MT9V03X_H - 1);
                Right_Add_Line(Right_Line[Right_Up_Find], Right_Up_Find, Right_Line[Right_Down_Find], Right_Down_Find);
            }
            else if (Left_Down_Find != 0 && Right_Down_Find == 0) // 11
            {                                                     // 三个点                                     //10
                Left_Add_Line(Left_Line[Left_Up_Find], Left_Up_Find, Left_Line[Left_Down_Find], Left_Down_Find);
                Lengthen_Right_Boundry(Right_Up_Find - 1, MT9V03X_H - 1);
            }
            else if (Left_Down_Find == 0 && Right_Down_Find == 0) // 11
            {                                                     // 就俩上点                                   //00
                Lengthen_Left_Boundry(Left_Up_Find - 1, MT9V03X_H - 1);
                Lengthen_Right_Boundry(Right_Up_Find - 1, MT9V03X_H - 1);
            }
        }
        else
        {
            Cross_Flag = 0;
        }
    }
    // 角点相关变量，debug使用
    // ips200_showuint8(0,12,Cross_Flag);
    //    ips200_showuint8(0,13,Island_State);
    //    ips200_showuint8(50,12,Left_Up_Find);
    //    ips200_showuint8(100,12,Right_Up_Find);
    //    ips200_showuint8(50,13,Left_Down_Find);
    //    ips200_showuint8(100,13,Right_Down_Find);
}


/**
* @brief   出界判断,选取图像底部中间10*3区域进行判断
* @param   *binaryImage[IMAGE_HEIGHT][IMAGE_WIDTH]  原始图像
* @retval   0:正常 1:出界
*/
uint8 image_out_of_bounds(uint8 binaryImage[IMAGE_HEIGHT][IMAGE_WIDTH])
{
    int sum=0;
    for(int i=0;i<10;i++)
    {
        for(int j=0;j<3;j++)
        {
            sum+=image_copy[IMAGE_HEIGHT-j][IMAGE_WIDTH/2-5+i];
        }
    }
    int average = sum / 30; // 计算平均值
    if(average<130)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}