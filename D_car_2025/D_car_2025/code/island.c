#include "zf_common_headfile.h"
#include "island.h"


#include "zf_common_headfile.h"
#include "Island.h"



//常用基本变量
extern uint8 image_two_value[MT9V03X_H][MT9V03X_W];//二值化后的原数组
extern volatile int Left_Line[MT9V03X_H]; //左边线数组
extern volatile int Right_Line[MT9V03X_H];//右边线数组
extern volatile int Search_Stop_Line;     //搜索截止行
extern volatile int Boundry_Start_Left;   //左右边界起始点
extern volatile int Boundry_Start_Right;  //第一个非丢线点,常规边界起始点
extern volatile int Left_Lost_Time;       //边界丢线数
extern volatile int Right_Lost_Time; 
extern volatile int Both_Lost_Time;//两边同时丢线数
extern volatile float Err;//摄像头误差

//十字变量
extern volatile int Cross_Flag; 

//环岛变量
volatile int Island_State=0;     //环岛状态标志
volatile int Left_Island_Flag=0; //左右环岛标志
volatile int Right_Island_Flag=0;//左右环岛标志
volatile int Left_Up_Guai[2];    //四个拐点的坐标存储，[0]存y，第某行，{1}存x，第某列
volatile int Right_Up_Guai[2];   //四个拐点的坐标存储，[0]存y，第某行，{1}存x，第某列

//斑马线
extern volatile int Zebra_State;

//坡道标志位
extern volatile int Ramp_Flag;//坡道标志


