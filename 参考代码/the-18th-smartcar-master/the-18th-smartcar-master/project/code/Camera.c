#include "zf_common_headfile.h"
#include "Camera.h"
#include  <math.h>

//常用基本变量
extern const uint8 Image_Flags[][9][8];     //放在图上的数字标记
uint8 image_two_value[MT9V03X_H][MT9V03X_W];//二值化后的原数组
volatile int Left_Line[MT9V03X_H]; //左边线数组
volatile int Right_Line[MT9V03X_H];//右边线数组
volatile int Mid_Line[MT9V03X_H];  //中线数组
volatile int Road_Wide[MT9V03X_H]; //赛宽数组
volatile int White_Column[MT9V03X_W];    //每列白列长度
volatile int Search_Stop_Line;     //搜索截止行,只记录长度，想要坐标需要用视野高度减去该值
volatile int Boundry_Start_Left;   //左右边界起始点
volatile int Boundry_Start_Right;  //第一个非丢线点,常规边界起始点
volatile int Left_Lost_Time;       //边界丢线数
volatile int Right_Lost_Time;
volatile int Both_Lost_Time;//两边同时丢线数
int Longest_White_Column_Left[2]; //最长白列,[0]是最长白列的长度，也就是Search_Stop_Line搜索截止行，[1】是第某列
int Longest_White_Column_Right[2];//最长白列,[0]是最长白列的长度，也就是Search_Stop_Line搜索截止行，[1】是第某列
int Left_Lost_Flag[MT9V03X_H] ; //左丢线数组，丢线置1，没丢线置0
int Right_Lost_Flag[MT9V03X_H]; //右丢线数组，丢线置1，没丢线置0

//---元素类
//电磁，摄像头标志位，默认摄像头
extern volatile uint8 Electromagnet_Flag;

//环岛
extern volatile int Island_State;     //环岛状态标志
extern volatile int Left_Island_Flag; //左右环岛标志
extern volatile int Right_Island_Flag;//左右环岛标志

//坡道
volatile int Ramp_Flag=0;//坡道标志

//十字
volatile int Cross_Flag=0;
volatile int Left_Down_Find=0; //十字使用，找到被置行数，没找到就是0
volatile int Left_Up_Find=0;   //四个拐点标志
volatile int Right_Down_Find=0;
volatile int Right_Up_Find=0;

//斑马线
volatile int Zebra_Stripes_Flag=0;//斑马线

//控制标
volatile uint8 Obstacle_Dir=0; //0右拐，1左拐
volatile uint8 Island_Switch=1;//环岛识别开启标志位
volatile uint8 Straight_Flag=0;//长直道识别标
volatile uint8 Ramp_Switch=0;  //坡道识别标志位

//舵机
extern volatile float Err;//摄像头误差

//图像消失
volatile uint8 Img_Disappear_Flag=0; //图像消失标志位，1就说明丢图了
volatile uint8 Outside_Edge_flag=0;  //和上面的是一个意思，后续改代码就没删掉他，其实用不到他了
volatile uint8 Stop_Flag=0;          //停止标

//横断路障标志位
volatile uint8 Barricade_Flag=0;     //横断，有1~4几个状态，0说明不在横断里

//电磁时候保护
extern volatile uint8 Electromagnet_Flag;//0是摄像头，1是电磁

//标准赛宽，将车子放在长直道上面实测，以下数值仅供参考
const uint8 Standard_Road_Wide[MT9V03X_H]=
{ 10, 12, 14, 16, 18, 20, 22, 24, 26, 28,
  30, 32, 34, 36, 38, 40, 42, 44, 46, 48,
  50, 52, 54, 56, 58, 60, 62, 64, 66, 68,
  70, 72, 74, 76, 78, 80, 82, 84, 86, 88,
  90, 92, 94, 96, 98,100,102,104,106,108,
 110,112,114,116,118,120,122,124,126,128,
 130,132,134,136,138,140,142,144,146,148};

