#include "zf_common_headfile.h"
#include "island.h"

// 常用基本变量
extern uint8 image_two_value[MT9V03X_H][MT9V03X_W]; // 二值化后的原数组
extern volatile int Left_Line[MT9V03X_H];           // 左边线数组
extern volatile int Right_Line[MT9V03X_H];          // 右边线数组
extern volatile int Search_Stop_Line;               // 搜索截止行
extern volatile int Boundry_Start_Left;             // 左右边界起始点
extern volatile int Boundry_Start_Right;            // 第一个非丢线点,常规边界起始点
extern volatile int Left_Lost_Time;                 // 边界丢线数
extern volatile int Right_Lost_Time;
extern volatile int Both_Lost_Time; // 两边同时丢线数
extern volatile float Err;          // 摄像头误差

// 十字变量
extern volatile int Cross_Flag;

// 环岛变量
volatile int Island_State = 0;      // 环岛状态标志
volatile int Left_Island_Flag = 0;  // 左右环岛标志
volatile int Right_Island_Flag = 0; // 左右环岛标志
volatile int Left_Up_Guai[2];       // 四个拐点的坐标存储，[0]存y，第某行，{1}存x，第某列
volatile int Right_Up_Guai[2];      // 四个拐点的坐标存储，[0]存y，第某行，{1}存x，第某列

// 斑马线
extern volatile int Zebra_State;

// 坡道标志位
extern volatile int Ramp_Flag; // 坡道标志

///*-------------------------------------------------------------------------------------------------------------------
//  @brief     环岛检测
//  @param     null
//  @return    null
//  Sample     Island_Detect(void);
//  @note      根据网上的开源代码修改的
//-------------------------------------------------------------------------------------------------------------------*/
// void Island_Detect()
//{
//    static float k = 0;                      // 3和5状态的k
//    static int island_state_5_down[2] = {0}; // 状态5时即将离开环岛，左右边界边最低点，[0]存y，第某行，{1}存x，第某列
//    static int island_state_3_up[2] = {0};   // 状态3时即将进入环岛用，左右上面角点[0]存y，第某行，{1}存x，第某列
//    static int left_down_guai[2] = {0};      // 四个拐点的坐标存储，[0]存y，第某行，{1}存x，第某列
//    static int right_down_guai[2] = {0};     // 四个拐点的坐标存储，[0]存y，第某行，{1}存x，第某列
//    int monotonicity_change_line[2];         // 单调性改变点坐标，[0]寸某行，[1]寸某列
//    int monotonicity_change_left_flag = 0;   // 不转折是0
//    int monotonicity_change_right_flag = 0;  // 不转折是0
//    int continuity_change_right_flag = 0;    // 连续是0
//    int continuity_change_left_flag = 0;     // 连续是0

//    if (Cross_Flag == 0 && Island_State == 0 && Ramp_Flag == 0) //&&Ramp_Flag==0
//    {
//        continuity_change_left_flag = Continuity_Change_Left(MT9V03X_H - 1 - 15, 15); // 连续性判断
//        continuity_change_right_flag = Continuity_Change_Right(MT9V03X_H - 1 - 15, 15);

//        monotonicity_change_left_flag = Monotonicity_Change_Left(MT9V03X_H - 1 - 15, 15);
//			  monotonicity_change_right_flag = Monotonicity_Change_Right(MT9V03X_H - 1 - 15, 15);

//        if (Left_Island_Flag == 0) // 左环
//        {
//            if (
//								monotonicity_change_right_flag == 0 &&                                      // 右边是单调的
//                continuity_change_left_flag != 0 &&                                         // 左边是不连续的
//                continuity_change_right_flag == 0 &&                                        // 左环岛右边是连续的
//               Left_Lost_Time >= 10 &&                                                     // 左边丢线很多
//               Left_Lost_Time <= 100 &&                                                    // 也不能全丢了
//                Right_Lost_Time <= 10 &&                                                    // 右边丢线较少
//                Search_Stop_Line >= 100 &&                                                  // 搜索截止行看到很远
//                Boundry_Start_Left >= MT9V03X_H - 8 && Boundry_Start_Right >= MT9V03X_H - 8 // 边界起始点靠下
//                )                                                                           // 双边丢线少
//            {
//
//                    Island_State = 1;
//                    Left_Island_Flag = 1;
//                }
//                else // 误判，归零
//                {
//                    Island_State = 0;
//                    Left_Island_Flag = 0;
//                }
//            }
//        }
//        if (Right_Island_Flag == 0) // 右环
//        {
//            if (
//								monotonicity_change_right_flag == 1 &&
//                continuity_change_left_flag <=30 &&                                         // 右环岛左边是连续的
//                continuity_change_right_flag != 0 &&                                        // 右边是不连续的
//                Right_Lost_Time >= 10 &&                                                    // 右丢线多
//                Right_Lost_Time <= 100 &&                                                   // 右丢线不能太多
//               Left_Lost_Time <= 10 &&                                                     // 左丢线少
//                Search_Stop_Line >= 100 &&                                                  // 搜索截止行看到很远
//                Boundry_Start_Left >= MT9V03X_H - 10 && Boundry_Start_Right >= MT9V03X_H - 10 // 边界起始点靠下
//            )
//            {
//								buzzer_on(60);
//                Island_State = 1;
//                Right_Island_Flag = 1;
//            }
//            else
//            {

//                Island_State = 0;
//                Right_Island_Flag = 0;
//            }
//        }
//

//
//
//
//
//
//
//    if (Left_Island_Flag == 1) // 1状态下拐点还在，没丢线
//    {
//        if (Island_State == 1)
//        {
//            monotonicity_change_line[0] = Monotonicity_Change_Left(30, 5); // 寻找单调性改变点
//            monotonicity_change_line[1] = Left_Line[monotonicity_change_line[0]];
//            Left_Add_Line((int)(monotonicity_change_line[1] * 0.15), MT9V03X_H - 1, monotonicity_change_line[1], monotonicity_change_line[0]);
//            if ((Island_State == 1) && (Boundry_Start_Left < 30)) // 下方当丢线时候进2
//            {

//                Island_State = 2;
//            }
//        }

//        else if (Island_State == 2) // 下方角点消失，2状态时下方应该是丢线，上面是弧线
//        {
//            monotonicity_change_line[0] = Monotonicity_Change_Left(70, 10); // 寻找单调性改变点
//            monotonicity_change_line[1] = Left_Line[monotonicity_change_line[0]];
//            Left_Add_Line((int)(monotonicity_change_line[1] * 0.1), MT9V03X_H - 1, monotonicity_change_line[1], monotonicity_change_line[0]);
//            if (Island_State == 2 && (Boundry_Start_Left >= MT9V03X_H - 5 || monotonicity_change_line[0] > 50)) // 当圆弧靠下时候，进3
//            {

//                Island_State = 3; // 最长白列寻找范围也要改，见camera.c
//                Left_Island_Flag = 1;
//            }
//        }
//        else if (Island_State == 3) // 3状态准备进环，寻找上拐点，连线
//        {
//            if (k != 0)
//            {
//                K_Draw_Line(k, MT9V03X_W - 30, MT9V03X_H - 1, 0); // k是刚刚算出来的，静态变量存着
//                Longest_White_Column();                           // 刷新边界数据
//            }
//            else
//            {
//                Left_Up_Guai[0] = Find_Left_Up_Point(40, 5); // 找左上拐点
//                Left_Up_Guai[1] = Left_Line[Left_Up_Guai[0]];

//                if (Left_Up_Guai[0] < 5) // 此处为了防止误判，如果经常从3状态归零，建议修改此处判断条件
//                {
//                    Island_State = 0;
//                    Left_Island_Flag = 0;
//                }