/*-------------------------------------------------------------------------------------------------------------------
  @brief     环岛检测
  @param     null
  @return    null
  Sample     Island_Detect(void);
  @note      利用四个拐点判别函数，单调性改变函，连续性数撕裂点，分为8步
-------------------------------------------------------------------------------------------------------------------*/
void Island_Detect()
{ 
    static float k=0;                          //3和5状态的k
    static int island_state_5_down[2]={0};     //状态5时即将离开环岛，左右边界边最低点，[0]存y，第某行，{1}存x，第某列
    static int island_state_3_up[2]={0};       //状态3时即将进入环岛用，左右上面角点[0]存y，第某行，{1}存x，第某列
    static int left_down_guai[2]={0};          //四个拐点的坐标存储，[0]存y，第某行，{1}存x，第某列
    static int right_down_guai[2]={0};         //四个拐点的坐标存储，[0]存y，第某行，{1}存x，第某列
    int monotonicity_change_line[2];           //单调性改变点坐标，[0]寸某行，[1]寸某列
    int monotonicity_change_left_flag=0;       //不转折是0
    int monotonicity_change_right_flag=0;      //不转折是0
    int continuity_change_right_flag=0;        //连续是0
    int continuity_change_left_flag=0;         //连续是0

    //仅保留单调性变化判断
    monotonicity_change_right_flag=Monotonicity_Change_Right(MT9V03X_H-1-10,10);
    monotonicity_change_left_flag=Monotonicity_Change_Left(MT9V03X_H-1-10,10);

    if(Cross_Flag==0&&Island_State==0&&Ramp_Flag==0)
    {
        if(Left_Island_Flag==0)  //左环判断：基于左边界单调性变化
        {
            //核心：左边界出现单调性变化，且检测到左下拐点
            if(monotonicity_change_left_flag!=0)  //左边界单调性改变
            {
                left_down_guai[0]=Find_Left_Down_Point(MT9V03X_H-1,20);  //找左下角点
                if(left_down_guai[0]>=30)  //拐点位置有效
                {
                    Island_State=1;
                    Left_Island_Flag=1;
                }
                else  //误判，归零
                {
                    Island_State=0;
                    Left_Island_Flag=0;
                }
            }
        }

        if(Right_Island_Flag==0)  //右环判断：基于右边界单调性变化
        {
            //核心：右边界出现单调性变化，且检测到右下拐点
            if(monotonicity_change_right_flag!=0)  //右边界单调性改变
            {
                right_down_guai[0]=Find_Right_Down_Point(MT9V03X_H-1,20);  //右下点
                if(right_down_guai[0]>=30)  //拐点位置有效
                {
                    buzzer_on();
                    system_delay_ms(100);
                    buzzer_off();
                    system_delay_ms(100);
                    Island_State=1;
                    Right_Island_Flag=1;
                }
                else
                {
                    Island_State=0;
                    Right_Island_Flag=0;
                }
            }
        }
    }

    if(Left_Island_Flag==1)  //左环岛状态机流转
    {
        if(Island_State==1)
        {
            //基于左边界单调性变化点连线
            monotonicity_change_line[0]=Monotonicity_Change_Left(30,5);  //获取左边界单调性改变点
            monotonicity_change_line[1]=Left_Line[monotonicity_change_line[0]];
            Left_Add_Line((int)(monotonicity_change_line[1]*0.15),MT9V03X_H-1,monotonicity_change_line[1],monotonicity_change_line[0]);
            
            if((Island_State==1)&&(Boundry_Start_Left<30))  //下方丢线转状态2
            {
                Island_State=2;
            }
        }

        else if(Island_State==2)
        {
            monotonicity_change_line[0]=Monotonicity_Change_Left(70,10);  //左边界单调性改变点
            monotonicity_change_line[1]=Left_Line[monotonicity_change_line[0]];
            Left_Add_Line((int)(monotonicity_change_line[1]*0.1),MT9V03X_H-1,monotonicity_change_line[1],monotonicity_change_line[0]);
            
            if(Island_State==2&&(Boundry_Start_Left>=MT9V03X_H-5||monotonicity_change_line[0]>50))
            {
                Island_State=3;
                Left_Island_Flag=1;
            }
        }

        else if(Island_State==3)  //准备进环：依赖左边界拐点与单调性
        {
            if(k!=0)
            {
                K_Draw_Line(k,MT9V03X_W-30,MT9V03X_H-1,0);
                Longest_White_Column();
            }
            else
            {
                Left_Up_Guai[0]=Find_Left_Up_Point(40,5);  //左上环点
                Left_Up_Guai[1]=Left_Line[Left_Up_Guai[0]];

                if (Left_Up_Guai[0]<5)  //防误判
                {
                    Island_State=0;
                    Left_Island_Flag=0;
                }

                if(k==0&&(15<=Left_Up_Guai[0]&&Left_Up_Guai[0]<50)&&(50<Left_Up_Guai[1]&&Left_Up_Guai[1]<110))
                {
                    island_state_3_up[0]= Left_Up_Guai[0];
                    island_state_3_up[1]= Left_Up_Guai[1];
                    k=(float)((float)(MT9V03X_H-island_state_3_up[0])/(float)(MT9V03X_W-20-island_state_3_up[1]));
                    K_Draw_Line(k,MT9V03X_W-30,MT9V03X_H-1,0);
                    Longest_White_Column();
                }
            }

            if((Island_State==3)&&(abs(gz)>=60))
            {
                k=0;
                Island_State=4;
                Longest_White_Column();
            }
        }

        else if(Island_State==4)  //已进环：监测右边界单调性变化
        {
            if(abs(gz)>200)
            {
                monotonicity_change_line[0]=Monotonicity_Change_Right(MT9V03X_H-10,10);  //右边界单调性改变点
                monotonicity_change_line[1]=Right_Line[monotonicity_change_line[0]];
                
                if((Island_State==4)&&(35<=monotonicity_change_line[0]&&monotonicity_change_line[0]<=55&&monotonicity_change_line[1]>=10))
                {
                    island_state_5_down[0]=MT9V03X_H-1;
                    island_state_5_down[1]=Right_Line[MT9V03X_H-1];
                    k=(float)((float)(MT9V03X_H-monotonicity_change_line[0])/(float)(island_state_5_down[1]-monotonicity_change_line[1]));
                    K_Add_Boundry_Right(k,island_state_5_down[1],island_state_5_down[0],0);
                    Island_State=5;
                }
            }
        }

        else if(Island_State==5)  //出环
        {
            K_Add_Boundry_Right(k,island_state_5_down[1],island_state_5_down[0],0);
            
            if((Island_State==5)&&(Boundry_Start_Right<MT9V03X_H-20))
            {
                Island_State=6;
            }
        }

        else if(Island_State==6)  //继续出环
        {
            K_Add_Boundry_Right(k,island_state_5_down[1],island_state_5_down[0],0);
            
            if((Island_State==6)&&((Boundry_Start_Right>MT9V03X_H-10)||(abs(gz)>=320)))
            {
                k=0;
                Island_State=7;
            }
        }

        else if(Island_State==7)  //找左拐点（基于左边界特征）
        {
            Left_Up_Guai[0]=Find_Left_Up_Point(MT9V03X_H-10,0);
            Left_Up_Guai[1]=Left_Line[Left_Up_Guai[0]];
            
            if((Island_State==7)&&(Left_Up_Guai[1]<=100)&&(5<=Left_Up_Guai[0]&&Left_Up_Guai[0]<=MT9V03X_H-20))
            {
                Island_State=8;
            }
        }

        else if(Island_State==8)  //出环结束
        {
            Left_Up_Guai[0]=Find_Left_Up_Point(MT9V03X_H-1,10);
            Left_Up_Guai[1]=Left_Line[Left_Up_Guai[0]];
            Lengthen_Left_Boundry(Left_Up_Guai[0]-1,MT9V03X_H-1);
            
            if((Island_State==8)&&(Left_Up_Guai[0]>=MT9V03X_H-20||(Left_Up_Guai[0]<10&&Boundry_Start_Left>=MT9V03X_H-10)))
            {
                yaw=0;
                Island_State=9;
                Left_Island_Flag=0;
            }
        }
    }

    else if(Right_Island_Flag==1)  //右环岛状态机流转
    {
        if(Island_State==1)
        {
            //基于右边界单调性变化点连线
            monotonicity_change_line[0]=Monotonicity_Change_Right(30,5);  //右边界单调性改变点
            monotonicity_change_line[1]=Right_Line[monotonicity_change_line[0]];
            Right_Add_Line((int)(MT9V03X_W-1-(monotonicity_change_line[1]*0.15)),MT9V03X_H-1,monotonicity_change_line[1],monotonicity_change_line[0]);
            
            if(Boundry_Start_Right<=30)//靠近下拐点进入状态2
            {
                Island_State=2;
            }
        }

        else if(Island_State==2)
        {
            monotonicity_change_line[0]=Monotonicity_Change_Right(70,5);  //右边界单调性改变点
            monotonicity_change_line[1]=Right_Line[monotonicity_change_line[0]];
            Right_Add_Line((int)(MT9V03X_W-1-(monotonicity_change_line[1]*0.15)),MT9V03X_H-1,monotonicity_change_line[1],monotonicity_change_line[0]);
            
            if(Island_State==2&&(Boundry_Start_Right>=MT9V03X_H-5||monotonicity_change_line[0]>50))
            {
                Island_State=3;
                Right_Island_Flag=1;
            }
        }

        else if(Island_State==3)  //准备进环：依赖右边界拐点与单调性
        {
            if(k!=0)
            {
                K_Draw_Line(k,30,MT9V03X_H-1,0);
                Longest_White_Column();
            }
            else
            {
                Right_Up_Guai[0]=Find_Right_Up_Point(40,10);  //右上环点
                Right_Up_Guai[1]=Right_Line[Right_Up_Guai[0]];

                if(Right_Up_Guai[0]<10)
                {
                    Island_State=0;
                    Right_Island_Flag=0;
                }

                if(k==0&&(15<=Right_Up_Guai[0]&&Right_Up_Guai[0]<50)&&(70<Right_Up_Guai[1]&&Right_Up_Guai[1]<150))
                {
                    island_state_3_up[0]= Right_Up_Guai[0];
                    island_state_3_up[1]= Right_Up_Guai[1];
                    k=(float)((float)(MT9V03X_H-island_state_3_up[0])/(float)(20-island_state_3_up[1]));
                    K_Draw_Line(k,30,MT9V03X_H-1,0);
                    Longest_White_Column();
                }
            }

            if((Island_State==3)&&(abs(gz)>=400))
            {
                k=0;
                Island_State=4;
                Longest_White_Column();
            }
        }

        else if(Island_State==4)  //已进环：监测左边界单调性变化
        {
            monotonicity_change_line[0]=Monotonicity_Change_Left(90,10);  //左边界单调性改变点
            monotonicity_change_line[1]=Left_Line[monotonicity_change_line[0]];
            
            if((Island_State==4)&&(35<=monotonicity_change_line[0]&&monotonicity_change_line[0]<=55&&monotonicity_change_line[1]<MT9V03X_W-10))
            {
                island_state_5_down[0]=MT9V03X_H-1;
                island_state_5_down[1]=Left_Line[MT9V03X_H-1]-5;
                k=(float)((float)(MT9V03X_H-monotonicity_change_line[0])/(float)(island_state_5_down[1]-monotonicity_change_line[1]));
                K_Add_Boundry_Left(k,island_state_5_down[1],island_state_5_down[0],0);
                Island_State=5;
            }
        }

        else if(Island_State==5)  //准备出环
        {
            K_Add_Boundry_Left(k,island_state_5_down[1],island_state_5_down[0],0);
            
            if(Island_State==5&&Boundry_Start_Left<MT9V03X_H-20)
            {
                Island_State=6;
            }
        }

        else if(Island_State==6)  //继续出环
        {
            K_Add_Boundry_Left(k,island_state_5_down[1],island_state_5_down[0],0);
            
            if((Island_State==6)&&(Boundry_Start_Left>MT9V03X_H-10||abs(gz)>=400))
            {
                k=0;
                Island_State=7;
            }
        }

        else if(Island_State==7)  //找右拐点（基于右边界特征）
        {
            Right_Up_Guai[0]=Find_Right_Up_Point(MT9V03X_H-10,0);
            Right_Up_Guai[1]=Right_Line[Right_Up_Guai[0]];
            
            if((Island_State==7)&&((Right_Up_Guai[1]>=MT9V03X_W-88&&(5<=Right_Up_Guai[0]&&Right_Up_Guai[0]<=MT9V03X_H-20))))
            {
                Island_State=8;
            }
        }

        else if(Island_State==8)  //出环结束
        {
            Right_Up_Guai[0]=Find_Right_Up_Point(MT9V03X_H-1,10);
            Right_Up_Guai[1]=Right_Line[Right_Up_Guai[0]];
            Lengthen_Right_Boundry(Right_Up_Guai[0]-1,MT9V03X_H-1);
            
            if((Island_State==8)&&(Right_Up_Guai[0]>=MT9V03X_H-20||(Right_Up_Guai[0]<10&&Boundry_Start_Left>=MT9V03X_H-10)))
            {
                Island_State=9;
                Right_Island_Flag=0;
            }
        }
    }
}