/*-------------------------------------------------------------------------------------------------------------------
  @brief     快速大津求阈值，来自山威
  @param     image       图像数组
             col         列 ，宽度
             row         行，长度
  @return    null
  Sample     threshold=my_adapt_threshold(mt9v03x_image[0],MT9V03X_W, MT9V03X_H);//山威快速大津
  @note      据说比传统大津法快一点，使用效果差不多
-------------------------------------------------------------------------------------------------------------------*/
int my_adapt_threshold(uint8 *image, uint16 col, uint16 row)   //注意计算阈值的一定要是原图像
{
    #define GrayScale 256
    uint16 width = col;
    uint16 height = row;
    int pixelCount[GrayScale];
    float pixelPro[GrayScale];
    int i, j;
    int pixelSum = width * height/4;
    int threshold = 0;
    uint8* data = image;  //指向像素数据的指针
    for (i = 0; i < GrayScale; i++)
    {
        pixelCount[i] = 0;
        pixelPro[i] = 0;
    }
    uint32 gray_sum=0;
    //统计灰度级中每个像素在整幅图像中的个数
    for (i = 0; i < height; i+=2)
    {
        for (j = 0; j < width; j+=2)
        {
            pixelCount[(int)data[i * width + j]]++;  //将当前的点的像素值作为计数数组的下标
            gray_sum+=(int)data[i * width + j];       //灰度值总和
        }
    }
    //计算每个像素值的点在整幅图像中的比例
    for (i = 0; i < GrayScale; i++)
    {
        pixelPro[i] = (float)pixelCount[i] / pixelSum;
    }
    //遍历灰度级[0,255]
    float w0, w1, u0tmp, u1tmp, u0, u1, u, deltaTmp, deltaMax = 0;
    w0 = w1 = u0tmp = u1tmp = u0 = u1 = u = deltaTmp = 0;
    for (j = 0; j < GrayScale; j++)
    {
        w0 += pixelPro[j];  //背景部分每个灰度值的像素点所占比例之和   即背景部分的比例
        u0tmp += j * pixelPro[j];  //背景部分 每个灰度值的点的比例 *灰度值
        w1=1-w0;
        u1tmp=gray_sum/pixelSum-u0tmp;
        u0 = u0tmp / w0;              //背景平均灰度
        u1 = u1tmp / w1;              //前景平均灰度
        u = u0tmp + u1tmp;            //全局平均灰度
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

/*-------------------------------------------------------------------------------------------------------------------
  @brief     普通大津求阈值
  @param     image       图像数组
             col         列 ，宽度
             row         行，长度
  @return    threshold   返回int类型的的阈值
  Sample     threshold=my_adapt_threshold(mt9v03x_image[0],MT9V03X_W, MT9V03X_H);//普通大津
  @note      据说没有山威大津快，我感觉两个区别不大
-------------------------------------------------------------------------------------------------------------------*/
int My_Adapt_Threshold(uint8*image,uint16 width, uint16 height)   //大津算法，注意计算阈值的一定要是原图像
{
    #define GrayScale 256
    int pixelCount[GrayScale];
    float pixelPro[GrayScale];
    int i, j;
    int pixelSum = width * height/4;
    int  threshold = 0;
    uint8* data = image;  //指向像素数据的指针
    for (i = 0; i < GrayScale; i++)
    {
        pixelCount[i] = 0;
        pixelPro[i] = 0;
    }
    uint32 gray_sum=0;
    for (i = 0; i < height; i+=2)//统计灰度级中每个像素在整幅图像中的个数
    {
        for (j = 0; j <width; j+=2)
        {
            pixelCount[(int)data[i * width + j]]++;  //将当前的点的像素值作为计数数组的下标
            gray_sum+=(int)data[i * width + j];       //灰度值总和
        }
    }
    for (i = 0; i < GrayScale; i++) //计算每个像素值的点在整幅图像中的比例
    {
        pixelPro[i] = (float)pixelCount[i] / pixelSum;
    }
    float w0, w1, u0tmp, u1tmp, u0, u1, u, deltaTmp, deltaMax = 0;
    w0 = w1 = u0tmp = u1tmp = u0 = u1 = u = deltaTmp = 0;
    for (j = 0; j < GrayScale; j++)//遍历灰度级[0,255]
    {
        w0 += pixelPro[j];  //背景部分每个灰度值的像素点所占比例之和   即背景部分的比例
        u0tmp += j * pixelPro[j];  //背景部分 每个灰度值的点的比例 *灰度值
        w1=1-w0;
        u1tmp=gray_sum/pixelSum-u0tmp;
        u0 = u0tmp / w0;              //背景平均灰度
        u1 = u1tmp / w1;              //前景平均灰度
        u = u0tmp + u1tmp;            //全局平均灰度
        deltaTmp = w0 * pow((u0 - u), 2) + w1 * pow((u1 - u), 2);//平方
        if (deltaTmp > deltaMax)
        {
            deltaMax = deltaTmp;//最大类间方差法
            threshold = j;
        }
        if (deltaTmp < deltaMax)
        {
            break;
        }
    }
    if(threshold>255)
        threshold=255;
    if(threshold<0)
        threshold=0;
  return threshold;
}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     图像二值化处理函数
  @param     原始图像数组，高度，宽度，二值化阈值
  @return    二值化之后的图像数组
  Sample     Image_Binarization(Threshold);//图像二值化
  @note      二值化处理，0x00黑，0xff是白，已经用宏定义替换过了
-------------------------------------------------------------------------------------------------------------------*/
void Image_Binarization(int threshold)//图像二值化
{
    uint16 i,j;
    for(i=0;i<MT9V03X_H;i++)
    {
        for(j=0;j<MT9V03X_W;j++)
        {
            if(mt9v03x_image[i][j]>=threshold)
                image_two_value[i][j]=IMG_WHITE;//白
            else
                image_two_value[i][j]=IMG_BLACK;//黑
        }
    }
}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     双最长白列巡线
  @param     null
  @return    null
  Sample     Longest_White_Column_Left();
  @note      最长白列巡线，寻找初始边界，丢线，最长白列等基础元素，后续读取这些变量来进行赛道识别
-------------------------------------------------------------------------------------------------------------------*/
void Longest_White_Column()//最长白列巡线
{
    int i, j;
    int start_column=20;//最长白列的搜索区间
    int end_column=MT9V03X_W-20;
    int left_border = 0, right_border = 0;//临时存储赛道位置
    Longest_White_Column_Left[0] = 0;//最长白列,[0]是最长白列的长度，[1】是第某列
    Longest_White_Column_Left[1] = 0;//最长白列,[0]是最长白列的长度，[1】是第某列
    Longest_White_Column_Right[0] = 0;//最长白列,[0]是最长白列的长度，[1】是第某列
    Longest_White_Column_Right[1] = 0;//最长白列,[0]是最长白列的长度，[1】是第某列
    Right_Lost_Time = 0;    //边界丢线数
    Left_Lost_Time  = 0;
    Boundry_Start_Left  = 0;//第一个非丢线点,常规边界起始点
    Boundry_Start_Right = 0;
    Both_Lost_Time = 0;//两边同时丢线数

    for (i = 0; i <=MT9V03X_H-1; i++)//数据清零
    {
        Right_Lost_Flag[i] = 0;
        Left_Lost_Flag[i] = 0;
        Left_Line[i] = 0;
        Right_Line[i] = MT9V03X_W-1;
    }
    for(i=0;i<=MT9V03X_W-1;i++)
    {
        White_Column[i] = 0;
    }

//环岛需要对最长白列范围进行限定，环岛3状态找不到上角点，可以修改下述参数
    //环岛3状态需要改变最长白列寻找范围
    if(Right_Island_Flag==1)//右环
    {
        if(Island_State==3)
        {
            start_column=40;
            end_column=MT9V03X_W-20;
        }
    }
    else if(Left_Island_Flag==1)//左环
    {
        if(Island_State==3)
        {
            start_column=20;
            end_column=MT9V03X_W-40;
        }
    }

    //从左到右，从下往上，遍历全图记录范围内的每一列白点数量
    for (j =start_column; j<=end_column; j++)
    {
        for (i = MT9V03X_H - 1; i >= 0; i--)
        {
            if(image_two_value[i][j] == IMG_BLACK)
                break;
            else
                White_Column[j]++;
        }
    }

    //从左到右找左边最长白列
    Longest_White_Column_Left[0] =0;
    for(i=start_column;i<=end_column;i++)
    {
        if (Longest_White_Column_Left[0] < White_Column[i])//找最长的那一列
        {
            Longest_White_Column_Left[0] = White_Column[i];//【0】是白列长度
            Longest_White_Column_Left[1] = i;              //【1】是下标，第j列
        }
    }
    //从右到左找右左边最长白列
    Longest_White_Column_Right[0] = 0;//【0】是白列长度
    for(i=end_column;i>=start_column;i--)//从右往左，注意条件，找到左边最长白列位置就可以停了
    {
        if (Longest_White_Column_Right[0] < White_Column[i])//找最长的那一列
        {
            Longest_White_Column_Right[0] = White_Column[i];//【0】是白列长度
            Longest_White_Column_Right[1] = i;              //【1】是下标，第j列
        }
    }

    Search_Stop_Line = Longest_White_Column_Left[0];//搜索截止行选取左或者右区别不大，他们两个理论上是一样的
    for (i = MT9V03X_H - 1; i >=MT9V03X_H-Search_Stop_Line; i--)//常规巡线
    {
        for (j = Longest_White_Column_Right[1]; j <= MT9V03X_W - 1 - 2; j++)
        {
            if (image_two_value[i][j] ==IMG_WHITE && image_two_value[i][j + 1] == IMG_BLACK && image_two_value[i][j + 2] == IMG_BLACK)//白黑黑，找到右边界
            {
                right_border = j;
                Right_Lost_Flag[i] = 0; //右丢线数组，丢线置1，不丢线置0
                break;
            }
            else if(j>=MT9V03X_W-1-2)//没找到右边界，把屏幕最右赋值给右边界
            {
                right_border = j;
                Right_Lost_Flag[i] = 1; //右丢线数组，丢线置1，不丢线置0
                break;
            }
        }
        for (j = Longest_White_Column_Left[1]; j >= 0 + 2; j--)//往左边扫描
        {
            if (image_two_value[i][j] ==IMG_WHITE && image_two_value[i][j - 1] == IMG_BLACK && image_two_value[i][j - 2] == IMG_BLACK)//黑黑白认为到达左边界
            {
                left_border = j;
                Left_Lost_Flag[i] = 0; //左丢线数组，丢线置1，不丢线置0
                break;
            }
            else if(j<=0+2)
            {
                left_border = j;//找到头都没找到边，就把屏幕最左右当做边界
                Left_Lost_Flag[i] = 1; //左丢线数组，丢线置1，不丢线置0
                break;
            }
        }
        Left_Line [i] = left_border;       //左边线线数组
        Right_Line[i] = right_border;      //右边线线数组
    }

    for (i = MT9V03X_H - 1; i >= 0; i--)//赛道数据初步分析
    {
        if (Left_Lost_Flag[i]  == 1)//单边丢线数
            Left_Lost_Time++;
        if (Right_Lost_Flag[i] == 1)
            Right_Lost_Time++;
        if (Left_Lost_Flag[i] == 1 && Right_Lost_Flag[i] == 1)//双边丢线数
            Both_Lost_Time++;
        if (Boundry_Start_Left ==  0 && Left_Lost_Flag[i]  != 1)//记录第一个非丢线点，边界起始点
            Boundry_Start_Left = i;
        if (Boundry_Start_Right == 0 && Right_Lost_Flag[i] != 1)
            Boundry_Start_Right = i;
        Road_Wide[i]=Right_Line[i]-Left_Line[i];
    }
    
    //debug使用，屏幕显示相关参数
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
    for(i=MT9V03X_H-1;i>=MT9V03X_H-Search_Stop_Line;i--)//从最底下往上扫描
    {
        image_two_value[i][Left_Line[i]+1]=IMG_BLACK;
        image_two_value[i][(Left_Line[i]+Right_Line[i])>>1]=IMG_BLACK;
        image_two_value[i][Right_Line[i]-1]=IMG_BLACK;
    }

    //在屏幕理论中线处显示红线，用于调整摄像头
    //ips200_draw_line ( MT9V03X_W/2, MT9V03X_H-10, MT9V03X_W/2, MT9V03X_H, RGB565_RED);
}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     左补线
  @param     补线的起点，终点
  @return    null
  Sample     Left_Add_Line(int x1,int y1,int x2,int y2);
  @note      补的直接是边界，点最好是可信度高的,不要乱补
-------------------------------------------------------------------------------------------------------------------*/
void Left_Add_Line(int x1,int y1,int x2,int y2)//左补线,补的是边界
{
    int i,max,a1,a2;
    int hx;
    if(x1>=MT9V03X_W-1)//起始点位置校正，排除数组越界的可能
       x1=MT9V03X_W-1;
    else if(x1<=0)
        x1=0;
     if(y1>=MT9V03X_H-1)
        y1=MT9V03X_H-1;
     else if(y1<=0)
        y1=0;
     if(x2>=MT9V03X_W-1)
        x2=MT9V03X_W-1;
     else if(x2<=0)
             x2=0;
     if(y2>=MT9V03X_H-1)
        y2=MT9V03X_H-1;
     else if(y2<=0)
             y2=0;
    a1=y1;
    a2=y2;
    if(a1>a2)//坐标互换
    {
        max=a1;
        a1=a2;
        a2=max;
    }
    for(i=a1;i<=a2;i++)//根据斜率补线即可
    {
        hx=(i-y1)*(x2-x1)/(y2-y1)+x1;
        if(hx>=MT9V03X_W)
            hx=MT9V03X_W;
        else if(hx<=0)
            hx=0;
        Left_Line[i]=hx;
    }
}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     右补线
  @param     补线的起点，终点
  @return    null
  Sample     Right_Add_Line(int x1,int y1,int x2,int y2);
  @note      补的直接是边界，点最好是可信度高的，不要乱补
-------------------------------------------------------------------------------------------------------------------*/
void Right_Add_Line(int x1,int y1,int x2,int y2)//右补线,补的是边界
{
    int i,max,a1,a2;
    int hx;
    if(x1>=MT9V03X_W-1)//起始点位置校正，排除数组越界的可能
       x1=MT9V03X_W-1;
    else if(x1<=0)
        x1=0;
    if(y1>=MT9V03X_H-1)
        y1=MT9V03X_H-1;
    else if(y1<=0)
        y1=0;
    if(x2>=MT9V03X_W-1)
        x2=MT9V03X_W-1;
    else if(x2<=0)
        x2=0;
    if(y2>=MT9V03X_H-1)
        y2=MT9V03X_H-1;
    else if(y2<=0)
         y2=0;
    a1=y1;
    a2=y2;
    if(a1>a2)//坐标互换
    {
        max=a1;
        a1=a2;
        a2=max;
    }
    for(i=a1;i<=a2;i++)//根据斜率补线即可
    {
        hx=(i-y1)*(x2-x1)/(y2-y1)+x1;
        if(hx>=MT9V03X_W)
            hx=MT9V03X_W;
        else if(hx<=0)
            hx=0;
        Right_Line[i]=hx;
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
void Find_Down_Point(int start,int end)
{
    int i,t;
    Right_Down_Find=0;
    Left_Down_Find=0;
    if(start<end)
    {
        t=start;
        start=end;
        end=t;
    }
    if(start>=MT9V03X_H-1-5)//下面5行数据不稳定，不能作为边界点来判断，舍弃
        start=MT9V03X_H-1-5;
    if(end<=MT9V03X_H-Search_Stop_Line)
        end=MT9V03X_H-Search_Stop_Line;
    if(end<=5)
       end=5;
    for(i=start;i>=end;i--)
    {
        if(Left_Down_Find==0&&//只找第一个符合条件的点
           abs(Left_Line[i]-Left_Line[i+1])<=5&&//角点的阈值可以更改
           abs(Left_Line[i+1]-Left_Line[i+2])<=5&&
           abs(Left_Line[i+2]-Left_Line[i+3])<=5&&
              (Left_Line[i]-Left_Line[i-2])>=8&&
              (Left_Line[i]-Left_Line[i-3])>=15&&
              (Left_Line[i]-Left_Line[i-4])>=15)
        {
            Left_Down_Find=i;//获取行数即可
        }
        if(Right_Down_Find==0&&//只找第一个符合条件的点
           abs(Right_Line[i]-Right_Line[i+1])<=5&&//角点的阈值可以更改
           abs(Right_Line[i+1]-Right_Line[i+2])<=5&&
           abs(Right_Line[i+2]-Right_Line[i+3])<=5&&
              (Right_Line[i]-Right_Line[i-2])<=-8&&
              (Right_Line[i]-Right_Line[i-3])<=-15&&
              (Right_Line[i]-Right_Line[i-4])<=-15)
        {
            Right_Down_Find=i;
        }
        if(Left_Down_Find!=0&&Right_Down_Find!=0)//两个找到就退出
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
void Find_Up_Point(int start,int end)
{
    int i,t;
    Left_Up_Find=0;
    Right_Up_Find=0;
    if(start<end)
    {
        t=start;
        start=end;
        end=t;
    }
    if(end<=MT9V03X_H-Search_Stop_Line)
        end=MT9V03X_H-Search_Stop_Line;
    if(end<=5)//及时最长白列非常长，也要舍弃部分点，防止数组越界
        end=5;
    if(start>=MT9V03X_H-1-5)//下面5行数据不稳定，不能作为边界点来判断，舍弃
        start=MT9V03X_H-1-5;
    for(i=start;i>=end;i--)
    {
        if(Left_Up_Find==0&&//只找第一个符合条件的点
           abs(Left_Line[i]-Left_Line[i-1])<=5&&
           abs(Left_Line[i-1]-Left_Line[i-2])<=5&&
           abs(Left_Line[i-2]-Left_Line[i-3])<=5&&
              (Left_Line[i]-Left_Line[i+2])>=8&&
              (Left_Line[i]-Left_Line[i+3])>=15&&
              (Left_Line[i]-Left_Line[i+4])>=15)
        {
            Left_Up_Find=i;//获取行数即可
        }
        if(Right_Up_Find==0&&//只找第一个符合条件的点
           abs(Right_Line[i]-Right_Line[i-1])<=5&&//下面两行位置差不多
           abs(Right_Line[i-1]-Right_Line[i-2])<=5&&
           abs(Right_Line[i-2]-Right_Line[i-3])<=5&&
              (Right_Line[i]-Right_Line[i+2])<=-8&&
              (Right_Line[i]-Right_Line[i+3])<=-15&&
              (Right_Line[i]-Right_Line[i+4])<=-15)
        {
            Right_Up_Find=i;//获取行数即可
        }
        if(Left_Up_Find!=0&&Right_Up_Find!=0)//下面两个找到就出去
        {
            break;
        }
    }
    if(abs(Right_Up_Find-Left_Up_Find)>=30)//纵向撕裂过大，视为误判
    {
        Right_Up_Find=0;
        Left_Up_Find=0;
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
    int down_search_start=0;//下点搜索开始行
    Cross_Flag=0;
    if(Island_State==0&&Ramp_Flag==0)//与环岛互斥开
    {
        Left_Up_Find=0;
        Right_Up_Find=0;
        if(Both_Lost_Time>=10)//十字必定有双边丢线，在有双边丢线的情况下再开始找角点
        {
            Find_Up_Point( MT9V03X_H-1, 0 );
            if(Left_Up_Find==0&&Right_Up_Find==0)//只要没有同时找到两个上点，直接结束
            {
                return;
            }
        }
        if(Left_Up_Find!=0&&Right_Up_Find!=0)//找到两个上点，就找到十字了
        {
            Cross_Flag=1;//对应标志位，便于各元素互斥掉
            down_search_start=Left_Up_Find>Right_Up_Find?Left_Up_Find:Right_Up_Find;//用两个上拐点坐标靠下者作为下点的搜索上限
            Find_Down_Point(MT9V03X_H-5,down_search_start+2);//在上拐点下2行作为下点的截止行
            if(Left_Down_Find<=Left_Up_Find)
            {
                Left_Down_Find=0;//下点不可能比上点还靠上
            }
            if(Right_Down_Find<=Right_Up_Find)
            {
                Right_Down_Find=0;//下点不可能比上点还靠上
            }
            if(Left_Down_Find!=0&&Right_Down_Find!=0)
            {//四个点都在，无脑连线，这种情况显然很少
                Left_Add_Line (Left_Line [Left_Up_Find ],Left_Up_Find ,Left_Line [Left_Down_Find ] ,Left_Down_Find);
                Right_Add_Line(Right_Line[Right_Up_Find],Right_Up_Find,Right_Line[Right_Down_Find],Right_Down_Find);
            }
            else if(Left_Down_Find==0&&Right_Down_Find!=0)//11//这里使用的都是斜率补线
            {//三个点                                     //01
                Lengthen_Left_Boundry(Left_Up_Find-1,MT9V03X_H-1);
                Right_Add_Line(Right_Line[Right_Up_Find],Right_Up_Find,Right_Line[Right_Down_Find],Right_Down_Find);
            }
            else if(Left_Down_Find!=0&&Right_Down_Find==0)//11
            {//三个点                                     //10
                Left_Add_Line (Left_Line [Left_Up_Find ],Left_Up_Find ,Left_Line [Left_Down_Find ] ,Left_Down_Find);
                Lengthen_Right_Boundry(Right_Up_Find-1,MT9V03X_H-1);
            }
            else if(Left_Down_Find==0&&Right_Down_Find==0)//11
            {//就俩上点                                   //00
                Lengthen_Left_Boundry (Left_Up_Find-1,MT9V03X_H-1);
                Lengthen_Right_Boundry(Right_Up_Find-1,MT9V03X_H-1);
            }
        }
        else
        {
            Cross_Flag=0;
        }
    }
    //角点相关变量，debug使用
    //ips200_showuint8(0,12,Cross_Flag);
//    ips200_showuint8(0,13,Island_State);
//    ips200_showuint8(50,12,Left_Up_Find);
//    ips200_showuint8(100,12,Right_Up_Find);
//    ips200_showuint8(50,13,Left_Down_Find);
//    ips200_showuint8(100,13,Right_Down_Find);
}


/*-------------------------------------------------------------------------------------------------------------------
  @brief     丢图检测
  @param     null
  @return    null
  Sample     Stop_Detect(void)
  @note      最长白列很短，边界不连续，停车
-------------------------------------------------------------------------------------------------------------------*/
void Img_Disappear_Detect(void)
{
    if(Barricade_Flag!=0||Stop_Flag!=0||Ramp_Flag!=0)//横断时候要关掉，停车之后也要关掉
    {
        return;
    }
    int i=0;
    uint8 black_line_count=0;
    uint8 continuity_change_right_flag=0;
    uint8 continuity_change_left_flag=0;
    continuity_change_left_flag= Continuity_Change_Left (MT9V03X_H-1,MT9V03X_H-10);//连续性判断
    continuity_change_right_flag=Continuity_Change_Right(MT9V03X_H-1,MT9V03X_H-10);

//相关参量，debug使用
//    ips200_show_int(0,16*10,Search_Stop_Line,3);
//    ips200_show_int(0,16*11,(Longest_White_Column_Left[1]-Longest_White_Column_Right[1]),3);
//    ips200_show_int(0,16*12,Road_Wide[(MT9V03X_H-Search_Stop_Line)+1],3);
//    ips200_show_int(0,16*13,Road_Wide[(MT9V03X_H-Search_Stop_Line)+2],3);
//    ips200_show_int(0,16*14,(Standard_Road_Wide[(MT9V03X_H-Search_Stop_Line)+2]-Road_Wide[(MT9V03X_H-Search_Stop_Line)+2]),3);
//    ips200_show_int(0,16*15,(Standard_Road_Wide[(MT9V03X_H-Search_Stop_Line)+1]-Road_Wide[(MT9V03X_H-Search_Stop_Line)+1]),3);
//    ips200_show_int(0,16*16,Boundry_Start_Left+Left_Lost_Time,3);
//    ips200_show_int(0,16*17,Boundry_Start_Right+Right_Lost_Time,3);

    if(continuity_change_left_flag!=0&&continuity_change_right_flag!=0&&Search_Stop_Line<=10)
    {//不连续，且搜索截止行非常短
        Img_Disappear_Flag=1;//认为丢图
        return;
    }//下面这节专门为弯道入断路准备的，图像特征就是赛道的尖尖，有个三角形
    else if(Search_Stop_Line<=45&&//截止行比较短
            (abs(Longest_White_Column_Left[1]-Longest_White_Column_Right[1])<15)&&//左右最长白列位置近
            (Road_Wide[(MT9V03X_H-Search_Stop_Line)+1]<=30)&&
            (Road_Wide[(MT9V03X_H-Search_Stop_Line)+2]<=30)&&//截止行下面赛宽很小
            (Standard_Road_Wide[(MT9V03X_H-Search_Stop_Line)+2]-Road_Wide[(MT9V03X_H-Search_Stop_Line)+2])>10&&
            (Standard_Road_Wide[(MT9V03X_H-Search_Stop_Line)+1]-Road_Wide[(MT9V03X_H-Search_Stop_Line)+1])>10&&//最长白列下面几行赛道宽度比正常短
            (Boundry_Start_Left +Left_Lost_Time ==MT9V03X_H-1)&&//丢线数+起始点==MT9V03X_H-1，说明丢的都是下面的点，上面没有丢点
            (Boundry_Start_Right+Right_Lost_Time==MT9V03X_H-1)&&//丢线数+起始点==MT9V03X_H-1，说明丢的都是下面的点
             Left_Lost_Time<10&&Right_Lost_Time<=10)//丢线数少，保证不能像90度弯一样，延伸出去
    {
        Img_Disappear_Flag=1;
        return;
    }
    else
    {
        for(i= MT9V03X_H-1; i>MT9V03X_H-1-10; i--)//选定区域全黑，认为丢图了
        {
            if(image_two_value[i][4]==IMG_BLACK&&image_two_value[i][5]==IMG_BLACK&&
                    image_two_value[i][10]==IMG_BLACK&&image_two_value[i][15]==IMG_BLACK&&
                    image_two_value[i][20]==IMG_BLACK&&image_two_value[i][25]==IMG_BLACK&&
                    image_two_value[i][64]==IMG_BLACK&&image_two_value[i][65]==IMG_BLACK&&
                    image_two_value[i][66]==IMG_BLACK&&image_two_value[i][67]==IMG_BLACK&&
                    image_two_value[i][68]==IMG_BLACK&&image_two_value[i][69]==IMG_BLACK&&
                    image_two_value[i][70]==IMG_BLACK&&image_two_value[i][75]==IMG_BLACK&&
                    image_two_value[i][66]==IMG_BLACK&&image_two_value[i][67]==IMG_BLACK&&
                    image_two_value[i][80]==IMG_BLACK&&image_two_value[i][90]==IMG_BLACK&&
                    image_two_value[i][100]==IMG_BLACK&&image_two_value[i][110]==IMG_BLACK&&
                    image_two_value[i][115]==IMG_BLACK&&image_two_value[i][120]==IMG_BLACK&&
                    image_two_value[i][125]==IMG_BLACK&&image_two_value[i][130]==IMG_BLACK&&
                    image_two_value[i][135]==IMG_BLACK&&image_two_value[i][136]==IMG_BLACK&&
                    image_two_value[i][137]==IMG_BLACK&&image_two_value[i][138]==IMG_BLACK
            )
                black_line_count++;
        }
        if(black_line_count>=5)
        {
            Img_Disappear_Flag=1;
        }
    }
}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     坡道检测
  @param     null
  @return    null
  Sample     Ramp_Detect();
  @note      赛宽变宽，测距前面有东西，丢线数少，截止行上面不丢线
-------------------------------------------------------------------------------------------------------------------*/
void Ramp_Detect(void)
{
    int i=0;
    int count=0;
    if(Cross_Flag!=0||Island_State!=0||Barricade_Flag!=0)//互斥
    {
        return;
    }

    if(Search_Stop_Line>=66)//截止行长
    {
        for(i=MT9V03X_H-1;i>MT9V03X_H-Search_Stop_Line;i--)//赛宽过长计数
        {
            if(Road_Wide[i]-Standard_Road_Wide[i]>10)//图像赛宽比标准赛宽大
            {
                count++;//赛宽过宽行
            }
        }

    }
    if(count>=10)//赛道过宽超过某一阈值
    {
        dl1a_get_distance();//tof测距
        if(dl1a_distance_mm<500)//测距测到了前面有东西
        {
            if(Ramp_Flag==0)//之前是0状态，说明是刚进坡道
            {
                Ramp_Flag=1;
            }
            else if(Ramp_Flag==3)//之前是3状态，说明是正在下坡
            {
                Ramp_Flag=4;//进4之后500ms内不判，这边是定时器中断处理
            }
        }
    }

    if(Ramp_Flag!=0)//蜂鸣器提示坡道状态
    {
        gpio_set_level(BEEP_PIN, 1);
    }
    else
    {
        gpio_set_level(BEEP_PIN, 0);
    }

//赛道超宽行计数，debug使用
//ips200_show_int(50,10*16,count,5);
}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     斑马线检测
  @param     null
  @return    null
  Sample     Zebra_Stripes_Detect(void)
  @note      边界起始靠下，最长白列较长，赛道宽度过窄，且附近大量跳变
-------------------------------------------------------------------------------------------------------------------*/
void Zebra_Stripes_Detect(void)
{
    int i=0,j=0;
    int change_count=0;//跳变计数
    int start_line=0;
    int endl_ine=0;
    int narrow_road_count=0;
    if(Cross_Flag!=0||(1<=Ramp_Flag&&Ramp_Flag<=3)||Zebra_Stripes_Flag!=0||
       Stop_Flag!=0 ||Electromagnet_Flag!=0||Barricade_Flag!=0)//元素互斥，不是十字，不是，不是坡道，不是停车
    {
        return;
    }

    ////赛宽变化判斑马线
    if(Search_Stop_Line>=60&&
       30<=Longest_White_Column_Left[1]&&Longest_White_Column_Left[1]<=MT9V03X_W-30&&
       30<=Longest_White_Column_Right[1]&&Longest_White_Column_Right[1]<=MT9V03X_W-30&&
       Boundry_Start_Left>=MT9V03X_H-15&&Boundry_Start_Right>=MT9V03X_H-15)
    {//截止行长，.最长白列的位置在中心附近，边界起始点靠下
        for(i=65;i>=20;i--)//在靠下的区域进行寻找赛道宽度过窄的地方
        {
            if((Standard_Road_Wide[i]-Road_Wide[i])>10)
            {
                narrow_road_count++;//多组赛宽变窄，才认为是斑马线
                if(narrow_road_count>=5)
                {
                    start_line=i;//记录赛道宽度很窄的位置
                    break;
                }
            }
        }
    }
    if(start_line!=0)//多组赛宽变窄，，以赛道过窄的位置为中心，划定一个范围，进行跳变计数
    {
        start_line=start_line+8;
        endl_ine=start_line-15;
        if(start_line>=MT9V03X_H-1)//限幅保护，防止数组越界
        {
            start_line=MT9V03X_H-1;
        }
        if(endl_ine<=0)//限幅保护，防止数组越界
        {
            endl_ine=0;
        }
        for(i=start_line;i>=endl_ine;i--)//区域内跳变计数
        {
            for(j=Left_Line[i];j<=Right_Line[i];j++)
            {
                if(image_two_value[i][j+1]-image_two_value[i][j]!=0)
                {
                    change_count++;

                }
            }
        }
//        ips200_show_uint(0*16,100,change_count,5);//debug使用，查看跳变数，便于适应赛道
    }
 //画出区域，便于找bug，debug使用
//        Draw_Line( Left_Line[start_line], start_line, Left_Line[endl_ine], endl_ine);
//        Draw_Line( Left_Line[start_line], start_line, Right_Line[start_line], start_line);
//        Draw_Line(Right_Line[endl_ine], endl_ine, Right_Line[start_line], start_line);
//        Draw_Line(Right_Line[endl_ine], endl_ine, Left_Line[endl_ine], endl_ine);
//        ips200_draw_line ( Left_Line[start_line], start_line, Left_Line[endl_ine], endl_ine, RGB565_RED);
//        ips200_draw_line ( Left_Line[start_line], start_line, Right_Line[start_line], start_line, RGB565_RED);
//        ips200_draw_line (Right_Line[endl_ine], endl_ine, Right_Line[start_line], start_line, RGB565_RED);
//        ips200_draw_line (Right_Line[endl_ine], endl_ine, Left_Line[endl_ine], endl_ine, RGB565_RED);

    if(change_count>30)//跳变大于某一阈值，认为找到了斑马线
    {
        Zebra_Stripes_Flag=1;
    }
//相关参数显示。debug使用
//    ips200_show_uint(0*16,50,narrow_road_count,5);
//    ips200_show_uint(1*16,50,change_count,5);
}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     直道检测
  @param     null
  @return    null
  Sample     Straight_Detect()；
  @note      利用最长白列，边界起始点，中线起始点，
-------------------------------------------------------------------------------------------------------------------*/
void Straight_Detect(void)
{
    Straight_Flag=0;
    if(Search_Stop_Line>=65)//截止行很远
    {
        if(Boundry_Start_Left>=68&&Boundry_Start_Right>=65)//起始点靠下
        {
            if(-5<=Err&&Err<=5)//误差很小
            {
                Straight_Flag=1;//认为是直道
            }
        }
    }
}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     断路检测
  @param     null
  @return    null
  Sample     Break_Road_Detect()；
  @note      利用最长白列，边界起始点，中线起始点，符合要求后切换电磁寻迹
-------------------------------------------------------------------------------------------------------------------*/
void Break_Road_Detect(void)
{
    if(Barricade_Flag!=0||Ramp_Flag!=0)//横断时候要关掉，停车之后也要关掉
    {
        return;
    }
    static uint8 break_road_state=0;

    //直道进入断路，提前判断
    if(Search_Stop_Line<=45&&//边界起始点靠下，截止行不长，左右丢线都少
       Boundry_Start_Left>=MT9V03X_H-10&&Boundry_Start_Right>=MT9V03X_H-10&&//边界起始行低
       Left_Lost_Time<=10&&Right_Lost_Time<=10&&
       (Boundry_Start_Left +Left_Lost_Time ==MT9V03X_H-1)&&//丢线数+起始点==MT9V03X_H-1，说明丢的都是下面的点，上面没有丢点
       (Boundry_Start_Right+Right_Lost_Time==MT9V03X_H-1)&&//丢线数+起始点==MT9V03X_H-1，说明丢的都是下面的点
       Left_Line[(MT9V03X_H-Search_Stop_Line)+1]>=10&&Right_Line[(MT9V03X_H-Search_Stop_Line)+1]<=(MT9V03X_W-10)&&//截止行往下不能丢线
       Left_Line[(MT9V03X_H-Search_Stop_Line)+2]>=10&&Right_Line[(MT9V03X_H-Search_Stop_Line)+2]<=(MT9V03X_W-10)&&
       Left_Line[(MT9V03X_H-Search_Stop_Line)+3]>=10&&Right_Line[(MT9V03X_H-Search_Stop_Line)+3]<=(MT9V03X_W-10)&&
       Left_Line[(MT9V03X_H-Search_Stop_Line)+4]>=10&&Right_Line[(MT9V03X_H-Search_Stop_Line)+4]<=(MT9V03X_W-10))
    {
        break_road_state=1;  //图像丢失+电磁数据正常，进入断路模式
        Electromagnet_Flag=1;//切换电磁寻迹
    }
    //普通情况进断路
    if(Electromagnet_Flag==0&&break_road_state==0&&Img_Disappear_Flag==1)
    {//目前状态是摄像头跑，断路状态是0，图像丢了
        break_road_state=1;  //图像丢失+电磁数据正常，进入断路模式
        Electromagnet_Flag=1;//切换电磁寻迹
    }

    //判断出断路
    if(break_road_state==1&&Search_Stop_Line>=50&&(Boundry_Start_Left>60||Boundry_Start_Right>60)&&(75<Longest_White_Column_Left[1]&&Longest_White_Column_Left[1]<105))
    {//在断路状态，搜索截止行高，边界起始点靠下,最长白列居中
        break_road_state=0;//截止行很长，边界起始点靠下，认为正常，换回摄像头
        Electromagnet_Flag=0;
        Img_Disappear_Flag=0;
    }

    //相关参数，debug使用
//    ips200_show_float(0,16*6,Electromagnet_Flag,5,3);
//    ips200_show_float(0,16*7,break_road_state,5,3);
//    ips200_show_float(0,16*8,Img_Disappear_Flag,5,3);
}
/*-------------------------------------------------------------------------------------------------------------------
  @brief     左边界延长
  @param     延长起始行数，延长到某行
  @return    null
  Sample     Stop_Detect(void)
  @note      从起始点向上找5个点，算出斜率，向下延长，直至结束点
-------------------------------------------------------------------------------------------------------------------*/
void Lengthen_Left_Boundry(int start,int end)
{
    int i,t;
    float k=0;
    if(start>=MT9V03X_H-1)//起始点位置校正，排除数组越界的可能
        start=MT9V03X_H-1;
    else if(start<=0)
        start=0;
    if(end>=MT9V03X_H-1)
        end=MT9V03X_H-1;
    else if(end<=0)
        end=0;
    if(end<start)//++访问，坐标互换
    {
        t=end;
        end=start;
        start=t;
    }

    if(start<=5)//因为需要在开始点向上找3个点，对于起始点过于靠上，不能做延长，只能直接连线
    {
         Left_Add_Line(Left_Line[start],start,Left_Line[end],end);
    }

    else
    {
        k=(float)(Left_Line[start]-Left_Line[start-4])/5.0;//这里的k是1/斜率
        for(i=start;i<=end;i++)
        {
            Left_Line[i]=(int)(i-start)*k+Left_Line[start];//(x=(y-y1)*k+x1),点斜式变形
            if(Left_Line[i]>=MT9V03X_W-1)
            {
                Left_Line[i]=MT9V03X_W-1;
            }
            else if(Left_Line[i]<=0)
            {
                Left_Line[i]=0;
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
void Lengthen_Right_Boundry(int start,int end)
{
    int i,t;
    float k=0;
    if(start>=MT9V03X_H-1)//起始点位置校正，排除数组越界的可能
        start=MT9V03X_H-1;
    else if(start<=0)
        start=0;
    if(end>=MT9V03X_H-1)
        end=MT9V03X_H-1;
    else if(end<=0)
        end=0;
    if(end<start)//++访问，坐标互换
    {
        t=end;
        end=start;
        start=t;
    }

    if(start<=5)//因为需要在开始点向上找3个点，对于起始点过于靠上，不能做延长，只能直接连线
    {
        Right_Add_Line(Right_Line[start],start,Right_Line[end],end);
    }
    else
    {
        k=(float)(Right_Line[start]-Right_Line[start-4])/5.0;//这里的k是1/斜率
        for(i=start;i<=end;i++)
        {
            Right_Line[i]=(int)(i-start)*k+Right_Line[start];//(x=(y-y1)*k+x1),点斜式变形
            if(Right_Line[i]>=MT9V03X_W-1)
            {
                Right_Line[i]=MT9V03X_W-1;
            }
            else if(Right_Line[i]<=0)
            {
                Right_Line[i]=0;
            }
        }
    }
}


/*-------------------------------------------------------------------------------------------------------------------
  @brief     画线
  @param     输入起始点，终点坐标，补一条宽度为2的黑线
  @return    null
  Sample     Draw_Line(0, 0,MT9V03X_W-1,MT9V03X_H-1);
             Draw_Line(MT9V03X_W-1, 0,0,MT9V03X_H-1);
                                    画一个大×
  @note     补的就是一条线，需要重新扫线
-------------------------------------------------------------------------------------------------------------------*/
void Draw_Line(int startX, int startY, int endX, int endY)
{
    int i,x,y;
    int start=0,end=0;
    if(startX>=MT9V03X_W-1)//限幅处理
        startX=MT9V03X_W-1;
    else if(startX<=0)
        startX=0;
    if(startY>=MT9V03X_H-1)
        startY=MT9V03X_H-1;
    else if(startY<=0)
        startY=0;
    if(endX>=MT9V03X_W-1)
        endX=MT9V03X_W-1;
    else if(endX<=0)
        endX=0;
    if(endY>=MT9V03X_H-1)
        endY=MT9V03X_H-1;
    else if(endY<=0)
        endY=0;
    if(startX==endX)//一条竖线
    {
        if (startY > endY)//互换
        {
            start=endY;
            end=startY;
        }
        for (i = start; i <= end; i++)
        {
            if(i<=1)
                i=1;
            image_two_value[i][startX]=IMG_BLACK;
            image_two_value[i-1][startX]=IMG_BLACK;
        }
    }
    else if(startY == endY)//补一条横线
    {
        if (startX > endX)//互换
        {
            start=endX;
            end=startX;
        }
        for (i = start; i <= end; i++)
        {
            if(startY<=1)
                startY=1;
            image_two_value[startY][i]=IMG_BLACK;
            image_two_value[startY-1][i]=IMG_BLACK;
        }
    }
    else //上面两个是水平，竖直特殊情况，下面是常见情况
    {
        if(startY>endY)//起始点矫正
        {
            start=endY;
            end=startY;
        }
        else
        {
            start=startY;
            end=endY;
        }
        for (i = start; i <= end; i++)//纵向补线，保证每一行都有黑点
        {
            x =(int)(startX+(endX-startX)*(i-startY)/(endY-startY));//两点式变形
            if(x>=MT9V03X_W-1)
                x=MT9V03X_W-1;
            else if (x<=1)
                x=1;
            image_two_value[i][x] = IMG_BLACK;
            image_two_value[i][x-1] = IMG_BLACK;
        }
        if(startX>endX)
        {
            start=endX;
            end=startX;
        }
        else
        {
            start=startX;
            end=endX;
        }
        for (i = start; i <= end; i++)//横向补线，保证每一列都有黑点
        {

            y =(int)(startY+(endY-startY)*(i-startX)/(endX-startX));//两点式变形
            if(y>=MT9V03X_H-1)
                y=MT9V03X_H-1;
            else if (y<=0)
                y=0;
            image_two_value[y][i] = IMG_BLACK;
        }
    }
}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     横断检测
  @param     null
  @return    null
  Sample
  @note      截止行很低，边界起始点靠下，tof测到前面有东西
-------------------------------------------------------------------------------------------------------------------*/
void Barricade_Detect(void)
{
    if(Barricade_Flag!=0||Ramp_Flag!=0||Electromagnet_Flag!=0)
    {//元素互斥
        return;
    }
    if(Boundry_Start_Left>=MT9V03X_H-10&&Boundry_Start_Right>=MT9V03X_H-10&&
       Left_Lost_Time<=10&&Right_Lost_Time<=10&&Search_Stop_Line<=60&&
       Left_Line[(MT9V03X_H-Search_Stop_Line)+1]>=10&&Right_Line[(MT9V03X_H-Search_Stop_Line)+1]<=(MT9V03X_W-10)&&//截止行往下不能丢线
       Left_Line[(MT9V03X_H-Search_Stop_Line)+2]>=10&&Right_Line[(MT9V03X_H-Search_Stop_Line)+2]<=(MT9V03X_W-10)&&
       Left_Line[(MT9V03X_H-Search_Stop_Line)+3]>=10&&Right_Line[(MT9V03X_H-Search_Stop_Line)+3]<=(MT9V03X_W-10)&&
       Left_Line[(MT9V03X_H-Search_Stop_Line)+4]>=10&&Right_Line[(MT9V03X_H-Search_Stop_Line)+4]<=(MT9V03X_W-10))
    {//边界起始点靠下，截止行不长，左右丢线都少，截止行往下不丢线
        dl1a_get_distance();//测距/
        if(dl1a_distance_mm<1000)//测距前面有东西，进避障模式
        {
            Barricade_Flag=1;
        }
    }
//    ips200_show_uint(5*16,100,black_count,4);
//    ips200_show_uint(5*16,120,dl1a_distance_mm,4);
}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     元素标志位显示
  @param     二值化图片数组
  @return    null
  Sample     Barricade_Flag_Show(MT9V03X_W,image_two_value,Island_State);
  @note      要在图片显示前使用，传入标志位可更改
-------------------------------------------------------------------------------------------------------------------*/
void Flag_Show_101(uint8 MT9V03XW,uint8(*InImg)[MT9V03XW],uint8 image_flag)
{
    for(uint8 H=1;H<8;H++)
    {
        for(uint8 W=MT9V03XW-20+1;W<MT9V03XW-20+8;W++)
        {
            switch (image_flag)
            {
                case 0:     *(*(InImg+H)+W)=Image_Flags[0][H][W-(MT9V03XW-20)];    break;
                case 1:     *(*(InImg+H)+W)=Image_Flags[1][H][W-(MT9V03XW-20)];    break;
                case 2:     *(*(InImg+H)+W)=Image_Flags[2][H][W-(MT9V03XW-20)];    break;
                case 3:     *(*(InImg+H)+W)=Image_Flags[3][H][W-(MT9V03XW-20)];    break;
                case 4:     *(*(InImg+H)+W)=Image_Flags[4][H][W-(MT9V03XW-20)];    break;
                case 5:     *(*(InImg+H)+W)=Image_Flags[5][H][W-(MT9V03XW-20)];    break;
                case 6:     *(*(InImg+H)+W)=Image_Flags[6][H][W-(MT9V03XW-20)];    break;
                case 7:     *(*(InImg+H)+W)=Image_Flags[7][H][W-(MT9V03XW-20)];    break;
                case 8:     *(*(InImg+H)+W)=Image_Flags[8][H][W-(MT9V03XW-20)];    break;
                case 9:     *(*(InImg+H)+W)=Image_Flags[9][H][W-(MT9V03XW-20)];    break;
                default:    break;
            }
        }
    }
}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     元素标志位显示
  @param     二值化图片数组
  @return    null
  Sample     Flag_Show_102(MT9V03X_W,image_two_value,Island_State);
  @note      要在图片显示前使用，传入标志位可更改
-------------------------------------------------------------------------------------------------------------------*/
void Flag_Show_102(uint8 MT9V03XW,uint8(*InImg)[MT9V03XW],uint8 image_flag)
{
    for(uint8 H=1;H<8;H++)
    {
        for(uint8 W=MT9V03XW-10+1;W<MT9V03XW-10+8;W++)
        {
            switch (image_flag)
            {
                    case 0:     *(*(InImg+H)+W)=Image_Flags[0][H][W-(MT9V03XW-10)];    break;
                    case 1:     *(*(InImg+H)+W)=Image_Flags[1][H][W-(MT9V03XW-10)];    break;
                    case 2:     *(*(InImg+H)+W)=Image_Flags[2][H][W-(MT9V03XW-10)];    break;
                    case 3:     *(*(InImg+H)+W)=Image_Flags[3][H][W-(MT9V03XW-10)];    break;
                    case 4:     *(*(InImg+H)+W)=Image_Flags[4][H][W-(MT9V03XW-10)];    break;
                    case 5:     *(*(InImg+H)+W)=Image_Flags[5][H][W-(MT9V03XW-10)];    break;
                    case 6:     *(*(InImg+H)+W)=Image_Flags[6][H][W-(MT9V03XW-10)];    break;
                    case 7:     *(*(InImg+H)+W)=Image_Flags[7][H][W-(MT9V03XW-10)];    break;
                    case 8:     *(*(InImg+H)+W)=Image_Flags[8][H][W-(MT9V03XW-10)];    break;
                    case 9:     *(*(InImg+H)+W)=Image_Flags[9][H][W-(MT9V03XW-10)];    break;
                    default:    break;
            }
        }
    }
}
/*-------------------------------------------------------------------------------------------------------------------
  @brief     元素标志位显示
  @param     二值化图片数组
  @return    null
  Sample     Flag_Show_201(MT9V03X_W,image_two_value,Island_State);
  @note      第二行最后一个数字
-------------------------------------------------------------------------------------------------------------------*/
void Flag_Show_201(uint8 MT9V03XW,uint8(*InImg)[MT9V03XW],uint8 image_flag)
{
    for(uint8 H=10+1;H<10+8;H++)
    {
        for(uint8 W=MT9V03XW-20+1;W<MT9V03XW-20+8;W++)
        {
            switch (image_flag)
            {
                case 0:     *(*(InImg+H)+W)=Image_Flags[0][H-10][W-(MT9V03XW-20)];    break;
                case 1:     *(*(InImg+H)+W)=Image_Flags[1][H-10][W-(MT9V03XW-20)];    break;
                case 2:     *(*(InImg+H)+W)=Image_Flags[2][H-10][W-(MT9V03XW-20)];    break;
                case 3:     *(*(InImg+H)+W)=Image_Flags[3][H-10][W-(MT9V03XW-20)];    break;
                case 4:     *(*(InImg+H)+W)=Image_Flags[4][H-10][W-(MT9V03XW-20)];    break;
                case 5:     *(*(InImg+H)+W)=Image_Flags[5][H-10][W-(MT9V03XW-20)];    break;
                case 6:     *(*(InImg+H)+W)=Image_Flags[6][H-10][W-(MT9V03XW-20)];    break;
                case 7:     *(*(InImg+H)+W)=Image_Flags[7][H-10][W-(MT9V03XW-20)];    break;
                case 8:     *(*(InImg+H)+W)=Image_Flags[8][H-10][W-(MT9V03XW-20)];    break;
                case 9:     *(*(InImg+H)+W)=Image_Flags[9][H-10][W-(MT9V03XW-20)];    break;
                default:    break;
            }
        }
    }
}
/*-------------------------------------------------------------------------------------------------------------------
  @brief     元素标志位显示
  @param     二值化图片数组
  @return    null
  Sample     Flag_Show_202(MT9V03X_W,image_two_value,Island_State);
  @note      第二行倒数第二个数字
-------------------------------------------------------------------------------------------------------------------*/
void Flag_Show_202(uint8 MT9V03XW,uint8(*InImg)[MT9V03XW],uint8 image_flag)
{
    for(uint8 H=10+1;H<10+8;H++)
    {
        for(uint8 W=MT9V03XW-10+1;W<MT9V03XW-10+8;W++)
        {
            switch (image_flag)
            {
                case 0:     *(*(InImg+H)+W)=Image_Flags[0][H-10][W-(MT9V03XW-10)];    break;
                case 1:     *(*(InImg+H)+W)=Image_Flags[1][H-10][W-(MT9V03XW-10)];    break;
                case 2:     *(*(InImg+H)+W)=Image_Flags[2][H-10][W-(MT9V03XW-10)];    break;
                case 3:     *(*(InImg+H)+W)=Image_Flags[3][H-10][W-(MT9V03XW-10)];    break;
                case 4:     *(*(InImg+H)+W)=Image_Flags[4][H-10][W-(MT9V03XW-10)];    break;
                case 5:     *(*(InImg+H)+W)=Image_Flags[5][H-10][W-(MT9V03XW-10)];    break;
                case 6:     *(*(InImg+H)+W)=Image_Flags[6][H-10][W-(MT9V03XW-10)];    break;
                case 7:     *(*(InImg+H)+W)=Image_Flags[7][H-10][W-(MT9V03XW-10)];    break;
                case 8:     *(*(InImg+H)+W)=Image_Flags[8][H-10][W-(MT9V03XW-10)];    break;
                case 9:     *(*(InImg+H)+W)=Image_Flags[9][H-10][W-(MT9V03XW-10)];    break;
                default:    break;
            }
        }
    }
}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     斑马线判断
  @param     null
  @return    null
  Sample     Zebra_Detect();
  @note      一个新想法，利用最长白列，每一列的最长白列可以看成从下往上的赛道边界，
                                           当看到斑马线时，会有较大的纵向撕裂，一条斑马线会有两次撕裂
                                           那么我用这个跳变计数，是否能识别斑马线呢，代码如下，我就不去测试了，看各位的测试结果了
-------------------------------------------------------------------------------------------------------------------*/
void Zebra_Detect(void)
{
    int j=0;
    int count=0;
    //前面的元素互斥各位自行补全
    if(Search_Stop_Line>=60)//视野很长
    {
        for(j=10;j<=MT9V03X_W-10;j++)
        {
            if(abs(White_Column[j]-White_Column[j+1])>=30)
            {
                count++;
            }
        }
    }
    if(count>=8)
    {
        Zebra_Stripes_Flag=1;
    }
}