//                if (k == 0 && (15 <= Left_Up_Guai[0] && Left_Up_Guai[0] < 50) && (50 < Left_Up_Guai[1] && Left_Up_Guai[1] < 110)) // 拐点出现在一定范围内，认为是拐点出现
//                {
//                    island_state_3_up[0] = Left_Up_Guai[0];
//                    island_state_3_up[1] = Left_Up_Guai[1];
//                    k = (float)((float)(MT9V03X_H - island_state_3_up[0]) / (float)(MT9V03X_W - 20 - island_state_3_up[1]));
//                    K_Draw_Line(k, MT9V03X_W - 30, MT9V03X_H - 1, 0); // 记录下第一次上点出现时位置，针对这个环岛拉一条死线，入环
//                    Longest_White_Column();                           // 刷新边界数据
//                }
//            }
//            if ((Island_State == 3) && (abs(gz) >= 60)) // 纯靠陀螺仪积分入环
//            {
//                k = 0;                  // 斜率清零
//                Island_State = 4;       // 这一步时候顺便调整了最长白列的搜索范围
//                Longest_White_Column(); // 最长白列
//            }
//        }
//        else if (Island_State == 4) // 状态4已经在里面
//        {
//            if (abs(gz) > 200) // 积分200度以后在打开出环判断
//            {
//                monotonicity_change_line[0] = Monotonicity_Change_Right(MT9V03X_H - 10, 10); // 单调性改变
//                monotonicity_change_line[1] = Right_Line[monotonicity_change_line[0]];
//                if ((Island_State == 4) && (35 <= monotonicity_change_line[0] && monotonicity_change_line[0] <= 55 && monotonicity_change_line[1] >= 10)) // 单调点靠下，进去5
//                {                                                                                                                                         // monotonicity_change_line[1]>=90&&
//                    island_state_5_down[0] = MT9V03X_H - 1;
//                    island_state_5_down[1] = Right_Line[MT9V03X_H - 1];
//                    k = (float)((float)(MT9V03X_H - monotonicity_change_line[0]) / (float)(island_state_5_down[1] - monotonicity_change_line[1]));
//                    K_Add_Boundry_Right(k, island_state_5_down[1], island_state_5_down[0], 0); // 和状态3一样，记住斜率
//                    Island_State = 5;
//                }
//            }
//        }
//        else if (Island_State == 5) // 出环
//        {
//            K_Add_Boundry_Right(k, island_state_5_down[1], island_state_5_down[0], 0);
//            if ((Island_State == 5) && (Boundry_Start_Right < MT9V03X_H - 20)) // 右边先丢线
//            {
//                Island_State = 6;
//            }
//        }
//        else if (Island_State == 6) // 还在出
//        {
//            K_Add_Boundry_Right(k, island_state_5_down[1], island_state_5_down[0], 0);
//            if ((Island_State == 6) && ((Boundry_Start_Right > MT9V03X_H - 10) || (abs(gz) >= 320))) // 右边不丢线
//            {                                                                                        //
//                k = 0;
//                Island_State = 7;
//            }
//        }
//        else if (Island_State == 7) // 基本出去了，在寻找拐点，准备离开环岛状态
//        {
//            Left_Up_Guai[0] = Find_Left_Up_Point(MT9V03X_H - 10, 0); // 获取左上点坐标，坐标点合理去8
//            Left_Up_Guai[1] = Left_Line[Left_Up_Guai[0]];
//            if ((Island_State == 7) && (Left_Up_Guai[1] <= 100) && (5 <= Left_Up_Guai[0] && Left_Up_Guai[0] <= MT9V03X_H - 20)) // 注意这里，对横纵坐标都有要求
//            {
//                Island_State = 8; // 基本上找到拐点就去8
//            }
//        }
//        else if (Island_State == 8) // 连线，出环最后一步
//        {
//            Left_Up_Guai[0] = Find_Left_Up_Point(MT9V03X_H - 1, 10); // 获取左上点坐标
//            Left_Up_Guai[1] = Left_Line[Left_Up_Guai[0]];
//            Lengthen_Left_Boundry(Left_Up_Guai[0] - 1, MT9V03X_H - 1);
//            if ((Island_State == 8) && (Left_Up_Guai[0] >= MT9V03X_H - 20 || (Left_Up_Guai[0] < 10 && Boundry_Start_Left >= MT9V03X_H - 10))) // 当拐点靠下时候，认为出环了，环岛结束
//            {                                                                                                                                 // 要么拐点靠下，要么拐点丢了，切下方不丢线，认为环岛结束了
//                gz = 0;                                                                                                                       // 数据清零
//                Island_State = 9;                                                                                                             // 8时候环岛基本结束了，为了防止连续判环，8后会进9，大概几十毫秒后归零，
//                Left_Island_Flag = 0;
//            }
//        }
//    }
//
//
//
//
//
//
//    else if (Right_Island_Flag == 1)
//    {
//        if (Island_State == 1) // 1状态下拐点还在，没丢线
//        {
//            monotonicity_change_line[0] = Monotonicity_Change_Right(MT9V03X_H - 1 - 15, 15); // 单调性改变
//            monotonicity_change_line[1] = Right_Line[monotonicity_change_line[0]];
//
//            Right_Add_Line((int)(MT9V03X_W - 1 - (monotonicity_change_line[1] * 0.15)), MT9V03X_H - 1, monotonicity_change_line[1], monotonicity_change_line[0]); // 右边补出口位置的线
//
//            if (monotonicity_change_line[0] > 50)                                                                                                                 // 右下角先丢线
//            {
//
//                Island_State = 2;
//            }
//        }

//        else if (Island_State == 2) // 下面已经出现大弧线，且上方出现角点
//        {
//
//            if (k != 0) // 将角点与下方连接，画一条死线
//            {
//								buzzer_on(50);
//                K_Draw_Line(k, 30, MT9V03X_H - 1, 0);
//                Longest_White_Column(); // 刷新最长白列
//            }
//            else
//            {
//                Right_Up_Guai[0] = Find_Right_Up_Point(MT9V03X_H - 1 - 15, 15); // 找右上拐点
//                Right_Up_Guai[1] = Right_Line[Right_Up_Guai[0]];

//

//                if (k == 0 && (15 <= Right_Up_Guai[0] && Right_Up_Guai[0] < MT9V03X_H - 1 - 15) && (70 < Right_Up_Guai[1] && Right_Up_Guai[1] < 150)) // 找第一个符合条件的角点，连线
//                {
//                    island_state_3_up[0] = Right_Up_Guai[0];
//                    island_state_3_up[1] = Right_Up_Guai[1];
//                    k = (float)((float)(MT9V03X_H - island_state_3_up[0]) / (float)(20 - island_state_3_up[1]));
//                    K_Draw_Line(k, 30, MT9V03X_H - 1, 0);
//										system_lastcount=system_count;
//                    Longest_White_Column(); // 刷新赛道数据
//                }
//            }
//
//            if ((Island_State == 2) &&k!=0&&	system_count-system_lastcount>100)
//            {
//                k = 0; // 斜率清零
//                Island_State = 3;
//                Longest_White_Column(); // 最长白列
//            } // 记得去最长白列那边改一下，区分下左右环岛
//        }
//        else if (Island_State == 3) // 4状态完全进去环岛了
//        {
//            if (gz > 200) // 环岛积分200度后再打开单调转折判断
//            {
//                monotonicity_change_line[0] = Monotonicity_Change_Left(90, 10); // 单调性改变
//                monotonicity_change_line[1] = Left_Line[monotonicity_change_line[0]];
//                if ((Island_State == 4) && (35 <= monotonicity_change_line[0] && monotonicity_change_line[0] <= 55 && monotonicity_change_line[1] < MT9V03X_W - 10)) // 单调点靠下，进去5
//                {                                                                                                                                                    // monotonicity_change_line[1]<=120&&
//                    island_state_5_down[0] = MT9V03X_H - 1;
//                    island_state_5_down[1] = Left_Line[MT9V03X_H - 1] - 5; // 抓住第一次出现的斜率，定死
//                    k = (float)((float)(MT9V03X_H - monotonicity_change_line[0]) / (float)(island_state_5_down[1] - monotonicity_change_line[1]));
//                    K_Add_Boundry_Left(k, island_state_5_down[1], island_state_5_down[0], 0);
//                    Island_State = 5;
//                }
//            }
//        }
//        else if (Island_State == 5) // 准备出环岛
//        {
//            K_Add_Boundry_Left(k, island_state_5_down[1], island_state_5_down[0], 0);
//            if (Island_State == 5 && Boundry_Start_Left < MT9V03X_H - 20) // 左边先丢线
//            {
//                Island_State = 6;
//            }
//        }
//        else if (Island_State == 6) // 继续出
//        {
//            K_Add_Boundry_Left(k, island_state_5_down[1], island_state_5_down[0], 0);
//            if ((Island_State == 6) && (Boundry_Start_Left > MT9V03X_H - 10 || abs(gz) >= 320)) // 左边先丢线
//            {                                                                                   //
//                k = 0;
//                Island_State = 7;
//            }
//        }
//        else if (Island_State == 7) // 基本出环岛，找角点
//        {
//            Right_Up_Guai[0] = Find_Right_Up_Point(MT9V03X_H - 10, 0); // 获取左上点坐标，找到了去8
//            Right_Up_Guai[1] = Right_Line[Right_Up_Guai[0]];
//            if ((Island_State == 7) && ((Right_Up_Guai[1] >= MT9V03X_W - 88 && (5 <= Right_Up_Guai[0] && Right_Up_Guai[0] <= MT9V03X_H - 20)))) // 注意这里，对横纵坐标都有要求，因为赛道不一样，会意外出现拐点
//            {                                                                                                                                   // 当角点位置合理时，进8
//                Island_State = 8;
//            }
//        }
//        else if (Island_State == 8) // 环岛8
//        {
//            Right_Up_Guai[0] = Find_Right_Up_Point(MT9V03X_H - 1, 10); // 获取右上点坐标
//            Right_Up_Guai[1] = Right_Line[Right_Up_Guai[0]];
//            Lengthen_Right_Boundry(Right_Up_Guai[0] - 1, MT9V03X_H - 1);
//            if ((Island_State == 8) && (Right_Up_Guai[0] >= MT9V03X_H - 20 || (Right_Up_Guai[0] < 10 && Boundry_Start_Left >= MT9V03X_H - 10))) // 当拐点靠下时候，认为出环了，环岛结束
//            {                                                                                                                                   // 角点靠下，或者下端不丢线，认为出环了
//                gz = 0;
//                Island_State = 9;
//                Right_Island_Flag = 0;
//            }
//        }
//    }