/*-------------------------------------------------------------------------------------------------------------------
  @brief     左赛道连续性检测
  @param     起始点，终止点
  @return    连续返回0，不连续返回断线出行数
  Sample     Continuity_Change_Left(int start,int end);
  @note      连续性的阈值设置为5，可更改
-------------------------------------------------------------------------------------------------------------------*/
int Continuity_Change_Left(int start,int end)//连续性阈值设置为5
{
    int i;
    int t;
    int continuity_change_flag=0;
    if(Left_Lost_Time>=0.9*MT9V03X_H)//大部分都丢线，没必要判断了
       return 1;
    if(Search_Stop_Line<=5)//搜所截止行很矮
       return 1;
    if(start>=MT9V03X_H-1-5)//数组越界保护
        start=MT9V03X_H-1-5;
    if(end<=5)
       end=5;
    if(start<end)//都是从下往上计算的，反了就互换一下
    {
       t=start;
       start=end;
       end=t;
    }
    
    for(i=start;i>=end;i--)
    {
       if(abs(Left_Line[i]-Left_Line[i-1])>=5)//连续判断阈值是5,可更改
       {
            continuity_change_flag=i;
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
int Continuity_Change_Right(int start,int end)
{
    int i;
    int t;
    int continuity_change_flag=0;
    if(Right_Lost_Time>=0.9*MT9V03X_H)//大部分都丢线，没必要判断了
       return 1;
    if(start>=MT9V03X_H-5)//数组越界保护
        start=MT9V03X_H-5;
    if(end<=5)
       end=5;
    if(start<end)//都是从下往上计算的，反了就互换一下
    {
       t=start;
       start=end;
       end=t;
    }

    for(i=start;i>=end;i--)
    {
        if(abs(Right_Line[i]-Right_Line[i-1])>=5)//连续性阈值是5，可更改
       {
            continuity_change_flag=i;
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
int Find_Left_Down_Point(int start,int end)//找四个角点，返回值是角点所在的行数
{
    int i,t;
    int left_down_line=0;
    if(Left_Lost_Time>=0.9*MT9V03X_H)//大部分都丢线，没有拐点判断的意义
       return left_down_line;
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
        if(left_down_line==0&&//只找第一个符合条件的点
           abs(Left_Line[i]-Left_Line[i+1])<=5&&//角点的阈值可以更改
           abs(Left_Line[i+1]-Left_Line[i+2])<=5&&  
           abs(Left_Line[i+2]-Left_Line[i+3])<=5&&
              (Left_Line[i]-Left_Line[i-2])>=5&&
              (Left_Line[i]-Left_Line[i-3])>=10&&
              (Left_Line[i]-Left_Line[i-4])>=10)
        {
            left_down_line=i;//获取行数即可
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
int Find_Left_Up_Point(int start,int end)//找四个角点，返回值是角点所在的行数
{
    int i,t;
    int left_up_line=0;
    if(Left_Lost_Time>=0.9*MT9V03X_H)//大部分都丢线，没有拐点判断的意义
       return left_up_line;
    if(start<end)
    {
        t=start;
        start=end;
        end=t;
    }
    if(end<=MT9V03X_H-Search_Stop_Line)//搜索截止行往上的全都不判
        end=MT9V03X_H-Search_Stop_Line;
    if(end<=5)//及时最长白列非常长，也要舍弃部分点，防止数组越界
        end=5;
    if(start>=MT9V03X_H-1-5)
        start=MT9V03X_H-1-5;
    for(i=start;i>=end;i--)
    {
        if(left_up_line==0&&//只找第一个符合条件的点
           abs(Left_Line[i]-Left_Line[i-1])<=5&&
           abs(Left_Line[i-1]-Left_Line[i-2])<=5&&  
           abs(Left_Line[i-2]-Left_Line[i-3])<=5&&
              (Left_Line[i]-Left_Line[i+2])>=8&& 
              (Left_Line[i]-Left_Line[i+3])>=15&&
              (Left_Line[i]-Left_Line[i+4])>=15)
        {
            left_up_line=i;//获取行数即可
            break;
        }
    }
    return left_up_line;//如果是MT9V03X_H-1，说明没有这么个拐点
}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     右下角点检测
  @param     起始点，终止点
  @return    返回角点所在的行数，找不到返回0
  Sample     Find_Right_Down_Point(int start,int end);
  @note      角点检测阈值可根据实际值更改
-------------------------------------------------------------------------------------------------------------------*/
int Find_Right_Down_Point(int start,int end)//找四个角点，返回值是角点所在的行数
{
    int i,t;
    int right_down_line=0;
    if(Right_Lost_Time>=0.9*MT9V03X_H)//大部分都丢线，没有拐点判断的意义
        return right_down_line;
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
        if(right_down_line==0&&//只找第一个符合条件的点
           abs(Right_Line[i]-Right_Line[i+1])<=5&&//角点的阈值可以更改
           abs(Right_Line[i+1]-Right_Line[i+2])<=5&&  
           abs(Right_Line[i+2]-Right_Line[i+3])<=5&&
              (Right_Line[i]-Right_Line[i-2])<=-5&&
              (Right_Line[i]-Right_Line[i-3])<=-10&&
              (Right_Line[i]-Right_Line[i-4])<=-10)
        {
            right_down_line=i;//获取行数即可
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
int Find_Right_Up_Point(int start,int end)//找四个角点，返回值是角点所在的行数
{
    int i,t;
    int right_up_line=0;
    if(Right_Lost_Time>=0.9*MT9V03X_H)//大部分都丢线，没有拐点判断的意义
        return right_up_line;
    if(start<end)
    {
        t=start;
        start=end;
        end=t;
    }    
    if(end<=MT9V03X_H-Search_Stop_Line)//搜索截止行往上的全都不判
        end=MT9V03X_H-Search_Stop_Line;
    if(end<=5)//及时最长白列非常长，也要舍弃部分点，防止数组越界
        end=5;
    if(start>=MT9V03X_H-1-5)
        start=MT9V03X_H-1-5;
    for(i=start;i>=end;i--)
    {
        if(right_up_line==0&&//只找第一个符合条件的点
           abs(Right_Line[i]-Right_Line[i-1])<=5&&//下面两行位置差不多
           abs(Right_Line[i-1]-Right_Line[i-2])<=5&&  
           abs(Right_Line[i-2]-Right_Line[i-3])<=5&&
              (Right_Line[i]-Right_Line[i+2])<=-8&&
              (Right_Line[i]-Right_Line[i+3])<=-15&&
              (Right_Line[i]-Right_Line[i+4])<=-15)
        {
            right_up_line=i;//获取行数即可
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
int Monotonicity_Change_Left(int start,int end)//单调性改变，返回值是单调性改变点所在的行数
{
    int i;
    int monotonicity_change_line=0;
    if(Left_Lost_Time>=0.9*MT9V03X_H)//大部分都丢线，没有单调性判断的意义
       return monotonicity_change_line;
    if(start>=MT9V03X_H-1-5)//数组越界保护，在判断第i个点时
       start=MT9V03X_H-1-5; //要访问它前后5个点，数组两头的点要不能作为起点终点
    if(end<=5)
        end=5;
    if(start<=end)//递减计算，入口反了，直接返回0
      return monotonicity_change_line;
    for(i=start;i>=end;i--)//会读取前5后5数据，所以前面对输入范围有要求
    {
        if(Left_Line[i]==Left_Line[i+5]&&Left_Line[i]==Left_Line[i-5]&&
        Left_Line[i]==Left_Line[i+4]&&Left_Line[i]==Left_Line[i-4]&&
        Left_Line[i]==Left_Line[i+3]&&Left_Line[i]==Left_Line[i-3]&&
        Left_Line[i]==Left_Line[i+2]&&Left_Line[i]==Left_Line[i-2]&&
        Left_Line[i]==Left_Line[i+1]&&Left_Line[i]==Left_Line[i-1])
        {//一堆数据一样，显然不能作为单调转折点
            continue;
        }
        else if(Left_Line[i]>=Left_Line[i+5]&&Left_Line[i]>=Left_Line[i-5]&&
        Left_Line[i]>=Left_Line[i+4]&&Left_Line[i]>=Left_Line[i-4]&&
        Left_Line[i]>=Left_Line[i+3]&&Left_Line[i]>=Left_Line[i-3]&&
        Left_Line[i]>=Left_Line[i+2]&&Left_Line[i]>=Left_Line[i-2]&&
        Left_Line[i]>=Left_Line[i+1]&&Left_Line[i]>=Left_Line[i-1])
        {//就很暴力，这个数据是在前5，后5中最大的（可以取等），那就是单调突变点
            monotonicity_change_line=i;
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
int Monotonicity_Change_Right(int start,int end)//单调性改变，返回值是单调性改变点所在的行数
{
    int i;
    int monotonicity_change_line=0;

    if(Right_Lost_Time>=0.9*MT9V03X_H)//大部分都丢线，没有单调性判断的意义
        return monotonicity_change_line;
    if(start>=MT9V03X_H-1-5)//数组越界保护
        start=MT9V03X_H-1-5;
     if(end<=5)
        end=5;
    if(start<=end)
        return monotonicity_change_line;
    for(i=start;i>=end;i--)//会读取前5后5数据，所以前面对输入范围有要求
    {
        if(Right_Line[i]==Right_Line[i+5]&&Right_Line[i]==Right_Line[i-5]&&
        Right_Line[i]==Right_Line[i+4]&&Right_Line[i]==Right_Line[i-4]&&
        Right_Line[i]==Right_Line[i+3]&&Right_Line[i]==Right_Line[i-3]&&
        Right_Line[i]==Right_Line[i+2]&&Right_Line[i]==Right_Line[i-2]&&
        Right_Line[i]==Right_Line[i+1]&&Right_Line[i]==Right_Line[i-1])
        {//一堆数据一样，显然不能作为单调转折点
            continue;
        }
        else if(Right_Line[i]<=Right_Line[i+5]&&Right_Line[i]<=Right_Line[i-5]&&
        Right_Line[i]<=Right_Line[i+4]&&Right_Line[i]<=Right_Line[i-4]&&
        Right_Line[i]<=Right_Line[i+3]&&Right_Line[i]<=Right_Line[i-3]&&
        Right_Line[i]<=Right_Line[i+2]&&Right_Line[i]<=Right_Line[i-2]&&
        Right_Line[i]<=Right_Line[i+1]&&Right_Line[i]<=Right_Line[i-1])
        {//就很暴力，这个数据是在前5，后5中最大的，那就是单调突变点
            monotonicity_change_line=i;
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
void K_Add_Boundry_Left(float k,int startX,int startY,int endY)
{
    int i,t;
    if(startY>=MT9V03X_H-1)
        startY=MT9V03X_H-1;
    else if(startY<=0)
        startY=0;
    if(endY>=MT9V03X_H-1)
        endY=MT9V03X_H-1;
    else if(endY<=0)
        endY=0;
    if(startY<endY)//--操作，start需要大
    {
        t=startY;
        startY=endY;
        endY=t;
    }
    for(i=startY;i>=endY;i--)
    {
        Left_Line[i]=(int)((i-startY)/k+startX);//(y-y1)=k(x-x1)变形，x=(y-y1)/k+x1
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

/*-------------------------------------------------------------------------------------------------------------------
  @brief     通过斜率，定点补线
  @param     k       输入斜率
             startY  输入起始点纵坐标
             endY    结束点纵坐标
  @return    null    直接补边线
  Sample     K_Add_Boundry_Right(float k,int startY,int endY);
  @note      补得线直接贴在边线上
-------------------------------------------------------------------------------------------------------------------*/
void K_Add_Boundry_Right(float k,int startX,int startY,int endY)
{
    int i,t;
    if(startY>=MT9V03X_H-1)
        startY=MT9V03X_H-1;
    else if(startY<=0)
        startY=0;
    if(endY>=MT9V03X_H-1)
        endY=MT9V03X_H-1;
    else if(endY<=0)
        endY=0;
    if(startY<endY)
    {
        t=startY;
        startY=endY;
        endY=t;
    }
    for(i=startY;i>=endY;i--)
    {
        Right_Line[i]=(int)((i-startY)/k+startX);//(y-y1)=k(x-x1)变形，x=(y-y1)/k+x1
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

/*-------------------------------------------------------------------------------------------------------------------
  @brief     根据斜率划线
  @param     输入斜率，定点，画一条黑线
  @return    null
  Sample     K_Draw_Line(k, 20,MT9V03X_H-1 ,0)
  @note      补的就是一条线，需要重新扫线
-------------------------------------------------------------------------------------------------------------------*/
void K_Draw_Line(float k, int startX, int startY,int endY)
{
    int endX=0;

    if(startX>=MT9V03X_W-1)//限幅处理
        startX=MT9V03X_W-1;
    else if(startX<=0)
        startX=0;
    if(startY>=MT9V03X_H-1)
        startY=MT9V03X_H-1;
    else if(startY<=0)
        startY=0;
    if(endY>=MT9V03X_H-1)
        endY=MT9V03X_H-1;
    else if(endY<=0)
        endY=0;
    endX=(int)((endY-startY)/k+startX);//(y-y1)=k(x-x1)变形，x=(y-y1)/k+x1
    Draw_Line(startX,startY,endX,endY);
}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     获取平均赛宽
  @param     int start_line,int end_line，起始行，中止行
  @return    这几行赛宽平均值
  Sample     road_wide=Get_Road_Wide(68,69);
  @note      ++运算，向下寻找，算出平均赛宽
-------------------------------------------------------------------------------------------------------------------*/
int Get_Road_Wide(int start_line,int end_line)
{
    if(start_line>=MT9V03X_H-1)
        start_line=MT9V03X_H-1;
    else if(start_line<=0)
        start_line=0;
    if(end_line>=MT9V03X_H-1)
        end_line=MT9V03X_H-1;
    else if(end_line<=0)
        end_line=0;
    int i=0,t=0;
    int road_wide=0;
    if(start_line>end_line)//++访问，坐标反了互换
    {
        t=start_line;
        start_line=end_line;
        end_line=t;
    }
    for(i=start_line;i<=end_line;i++)
    {
        road_wide+=Right_Line[i]-Left_Line[i];
    }
    road_wide=road_wide/(end_line-start_line+1);//平均赛宽
    return road_wide;
}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     获取左赛道边界斜率
  @param     int start_line,int end_line，起始行，中止行
  @return    两点之间的斜率
  Sample     k=Get_Left_K(68,69);
  @note      两点之间得出斜率，默认第一个参数小，第二个参数大
-------------------------------------------------------------------------------------------------------------------*/
float Get_Left_K(int start_line,int end_line)
{
    if(start_line>=MT9V03X_H-1)
        start_line=MT9V03X_H-1;
    else if(start_line<=0)
        start_line=0;
    if(end_line>=MT9V03X_H-1)
        end_line=MT9V03X_H-1;
    else if(end_line<=0)
        end_line=0;
    float k=0;
    int t=0;
    if(start_line>end_line)//++访问，坐标反了互换
    {
        t=start_line;
        start_line=end_line;
        end_line=t;
    }
    k=(float)(((float)Left_Line[start_line]-(float)Left_Line[end_line])/(end_line-start_line+1));
    return k;
}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     获取右赛道边界斜率
  @param     int start_line,int end_line，起始行，中止行
  @return    两点之间的斜率
  Sample     k=Get_Right_K(68,69);
  @note      两点之间得出斜率，默认第一个参数小，第二个参数大
-------------------------------------------------------------------------------------------------------------------*/
float Get_Right_K(int start_line,int end_line)
{
    if(start_line>=MT9V03X_H-1)
        start_line=MT9V03X_H-1;
    else if(start_line<=0)
        start_line=0;
    if(end_line>=MT9V03X_H-1)
        end_line=MT9V03X_H-1;
    else if(end_line<=0)
        end_line=0;
    float k=0;
    int t=0;
    if(start_line>end_line)//++访问，坐标反了互换
    {
        t=start_line;
        start_line=end_line;
        end_line=t;
    }
    k=(float)(((float)Right_Line[start_line]-(float)Right_Line[end_line])/(end_line-start_line+1));
    return k;
}