//    //    if(Err>=0)
//    //    {
//    //       Flag_Show_101(MT9V03X_W,image_two_value,(uint8)(uint8)Err/10);//上面那个数字
//    //       Flag_Show_102(MT9V03X_W,image_two_value,(uint8)((uint8)Err%10));//下面的数字
//    //    }
//    //    else
//    //    {
//    //        Flag_Show_101(MT9V03X_W,image_two_value,(uint8)(uint8)-Err/10);//上面那个数字
//    //        Flag_Show_102(MT9V03X_W,image_two_value,(uint8)((uint8)-Err%10));//下面的数字
//    //    }

//    //    ips200_showfloat(0,12,gz,3,3);
//    //    ips200_showuint16(0,11,Island_State);
//}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     左赛道连续性检测
  @param     起始点，终止点
  @return    连续返回0，不连续返回断线出行数
  Sample     Continuity_Change_Left(int start,int end);
  @note      连续性的阈值设置为5，可更改
-------------------------------------------------------------------------------------------------------------------*/
int Continuity_Change_Left(int start, int end) // 连续性阈值设置为5
{
    int i;
    int t;
    int continuity_change_flag = 0;
    if (Left_Lost_Time >= 0.9 * MT9V03X_H) // 大部分都丢线，没必要判断了
        return 1;
    if (Search_Stop_Line <= 5) // 搜所截止行很矮
        return 1;
    if (start >= MT9V03X_H - 1 - 5) // 数组越界保护
        start = MT9V03X_H - 1 - 5;
    if (end <= 5)
        end = 5;
    if (start < end) // 都是从下往上计算的，反了就互换一下
    {
        t = start;
        start = end;
        end = t;
    }

    for (i = start; i >= end; i--)
    {
        if (abs(Left_Line[i] - Left_Line[i - 1]) >= 3) // 连续判断阈值是3,可更改
        {
            continuity_change_flag = i;
            break;
        }
    }
    return continuity_change_flag;
}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     右赛道连续性检测
  @param     起始点，终止点
  @return    连续返回0，不连续返回断线出行数
  Sample     continuity_change_flag=Continuity_Change_Right(int start,int end)
  @note      连续性的阈值设置为5，可更改
-------------------------------------------------------------------------------------------------------------------*/
int Continuity_Change_Right(int start, int end)
{
    int i;
    int t;
    int continuity_change_flag = 0;
    if (Right_Lost_Time >= 0.9 * MT9V03X_H) // 大部分都丢线，没必要判断了
        return 1;
    if (start >= MT9V03X_H - 5) // 数组越界保护
        start = MT9V03X_H - 5;
    if (end <= 5)
        end = 5;
    if (start < end) // 都是从下往上计算的，反了就互换一下
    {
        t = start;
        start = end;
        end = t;
    }

    for (i = start; i >= end; i--)
    {
        if (abs(Right_Line[i] - Right_Line[i - 1]) >= 3) // 连续性阈值是3，可更改
        {
            continuity_change_flag = i;
            break;
        }
    }
    return continuity_change_flag;
}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     左下角点检测
  @param     起始点，终止点
  @return    返回角点所在的行数，找不到返回0
  Sample     Find_Left_Down_Point(int start,int end);
  @note      角点检测阈值可根据实际值更改
-------------------------------------------------------------------------------------------------------------------*/
int Find_Left_Down_Point(int start, int end) // 找四个角点，返回值是角点所在的行数
{
    int i, t;
    int left_down_line = 0;
    if (Left_Lost_Time >= 0.9 * MT9V03X_H) // 大部分都丢线，没有拐点判断的意义
        return left_down_line;
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
        if (left_down_line == 0 &&                       // 只找第一个符合条件的点
            abs(Left_Line[i] - Left_Line[i + 1]) <= 5 && // 角点的阈值可以更改
            abs(Left_Line[i + 1] - Left_Line[i + 2]) <= 5 &&
            abs(Left_Line[i + 2] - Left_Line[i + 3]) <= 5 &&
            (Left_Line[i] - Left_Line[i - 2]) >= 5 &&
            (Left_Line[i] - Left_Line[i - 3]) >= 10 &&
            (Left_Line[i] - Left_Line[i - 4]) >= 10)
        {
            left_down_line = i; // 获取行数即可
            break;
        }
    }
    return left_down_line;
}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     左上角点检测
  @param     起始点，终止点
  @return    返回角点所在的行数，找不到返回0
  Sample     Find_Left_Up_Point(int start,int end);
  @note      角点检测阈值可根据实际值更改
-------------------------------------------------------------------------------------------------------------------*/
int Find_Left_Up_Point(int start, int end) // 找四个角点，返回值是角点所在的行数
{
    int i, t;
    int left_up_line = 0;
    if (Left_Lost_Time >= 0.9 * MT9V03X_H) // 大部分都丢线，没有拐点判断的意义
        return left_up_line;
    if (start < end)
    {
        t = start;
        start = end;
        end = t;
    }
    if (end <= MT9V03X_H - Search_Stop_Line) // 搜索截止行往上的全都不判
        end = MT9V03X_H - Search_Stop_Line;
    if (end <= 5) // 及时最长白列非常长，也要舍弃部分点，防止数组越界
        end = 5;
    if (start >= MT9V03X_H - 1 - 5)
        start = MT9V03X_H - 1 - 5;
    for (i = start; i >= end; i--)
    {
        if (left_up_line == 0 && // 只找第一个符合条件的点
            abs(Left_Line[i] - Left_Line[i - 1]) <= 5 &&
            abs(Left_Line[i - 1] - Left_Line[i - 2]) <= 5 &&
            abs(Left_Line[i - 2] - Left_Line[i - 3]) <= 5 &&
            (Left_Line[i] - Left_Line[i + 2]) >= 8 &&
            (Left_Line[i] - Left_Line[i + 3]) >= 15 &&
            (Left_Line[i] - Left_Line[i + 4]) >= 15)
        {
            left_up_line = i; // 获取行数即可
            break;
        }
    }
    return left_up_line; // 如果是MT9V03X_H-1，说明没有这么个拐点
}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     右下角点检测
  @param     起始点，终止点
  @return    返回角点所在的行数，找不到返回0
  Sample     Find_Right_Down_Point(int start,int end);
  @note      角点检测阈值可根据实际值更改
-------------------------------------------------------------------------------------------------------------------*/
int Find_Right_Down_Point(int start, int end) // 找四个角点，返回值是角点所在的行数
{
    int i, t;
    int right_down_line = 0;
    if (Right_Lost_Time >= 0.9 * MT9V03X_H) // 大部分都丢线，没有拐点判断的意义
        return right_down_line;
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
        if (right_down_line == 0 &&                        // 只找第一个符合条件的点
            abs(Right_Line[i] - Right_Line[i + 1]) <= 5 && // 角点的阈值可以更改
            abs(Right_Line[i + 1] - Right_Line[i + 2]) <= 5 &&
            abs(Right_Line[i + 2] - Right_Line[i + 3]) <= 5 &&
            (Right_Line[i] - Right_Line[i - 2]) <= -5 &&
            (Right_Line[i] - Right_Line[i - 3]) <= -10 &&
            (Right_Line[i] - Right_Line[i - 4]) <= -10)
        {
            right_down_line = i; // 获取行数即可
            break;
        }
    }
    return right_down_line;
}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     右上角点检测
  @param     起始点，终止点
  @return    返回角点所在的行数，找不到返回0
  Sample     Find_Right_Up_Point(int start,int end);
  @note      角点检测阈值可根据实际值更改
-------------------------------------------------------------------------------------------------------------------*/
int Find_Right_Up_Point(int start, int end) // 找四个角点，返回值是角点所在的行数
{
    int i, t;
    int right_up_line = 0;
    if (Right_Lost_Time >= 0.9 * MT9V03X_H) // 大部分都丢线，没有拐点判断的意义
        return right_up_line;
    if (start < end)
    {
        t = start;
        start = end;
        end = t;
    }
    if (end <= MT9V03X_H - Search_Stop_Line) // 搜索截止行往上的全都不判
        end = MT9V03X_H - Search_Stop_Line;
    if (end <= 5) // 及时最长白列非常长，也要舍弃部分点，防止数组越界
        end = 5;
    if (start >= MT9V03X_H - 1 - 5)
        start = MT9V03X_H - 1 - 5;
    for (i = end; i >= start; i++)
    {
        if (right_up_line == 0 &&                          // 只找第一个符合条件的点
            abs(Right_Line[i] - Right_Line[i - 1]) <= 5 && // 下面两行位置差不多
            abs(Right_Line[i - 1] - Right_Line[i - 2]) <= 5 &&
            abs(Right_Line[i - 2] - Right_Line[i - 3]) <= 5 &&
            (Right_Line[i] - Right_Line[i + 2]) <= -8 &&
            (Right_Line[i] - Right_Line[i + 3]) <= -15 &&
            (Right_Line[i] - Right_Line[i + 4]) <= -15)
        {
            right_up_line = i; // 获取y坐标即可
            break;
        }
    }
    return right_up_line;
}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     单调性突变检测
  @param     起始点，终止行
  @return    点所在的行数，找不到返回0
  Sample     Find_Right_Up_Point(int start,int end);
  @note      前5后5它最大（最小），那他就是角点
-------------------------------------------------------------------------------------------------------------------*/
int Monotonicity_Change_Left(int start, int end) // 单调性改变，返回值是单调性改变点所在的行数
{
    int i;
    int monotonicity_change_line = 0;
    if (Left_Lost_Time >= 100) // 大部分都丢线，没有单调性判断的意义
        return monotonicity_change_line;
    if (start >= MT9V03X_H - 1 - 5) // 数组越界保护，在判断第i个点时
        start = MT9V03X_H - 1 - 5;  // 要访问它前后5个点，数组两头的点要不能作为起点终点
    if (end <= 5)
        end = 5;
    if (start <= end) // 递减计算，入口反了，直接返回0
        return monotonicity_change_line;
    for (i = start; i >= end; i--) // 会读取前5后5数据，所以前面对输入范围有要求
    {
        if (
            Left_Line[i] == Left_Line[i + 3] && Left_Line[i] == Left_Line[i - 3] &&
            Left_Line[i] == Left_Line[i + 2] && Left_Line[i] == Left_Line[i - 2] &&
            Left_Line[i] == Left_Line[i + 1] && Left_Line[i] == Left_Line[i - 1])
        { // 一堆数据一样，显然不能作为单调转折点
            continue;
        }
        else if (
            Left_Line[i] >= Left_Line[i + 3] && Left_Line[i] >= Left_Line[i - 3] &&
            Left_Line[i] >= Left_Line[i + 2] && Left_Line[i] >= Left_Line[i - 2] &&
            Left_Line[i] >= Left_Line[i + 1] && Left_Line[i] >= Left_Line[i - 1])
        { // 就很暴力，这个数据是在前5，后5中最大的（可以取等），那就是单调突变点
            monotonicity_change_line = i;
            break;
        }
    }
    return monotonicity_change_line;
}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     单调性突变检测
  @param     起始点，终止行
  @return    点所在的行数，找不到返回0
  Sample     Find_Right_Up_Point(int start,int end);
  @note      前5后5它最大（最小），那他就是角点
-------------------------------------------------------------------------------------------------------------------*/
int Monotonicity_Change_Right(int start, int end) // 单调性改变，返回值是单调性改变点所在的行数
{
    int i;
    int monotonicity_change_line = 0;

    if (Right_Lost_Time >= 100) // 大部分都丢线，没有单调性判断的意义
        return monotonicity_change_line;
    if (start >= MT9V03X_H - 1 - 5) // 数组越界保护
        start = MT9V03X_H - 1 - 5;
    if (end <= 5)
        end = 5;
    if (start <= end)
        return monotonicity_change_line;

    for (i = start; i >= end; i--) // 会读取前5后5数据，所以前面对输入范围有要求
    {
        if (
            Right_Line[i] == Right_Line[i + 3] && Right_Line[i] == Right_Line[i - 3] &&
            Right_Line[i] == Right_Line[i + 2] && Right_Line[i] == Right_Line[i - 2] &&
            Right_Line[i] == Right_Line[i + 1] && Right_Line[i] == Right_Line[i - 1])
        { // //如果当前点与前后3个点相等，继续
            continue;
        }
        else if (
            Right_Line[i] <= Right_Line[i + 3] && Right_Line[i] <= Right_Line[i - 3] &&
            Right_Line[i] <= Right_Line[i + 2] && Right_Line[i] <= Right_Line[i - 2] &&
            Right_Line[i] <= Right_Line[i + 1] && Right_Line[i] <= Right_Line[i - 1])
        { ////如果当前点大于前后3个点，认为是突变点
            monotonicity_change_line = i;
            break;
        }
    }
    return monotonicity_change_line;
}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     通过斜率，定点补线--
  @param     k       输入斜率
             startY  输入起始点纵坐标
             endY    结束点纵坐标
  @return    null
  Sample     K_Add_Boundry_Left(float k,int startY,int endY);
  @note      补得线直接贴在边线上
-------------------------------------------------------------------------------------------------------------------*/
void K_Add_Boundry_Left(float k, int startX, int startY, int endY)
{
    int i, t;
    if (startY >= MT9V03X_H - 1)
        startY = MT9V03X_H - 1;
    else if (startY <= 0)
        startY = 0;
    if (endY >= MT9V03X_H - 1)
        endY = MT9V03X_H - 1;
    else if (endY <= 0)
        endY = 0;
    if (startY < endY) //--操作，start需要大
    {
        t = startY;
        startY = endY;
        endY = t;
    }
    for (i = startY; i >= endY; i--)
    {
        Left_Line[i] = (int)((i - startY) / k + startX); //(y-y1)=k(x-x1)变形，x=(y-y1)/k+x1
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

/*-------------------------------------------------------------------------------------------------------------------
  @brief     通过斜率，定点补线
  @param     k       输入斜率
             startY  输入起始点纵坐标
             endY    结束点纵坐标
  @return    null    直接补边线
  Sample     K_Add_Boundry_Right(float k,int startY,int endY);
  @note      补得线直接贴在边线上
-------------------------------------------------------------------------------------------------------------------*/
void K_Add_Boundry_Right(float k, int startX, int startY, int endY)
{
    int i, t;
    if (startY >= MT9V03X_H - 1)
        startY = MT9V03X_H - 1;
    else if (startY <= 0)
        startY = 0;
    if (endY >= MT9V03X_H - 1)
        endY = MT9V03X_H - 1;
    else if (endY <= 0)
        endY = 0;
    if (startY < endY)
    {
        t = startY;
        startY = endY;
        endY = t;
    }
    for (i = startY; i >= endY; i--)
    {
        Right_Line[i] = (int)((i - startY) / k + startX); //(y-y1)=k(x-x1)变形，x=(y-y1)/k+x1
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

/*-------------------------------------------------------------------------------------------------------------------
  @brief     根据斜率划线
  @param     输入斜率，定点，画一条黑线
  @return    null
  Sample     K_Draw_Line(k, 20,MT9V03X_H-1 ,0)
  @note      补的就是一条线，需要重新扫线
-------------------------------------------------------------------------------------------------------------------*/
void K_Draw_Line(float k, int startX, int startY, int endY)
{
    int endX = 0;

    if (startX >= MT9V03X_W - 1) // 限幅处理
        startX = MT9V03X_W - 1;
    else if (startX <= 0)
        startX = 0;
    if (startY >= MT9V03X_H - 1)
        startY = MT9V03X_H - 1;
    else if (startY <= 0)
        startY = 0;
    if (endY >= MT9V03X_H - 1)
        endY = MT9V03X_H - 1;
    else if (endY <= 0)
        endY = 0;
    endX = (int)((endY - startY) / k + startX); //(y-y1)=k(x-x1)变形，x=(y-y1)/k+x1
    Draw_Line(startX, startY, endX, endY);
}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     获取平均赛宽
  @param     int start_line,int end_line，起始行，中止行
  @return    这几行赛宽平均值
  Sample     road_wide=Get_Road_Wide(68,69);
  @note      ++运算，向下寻找，算出平均赛宽
-------------------------------------------------------------------------------------------------------------------*/
int Get_Road_Wide(int start_line, int end_line)
{
    if (start_line >= MT9V03X_H - 1)
        start_line = MT9V03X_H - 1;
    else if (start_line <= 0)
        start_line = 0;
    if (end_line >= MT9V03X_H - 1)
        end_line = MT9V03X_H - 1;
    else if (end_line <= 0)
        end_line = 0;
    int i = 0, t = 0;
    int road_wide = 0;
    if (start_line > end_line) //++访问，坐标反了互换
    {
        t = start_line;
        start_line = end_line;
        end_line = t;
    }
    for (i = start_line; i <= end_line; i++)
    {
        road_wide += Right_Line[i] - Left_Line[i];
    }
    road_wide = road_wide / (end_line - start_line + 1); // 平均赛宽
    return road_wide;
}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     获取左赛道边界斜率
  @param     int start_line,int end_line，起始行，中止行
  @return    两点之间的斜率
  Sample     k=Get_Left_K(68,69);
  @note      两点之间得出斜率，默认第一个参数小，第二个参数大
-------------------------------------------------------------------------------------------------------------------*/
float Get_Left_K(int start_line, int end_line)
{
    if (start_line >= MT9V03X_H - 1)
        start_line = MT9V03X_H - 1;
    else if (start_line <= 0)
        start_line = 0;
    if (end_line >= MT9V03X_H - 1)
        end_line = MT9V03X_H - 1;
    else if (end_line <= 0)
        end_line = 0;
    float k = 0;
    int t = 0;
    if (start_line > end_line) //++访问，坐标反了互换
    {
        t = start_line;
        start_line = end_line;
        end_line = t;
    }
    k = (float)(((float)Left_Line[start_line] - (float)Left_Line[end_line]) / (end_line - start_line + 1));
    return k;
}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     获取右赛道边界斜率
  @param     int start_line,int end_line，起始行，中止行
  @return    两点之间的斜率
  Sample     k=Get_Right_K(68,69);
  @note      两点之间得出斜率，默认第一个参数小，第二个参数大
-------------------------------------------------------------------------------------------------------------------*/
float Get_Right_K(int start_line, int end_line)
{
    if (start_line >= MT9V03X_H - 1)
        start_line = MT9V03X_H - 1;
    else if (start_line <= 0)
        start_line = 0;
    if (end_line >= MT9V03X_H - 1)
        end_line = MT9V03X_H - 1;
    else if (end_line <= 0)
        end_line = 0;
    float k = 0;
    int t = 0;
    if (start_line > end_line) //++访问，坐标反了互换
    {
        t = start_line;
        start_line = end_line;
        end_line = t;
    }
    k = (float)(((float)Right_Line[start_line] - (float)Right_Line[end_line]) / (end_line - start_line + 1));
    return k;
}

/*==============================================================================================================
******************************************************************************************************************
                                            这是分割行
**********************************************************************************************************************
==================================================================================================================*/

uint8 left_down_point;             // 左下拐点
uint8 left_up_point;               // 左上拐点
uint8 right_down_point;            // 右下拐点
uint8 right_up_point;              // 右上拐点
uint8 circle_flag;                 // 环岛标志位
uint8 right_circle_flag;           // 右环岛标志
uint8 circle_once_time = 0;        // 环岛标志位单次触发
const uint8 road_wide[MT9V03X_H] = // 每行道宽数据
    {
        41, 42, 43, 45, 46, 47, 49, 49, 51, 53,
        53, 55, 55, 57, 58, 59, 61, 62, 63, 64,
        65, 67, 68, 69, 70, 72, 73, 74, 76, 76,
        78, 79, 80, 82, 82, 84, 86, 86, 88, 88,
        90, 91, 92, 94, 95, 96, 97, 98, 100, 100,
        102, 103, 105, 105, 107, 108, 109, 111, 112, 113,
        114, 116, 117, 118, 119, 120, 122, 123, 124, 126,
        126, 128, 129, 130, 132, 132, 134, 134, 136, 138,
        138, 140, 140, 142, 144, 144, 146, 146, 148, 149,
        150, 151, 152, 154, 155, 156, 157, 158, 159, 161,
        162, 163, 164, 165, 166, 167, 169, 170, 171, 172,
        173, 175, 175, 177, 177, 179, 180, 181, 184, 184};

/**
 *
 * @brief  左边界延长
 * @param  start_point 延长起点
 * @param  end_point   延长终点
 **/
void lenthen_Left_Line(uint8 start_point, uint8 end_point)
{
    float k;
    // 防止越界
    if (start_point >= MT9V03X_H - 1)
        start_point = MT9V03X_H - 1;
    if (start_point < 0)
        start_point = 0;
    if (end_point >= MT9V03X_H - 1)
        end_point = MT9V03X_H - 1;
    if (end_point < 0)
        end_point = 0;

    if (end_point < start_point)
    {
        uint8 t = start_point;
        start_point = end_point;
        end_point = t;
    }

    if (start_point <= 5) // 起点过于靠上，直接连线
    {
        Left_Add_Line(Left_Line[start_point], start_point, Left_Line[end_point], end_point);
    }
    else
    {
        k = (float)(Left_Line[start_point] - Left_Line[start_point - 4]) / 5.0; // 斜率
        for (uint8 i = start_point; i <= end_point; i++)
        {
            Left_Line[i] = Left_Line[start_point] + (int)(i - start_point) * k; // 使用斜率延长

            if (Left_Line[i] < 1) // 防止越界
            {
                Left_Line[i] = 1;
            }

            if (Left_Line[i] >= MT9V03X_W - 2) // 防止越界
            {
                Left_Line[i] = MT9V03X_W - 2;
            }
        }
    }
}

/**
 * @brief  左边界延长（从下往上）
 * @param  start_point 延长起点（下方点）
 * @param  end_point   延长终点（上方点）
 **/
void lenthen_Left_Line_up(uint8 start_point, uint8 end_point)
{
    float k;
    // 防止越界
    if (start_point >= MT9V03X_H - 1)
        start_point = MT9V03X_H - 1;
    if (start_point < 0)
        start_point = 0;
    if (end_point >= MT9V03X_H - 1)
        end_point = MT9V03X_H - 1;
    if (end_point < 0)
        end_point = 0;

    // 确保起点大于终点
    if (start_point < end_point)
    {
        uint8 t = start_point;
        start_point = end_point;
        end_point = t;
    }

    // 如果起点过于靠下，直接连线
    if (start_point >= MT9V03X_H - 6)
    {
        Left_Add_Line(Left_Line[start_point], start_point, Left_Line[end_point], end_point);
    }
    else
    {
        // 计算斜率
        k = (float)(Left_Line[start_point] - Left_Line[start_point + 4]) / 5.0;

        // 从起点向上延长
        for (int16_t i = start_point; i >= end_point; i--)
        {
            Left_Line[i] = Left_Line[start_point] + (int)((i - start_point) * (-k)); // 使用斜率延长(负斜率)

            // 防止越界
            if (Left_Line[i] < 1)
            {
                Left_Line[i] = 1;
            }
            if (Left_Line[i] >= MT9V03X_W - 2)
            {
                Left_Line[i] = MT9V03X_W - 2;
            }
        }
    }
}

/**
 *
 * @brief  右边界延长
 * @param  start_point 延长起点
 * @param  end_point   延长终点
 **/
void lenthen_Right_Line(uint8 start_point, uint8 end_point)
{
    float k;
    // 防止越界
    if (start_point >= MT9V03X_H - 1)
        start_point = MT9V03X_H - 1;
    if (start_point < 0)
        start_point = 0;
    if (end_point >= MT9V03X_H - 1)
        end_point = MT9V03X_H - 1;
    if (end_point < 0)
        end_point = 0;

    if (end_point < start_point)
    {
        uint8 t = start_point;
        start_point = end_point;
        end_point = t;
    }

    if (start_point <= 5) // 起点过于靠上，直接连线
    {
        Left_Add_Line(Right_Line[start_point], start_point, Right_Line[end_point], end_point);
    }
    else
    {
        k = (float)(Right_Line[start_point] - Right_Line[start_point - 4]) / 5.0; // 斜率
        for (uint8 i = start_point; i <= end_point; i++)
        {
            Right_Line[i] = Right_Line[start_point] + (int)(i - start_point) * k; // 使用斜率延长

            if (Right_Line[i] < 1) // 防止越界
            {
                Right_Line[i] = 1;
            }

            if (Right_Line[i] >= MT9V03X_W - 2) // 防止越界
            {
                Right_Line[i] = MT9V03X_W - 2;
            }
        }
    }
}

/**
 *
 * @brief  道宽半边补左线
 **/
void road_wide_draw_Left_Line(void)
{
    for (int i = 0; i < MT9V03X_H - 1; i++)
    {
        Left_Line[i] = Right_Line[i] - road_wide[i];
        if (Left_Line[i] < 1) // 防止越界
        {
            Left_Line[i] = 1;
        }
    }
}

/**
 *
 * @brief  道宽半边补右线
 **/
void road_wide_draw_Right_Line(void)
{
    for (int i = 0; i < MT9V03X_H - 1; i++)
    {
        Right_Line[i] = Left_Line[i] + road_wide[i];
        if (Right_Line[i] >= MT9V03X_W - 2) // 防止越界
        {
            Right_Line[i] = MT9V03X_W - 2;
        }
    }
}

/**
 *
 * @brief  找下拐点
 * @param  start_point 搜索起点
 * @param  end_point    搜索终点
 **/

void find_down_point(uint8 start_point, uint8 end_point)
{
    // 参数清零
    left_down_point = 0;
    right_down_point = 0;
    if (start_point < end_point)
    {
        uint8 t = start_point;
        start_point = end_point;
        end_point = t;
    }
    if (start_point > MT9V03X_H - 5 - 1)
    {
        start_point = MT9V03X_H - 5 - 1;
    }
    if (end_point < MT9V03X_H - Search_Stop_Line)
    {
        end_point = MT9V03X_H - Search_Stop_Line;
    }
    if (end_point < 5)
    {
        end_point = 5;
    }
    for (int i = start_point; i >= end_point; i--)
    {
        // 点i下面2个连续相差不大并且点i与上面边3个点分别相差很大，认为有下左拐点
        if (left_down_point == 0 &&
            abs(Left_Line[i] - Left_Line[i + 1]) <= 7 &&
            abs(Left_Line[i + 1] - Left_Line[i + 2]) <= 7 &&
            abs(Left_Line[i + 2] - Left_Line[i + 3]) <= 7 &&
            (Left_Line[i] - Left_Line[i - 2]) >= 8 &&
            (Left_Line[i] - Left_Line[i - 3]) >= 8 &&
            (Left_Line[i] - Left_Line[i - 4]) >= 8)
        {
            left_down_point = i + 3;
        }
        if (right_down_point == 0 &&
            abs(Right_Line[i] - Right_Line[i + 1]) <= 6 &&
            abs(Right_Line[i + 1] - Right_Line[i + 2]) <= 6 &&
            abs(Right_Line[i + 2] - Right_Line[i + 3]) <= 6 &&
            (Right_Line[i] - Right_Line[i - 2]) <= -8 &&
            (Right_Line[i] - Right_Line[i - 3]) <= -8 &&
            (Right_Line[i] - Right_Line[i - 4]) <= -8)
        {
            right_down_point = i + 3;
        }
        if (left_down_point != 0 && right_down_point != 0)
        {
            break;
        }
    }
}

/**
 *
 * @brief  找上拐点
 * @param  start_point 搜索起点
 * @param  end_point    搜索终点
 **/
void find_up_point(uint8 start_point, uint8 end_point)
{
    left_up_point = 0;
    right_up_point = 0;
    if (start_point < end_point)
    {
        uint8 t = start_point;
        start_point = end_point;
        end_point = t;
    }

    if (start_point > MT9V03X_H - 5 - 1)
    {
        start_point = MT9V03X_H - 5 - 1;
    }

    if (end_point < MT9V03X_H - Search_Stop_Line)
    {
        end_point = MT9V03X_H - Search_Stop_Line;
    }

    if (end_point < 5)
    {
        end_point = 5;
    }

    for (int i = start_point; i >= end_point; i--)
    {
        // 点i下面2个连续相差不大并且点i与上面边3个点分别相差很大，认为有上左拐点
        if (left_up_point == 0 &&
            abs(Left_Line[i] - Left_Line[i - 1]) <= 5 &&
            abs(Left_Line[i - 1] - Left_Line[i - 2]) <= 5 &&
            abs(Left_Line[i - 2] - Left_Line[i - 3]) <= 5 &&
            (Left_Line[i] - Left_Line[i + 2]) >= 8 &&
            (Left_Line[i] - Left_Line[i + 3]) >= 15 &&
            (Left_Line[i] - Left_Line[i + 4]) >= 15)
        {
            left_up_point = i - 3;
        }
        if (right_up_point == 0 &&
            abs(Right_Line[i] - Right_Line[i - 1]) <= 3 &&
            abs(Right_Line[i - 1] - Right_Line[i - 2]) <= 3 &&
            abs(Right_Line[i - 2] - Right_Line[i - 3]) <= 3 &&
            (Right_Line[i] - Right_Line[i + 2]) <= -8 &&
            (Right_Line[i] - Right_Line[i + 3]) <= -15 &&
            (Right_Line[i] - Right_Line[i + 4]) <= -15)
        {
            right_up_point = i - 3;
        }
        if (left_up_point != 0 && right_up_point != 0)
        {
            break;
        }
    }
}

/**
 *
 * @brief  找圆环右上拐点
 * @param  start_point 搜索起点
 * @param  end_point    搜索终点
 **/
void circle_right_up_point(uint8 start_point, uint8 end_point)
{
    right_up_point = 0;
    if (start_point < end_point)
    {
        uint8 t = start_point;
        start_point = end_point;
        end_point = t;
    }

    if (start_point > MT9V03X_H - 5 - 1)
    {
        start_point = MT9V03X_H - 5 - 1;
    }

    if (end_point < 5)
    {
        end_point = 5;
    }

    for (int i = start_point; i >= end_point; i--)
    {
        if (right_up_point == 0 &&
            abs(Right_Line[i] - Right_Line[i - 1]) <= 5 &&
            abs(Right_Line[i - 1] - Right_Line[i - 2]) <= 5 &&
            abs(Right_Line[i - 2] - Right_Line[i - 3]) <= 5 &&
            (Right_Line[i] - Right_Line[i + 2]) <= -6 &&
            (Right_Line[i] - Right_Line[i + 3]) <= -8 &&
            (Right_Line[i] - Right_Line[i + 4]) <= -8)
        {
            right_up_point = i - 3;
        }
        if (right_up_point != 0)
        {
            break;
        }
    }
}

/**
 * @brief  找圆环右下拐点
 * @param  start_point 搜索起点
 * @param  end_point    搜索终点
 **/
void circle_right_down_point(uint8 start_point, uint8 end_point)
{
    // 参数清零
    right_down_point = 0;
    if (start_point < end_point)
    {
        uint8 t = start_point;
        start_point = end_point;
        end_point = t;
    }
    if (start_point > MT9V03X_H - 5 - 1)
    {
        start_point = MT9V03X_H - 5 - 1;
    }
    if (end_point < 5)
    {
        end_point = 5;
    }
    for (int i = start_point; i >= end_point; i--)
    {
        if (right_down_point == 0 &&
            abs(Right_Line[i] - Right_Line[i + 1]) <= 7 &&
            abs(Right_Line[i + 1] - Right_Line[i + 2]) <= 7 &&
            abs(Right_Line[i + 2] - Right_Line[i + 3]) <= 7 &&
            (Right_Line[i] - Right_Line[i - 2]) <= -5 &&
            (Right_Line[i] - Right_Line[i - 3]) <= -6 &&
            (Right_Line[i] - Right_Line[i - 4]) <= -6)
        {
            right_down_point = i + 3;
        }
        if (right_down_point != 0)
        {
            break;
        }
    }
}

/**
 * @brief  找圆环左下拐点
 * @param  start_point 搜索起点
 * @param  end_point    搜索终点
 **/
void circle_left_down_point(uint8 start_point, uint8 end_point)
{
    // 参数清零
    left_down_point = 0;
    if (start_point < end_point)
    {
        uint8 t = start_point;
        start_point = end_point;
        end_point = t;
    }
    if (start_point > MT9V03X_H - 5 - 1)
    {
        start_point = MT9V03X_H - 5 - 1;
    }
    if (end_point < 5)
    {
        end_point = 5;
    }
    for (int i = start_point; i >= end_point; i--)
    {
        if (right_down_point == 0 &&
            abs(Right_Line[i] - Right_Line[i + 1]) <= 7 &&
            abs(Right_Line[i + 1] - Right_Line[i + 2]) <= 7 &&
            abs(Right_Line[i + 2] - Right_Line[i + 3]) <= 7 &&
            (Right_Line[i] - Right_Line[i - 2]) >= 8 &&
            (Right_Line[i] - Right_Line[i - 3]) >= 10 &&
            (Right_Line[i] - Right_Line[i - 4]) >= 10)
        {
            left_down_point = i + 3;
        }
        if (left_down_point != 0)
        {
            break;
        }
    }
}

/**
 *
 * @brief  判断左边界连续性
 * @retval 连续返回0，不连续返回断裂点
 **/
uint8 left_countinuity_detect(uint8 start_point, uint8 end_point)
{
    uint8 continuity_line = 0; // 连续标志

    if (start_point < end_point) // 从下往上扫
    {
        uint8 t = start_point;
        start_point = end_point;
        end_point = t;
    }

    if (Left_Lost_Time > MT9V03X_H * 0.95) // 如果左边丢线超过95%，直接返回1
    {
        return 1;
    }

    if (Search_Stop_Line <= 5) // 如果搜索截止行小于5，直接返回1
    {
        return 1;
    }

    if (start_point > MT9V03X_H - 5) // 防止起点越界
    {
        start_point = MT9V03X_H - 5;
    }

    if (end_point < 5) // 防止终点越界
    {
        end_point = 5;
    }

    for (uint8 i = start_point; i > end_point; i--)
    {
        if (abs(Left_Line[i] - Left_Line[i - 1]) >= 3)
        {
            continuity_line = i; // 如果当前点与前一个点相差大于4，认为不连续
            break;
        }
    }
    return continuity_line; // 返回断裂点坐标，如果返回0，表示连续
}

/**
 *
 * @brief  判断右边界连续性
 * @retval 连续返回0，不连续返回断裂点
 **/
uint8 right_countinuity_detect(uint8 start_point, uint8 end_point)
{
    uint8 continuity_line = 0; // 连续标志

    if (start_point < end_point) // 从下往上扫
    {
        uint8 t = start_point;
        start_point = end_point;
        end_point = t;
    }

    if (Right_Lost_Time > MT9V03X_H * 0.95) // 如果右边丢线超过95%，直接返回1
    {
        return 1;
    }

    if (start_point > MT9V03X_H - 5) // 防止起点越界
    {
        start_point = MT9V03X_H - 5;
    }

    if (end_point < 5) // 防止终点越界
    {
        end_point = 5;
    }

    for (uint8 i = start_point; i > end_point; i--)
    {
        if (abs(Right_Line[i] - Right_Line[i - 1]) >= 3)
        {
            continuity_line = i; // 如果当前点与前一个点相差大于4，认为不连续
            break;
        }
    }
    return continuity_line; // 返回断裂点坐标，如果返回0，表示连续
}

/**
 *
 * @brief  找到左边单调性突变点
 * @retval 拐点坐标
 **/
uint8 find_left_change(uint8 start_point, uint8 end_point)
{
    uint8 left_change_line = 0; // 右边突变标志

    if (start_point < end_point) // 从下往上扫
    {
        return left_change_line; // 如果起点小于终点，直接返回0
    }

    if (start_point >= MT9V03X_H - 5) // 防止起点越界
    {
        start_point = MT9V03X_H - 5;
    }

    if (end_point < 5) // 防止终点越界
    {
        end_point = 5;
    }

    if (Right_Lost_Time > MT9V03X_H * 0.95)
    {
        return left_change_line; // 如果右边丢线超过95%，直接返回0
    }

    for (uint8 i = start_point; i > end_point; i--)
    {
        if (abs(Left_Line[i] - Left_Line[i - 5]) <= 10 && abs(Left_Line[i] - Left_Line[i + 5]) <= 12) // 如果当前点与前后5个点相差小于10
        {
            if (Left_Line[i] == Left_Line[i - 5] && Left_Line[i] == Left_Line[i + 5] &&
                Left_Line[i] == Left_Line[i - 4] && Left_Line[i] == Left_Line[i + 4] &&
                Left_Line[i] == Left_Line[i - 3] && Left_Line[i] == Left_Line[i + 3] &&
                Left_Line[i] == Left_Line[i - 2] && Left_Line[i] == Left_Line[i + 2] &&
                Left_Line[i] == Left_Line[i - 1] && Left_Line[i] == Left_Line[i + 1])
            {
                continue; // 如果当前点与前后5个点相等，继续
            }
            else if (Left_Line[i] >= Left_Line[i - 5] && Left_Line[i] >= Left_Line[i + 5] &&
                     Left_Line[i] >= Left_Line[i - 4] && Left_Line[i] >= Left_Line[i + 4] &&
                     Left_Line[i] >= Left_Line[i - 3] && Left_Line[i] >= Left_Line[i + 3] &&
                     Left_Line[i] >= Left_Line[i - 2] && Left_Line[i] >= Left_Line[i + 2] &&
                     Left_Line[i] >= Left_Line[i - 1] && Left_Line[i] >= Left_Line[i + 1])
            {
                left_change_line = i; // 如果当前点大于前后5个点，认为是突变点
                break;
            }
        }
    }
    return left_change_line; // 返回突变点坐标
}

/**
 *
 * @brief  找到右边单调性突变点
 * @retval 拐点坐标
 **/
uint8 find_right_change(uint8 start_point, uint8 end_point)
{
    uint8 right_change_line = 0; // 右边突变标志

    if (start_point < end_point) // 从下往上扫
    {
        return right_change_line; // 如果起点小于终点，直接返回0
    }

    if (start_point >= MT9V03X_H - 5) // 防止起点越界
    {
        start_point = MT9V03X_H - 5;
    }

    if (end_point < 5) // 防止终点越界
    {
        end_point = 5;
    }

    if (Right_Lost_Time > MT9V03X_H * 0.95)
    {
        return right_change_line; // 如果右边丢线超过95%，直接返回0
    }

    for (uint8 i = start_point; i > end_point; i--)
    {
        if (abs(Right_Line[i] - Right_Line[i - 5]) <= 10 && abs(Right_Line[i] - Right_Line[i + 5]) <= 14) // 如果当前点与前后5个点相差小于10
        {
            if (Right_Line[i] == Right_Line[i - 5] && Right_Line[i] == Right_Line[i + 5] &&
                Right_Line[i] == Right_Line[i - 4] && Right_Line[i] == Right_Line[i + 4] &&
                Right_Line[i] == Right_Line[i - 3] && Right_Line[i] == Right_Line[i + 3] &&
                Right_Line[i] == Right_Line[i - 2] && Right_Line[i] == Right_Line[i + 2] &&
                Right_Line[i] == Right_Line[i - 1] && Right_Line[i] == Right_Line[i + 1])
            {
                continue; // 如果当前点与前后5个点相等，继续
            }
            else if (Right_Line[i] <= Right_Line[i - 5] && Right_Line[i] <= Right_Line[i + 5] &&
                     Right_Line[i] <= Right_Line[i - 4] && Right_Line[i] <= Right_Line[i + 4] &&
                     Right_Line[i] <= Right_Line[i - 3] && Right_Line[i] <= Right_Line[i + 3] &&
                     Right_Line[i] <= Right_Line[i - 2] && Right_Line[i] <= Right_Line[i + 2] &&
                     Right_Line[i] <= Right_Line[i - 1] && Right_Line[i] <= Right_Line[i + 1])
            {
                right_change_line = i; // 如果当前点大于前后5个点，认为是突变点
                break;
            }
        }
    }

    return right_change_line; // 返回突变点坐标
}

// 右环岛处理中间变量
uint8 continuity_left_change_flag = 0;  // 左边连续变化标志
uint8 continuity_right_change_flag = 0; // 右边连续变化标志
uint8 left_change_line = 0;             // 左边突变点
uint8 right_change_line = 0;            // 右边突变点
int16 encoder_sum;                      // 圆环状态编码器计数

/**
 *
 * @brief  判断环岛状态并补线
 **/
void circle_judge(void)
{

    continuity_left_change_flag = 0;  // 左边连续变化标志
    continuity_right_change_flag = 0; // 右边连续变化标志
    left_change_line = 0;             // 左边突变点
    right_change_line = 0;            // 右边突变点

    if (Cross_Flag == 0) // 避开十字
    {
        continuity_left_change_flag = left_countinuity_detect(MT9V03X_H - 1 - 60, 15);   // 判断左边连续性
        continuity_right_change_flag = right_countinuity_detect(MT9V03X_H - 1 - 15, 15); // 判断右边连续性
        left_change_line = find_left_change(MT9V03X_H - 1 - 40, 15);                     // 寻找左边突变点
        right_change_line = find_right_change(MT9V03X_H - 1 - 5, 5);                     // 寻找右边突变点

        find_down_point(MT9V03X_H - 20, 60);        // 寻找下拐点
        circle_right_down_point(MT9V03X_H - 5, 80); // 寻找右下拐点
        find_up_point(MT9V03X_H - 5, 10);           // 寻找上拐点

        if (circle_flag == 0)
        {
            if (continuity_left_change_flag != 0 &&
                continuity_right_change_flag <= 40 &&
                Left_Lost_Time >= 10 && Left_Lost_Time <= 100 &&
                Right_Lost_Time <= 10 &&
                Boundry_Start_Left >= MT9V03X_H - 8 &&
                Boundry_Start_Right >= MT9V03X_H - 8 &&
                Search_Stop_Line >= 100 )
            {
            }
            if (continuity_left_change_flag <= 40 &&
                continuity_right_change_flag != 0 &&
                Right_Lost_Time >= 10 && Right_Lost_Time <= 100 &&
                Left_Lost_Time <= 10 &&
                Boundry_Start_Left >= MT9V03X_H - 8 &&
                Boundry_Start_Right >= MT9V03X_H - 8 &&
                Search_Stop_Line >= 100 &&
                right_down_point)
            {
                if (car_run && circle_flag == 0 && right_circle_flag == 0) // 如果车在行驶中
                {
                    buzzer_on(50); // 蜂鸣器响
                }

                right_circle_flag = 1; // 右圆环标志置1
                circle_flag = 1;       // 环岛标志置1

//                turn_start += 5;
//                turn_end += 5;
            }
            else
            {
                right_circle_flag = 0; // 右圆环标志置0
                circle_flag = 0;       // 环岛标志置0
            }
        }

        else if (circle_flag == 1)
        {
            if (right_circle_flag == 1) // 处理右圆环
            {
                road_wide_draw_Right_Line();                  // 右边道宽补线
                if (right_change_line > 55 && right_up_point) // 右边突点坐标过大并且有右上拐点
                {
                    right_circle_flag = 2; // 右圆环标志置2
                    if (car_run)
                    {
                        buzzer_on(50); // 蜂鸣器响
                    }
                }
            }
            else if (right_circle_flag == 2)
            {

                circle_right_up_point(MT9V03X_H - 10, 10); // 寻找右上拐点
                if (right_up_point)
                {
                    Left_Add_Line(Right_Line[right_up_point], right_up_point, Left_Line[Boundry_Start_Left], Boundry_Start_Left); // 左边补线
                }
                else
                {
                    road_wide_draw_Left_Line(); // 左边道宽补线
                }

                // road_wide_draw_Left_Line(); // 左边道宽补线

                if (encoder_sum >= 16000)
                {
                    right_circle_flag = 3;
//                    turn_start -= 5;
//                    turn_end -= 5;
                    if (car_run)
                    {
                        buzzer_on(50); // 蜂鸣器响
                    }
                    encoder_sum = 0; // 编码器积分清零
                }
            }
            else if (right_circle_flag == 3)
            {
                if (left_down_point >= 3)
                {
                    right_circle_flag = 4; // 右圆环标志置4
                    //                    if(car_run)
                    //                    {
                    //                        buzzer_on(50);//蜂鸣器响
                    //                    }
                }
            }
            else if (right_circle_flag == 4)
            {
                find_down_point(MT9V03X_H - 10, 10); // 寻找下拐点
                lenthen_Left_Line_up(left_down_point, 0);
                if (left_down_point > 50)
                {
                    right_circle_flag = 5; // 右圆环标志置0
                    //                    if(car_run)
                    //                    {
                    //                        buzzer_on(50);//蜂鸣器响
                    //                    }
                }
            }
            else if (right_circle_flag == 5)
            {
                if (right_up_point)
                {
                    right_circle_flag = 6;
                    //                    if(car_run)
                    //                    {
                    //                        buzzer_on(50);//蜂鸣器响
                    //                    }
                }
                else
                {
                    road_wide_draw_Left_Line(); // 左边道宽补线
                }
            }
            else if (right_circle_flag == 6)
            {
                find_up_point(MT9V03X_H - 5, 5); // 寻找上拐点
                if (right_up_point)
                {
                    lenthen_Right_Line(right_up_point, MT9V03X_H - 1); // 右边延长
                }
                else
                {
                    road_wide_draw_Right_Line(); // 右边道宽补线
                }
                if (right_up_point == 0)
                {
                    circle_flag = 0;       // 环岛标志置0
                    right_circle_flag = 0; // 右圆环标志置0
                    encoder_sum = 0;       // 编码器积分清零
                    circle_once_time = 1;  // 环岛一次标志置1
                    //                    if(car_run)
                    //                    {
                    //                        buzzer_on(50);//蜂鸣器响
                    //                    }
                }
            }
        }
    }
}
