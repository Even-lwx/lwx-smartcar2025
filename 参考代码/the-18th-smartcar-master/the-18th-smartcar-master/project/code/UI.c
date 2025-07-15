#include "zf_common_headfile.h"
#include "UI.h"

#define MAIN_MENU_NUM  10//一级目录有7个子目录
#define SEC_MENU01_NUM 4//二级目录第一个有3个子目录
#define SEC_MENU02_NUM 18
#define SEC_MENU03_NUM 16
#define SEC_MENU04_NUM 4
#define SEC_MENU05_NUM 5
#define SEC_MENU06_NUM 2
#define SEC_MENU07_NUM 19
#define SEC_MENU08_NUM 2

//内部调用的行列信息
volatile uint8 menu_deep=1;     //菜单深度显示，1为一级菜单，2为二级菜单
volatile uint8 sec_menu_item=1; //二级菜单选择的第几行
volatile uint8 main_menu_item=1;//主菜单选择的第几行
volatile uint8 menu_show_flag=1;//1正常显示,0关掉屏幕
volatile uint8 data_change_flag=0;//数据修改标志位

//从外面引来的标志位
extern uint8 Go;           //发车阶段的标志位
extern uint8 Speed_Mode;   //速度模式,0低速,1中等,2高速
extern uint8 Go_Direction; //0右,1左
extern uint8 image_two_value[MT9V03X_H][MT9V03X_W];//二值化后的原数组
extern volatile uint8 Straight_Flag;

extern uint8 Obstacle_Dir; //0右拐，1左拐
extern uint8 Island_Switch;//环岛识别开启标志位
extern uint8 Ramp_Switch;  //坡道识别标志位
extern int Boundry_Start_Left;   //左右边界起始点
extern int Boundry_Start_Right;  //第一个非丢线点,常规边界起始点
extern int Left_Lost_Time;       //边界丢线数
extern int Right_Lost_Time;
extern int Both_Lost_Time;//两边同时丢线数
extern int Longest_White_Column_Left[2]; //最长白列,[0]是最长白列的长度，也就是Search_Stop_Line搜索截止行，[1】是第某列
extern int Longest_White_Column_Right[2];//最长白列,[0]是最长白列的长度，也就是Search_Stop_Line搜索截止行，[1】是第某列
extern volatile float Err;//摄像头误差
extern volatile int Steer_Angle;
extern volatile int Speed_Left_Real;//左右实际值
extern volatile int Speed_Right_Real;
extern volatile int Speed_Left_Set;
extern volatile int Speed_Right_Set;

//ADC参数
extern volatile float ADC_Nor_Value[ADC_NUM];//adc每个通道归一化后的值
extern volatile float ADC_Fil_Value[ADC_NUM]; //adc滤波后的值
extern volatile float ADC_Err;//adc滤波后的值

//元素状态
extern volatile int Zebra_Stripes_Flag;//斑马线
extern volatile int Island_State;      //环岛状态标志
extern volatile int Left_Island_Flag;  //左右环岛标志
extern volatile int Right_Island_Flag; //左右环岛标志
extern volatile int Cross_Flag;        //十字
extern volatile int Ramp_Flag;//坡道标志
extern volatile uint8 Img_Disappear_Flag;//0图像消失标志位
extern volatile uint8 Outside_Edge_flag; //出界标志
extern volatile uint8 Stop_Flag;         //停止标志
extern volatile uint8 Barricade_Flag;    //横断
extern volatile uint8 Electromagnet_Flag;//0是摄像头，1是电磁

//最常改的参数
extern int ADC_Speed;      //电磁速度
extern int Base_Speed;     //基准速度
extern int Island_Speed;   //环岛速度
extern int Straight_Speed; //直道高速
extern int Ramp_Speed;    //稳定200
extern int Barricade_Speed;
extern float Island_Diff;  //环岛差速
extern float Err_Diff;     //差速系数
extern float Shift_Ratio;  //变速系数
extern float Island_Diff;  //环岛差速
extern float Island_Ratio; //环岛差速

extern float Island_P;     //环岛p
extern float Island_D;     //环岛d
extern float Global_D;     //全局d
extern float P_L;          //左轮p
extern float I_L;          //左轮i
extern float P_R;          //右轮p
extern float I_R;          //右轮i


void User_Interface(void)//UI总显示选择总程序
{
    if(menu_show_flag==1)
    {
        if(menu_deep==1)//一级目录显示及选择
        {
            Main_Menu();
        }
        else if(menu_deep==2)//二级目录显示及选择
        {
            if(main_menu_item==1)//根据进入时一级目录的行数,选择进入不同的二级目录
            {
                Sec_Menu_01();//发车方向，速度模式
            }
            else if(main_menu_item==2)
            {
                Sec_Menu_02();//速度相关
            }
            else if(main_menu_item==3)
            {
                Sec_Menu_03();//元素状态，开关
            }
            else if(main_menu_item==4)
            {
                Sec_Menu_04();//方向pd系数相关
            }
            else if(main_menu_item==5)
            {
                Sec_Menu_05();//速度pi系数相关
            }
            else if(main_menu_item==6)
            {
                Sec_Menu_06();//看图
            }
            else if(main_menu_item==7)
            {
                Sec_Menu_07();//各种参数
            }
            else if(main_menu_item==8)
            {
                Sec_Menu_08();//adc电磁数据
            }
            else if(main_menu_item==9)
            {
                Sec_Menu_09();//清屏
            }
            else if(main_menu_item==10)
            {
                Sec_Menu_10();//保存参数
            }
        }
    }
    else//不显示,直接结束
    {
        return;
    }
}


void Main_Menu(void) //主菜单显示8*16字符
{
    ips200_show_string(50, 0 * 16, "Main_Select");//进入发车相关选择,
    ips200_show_string(16, 1 * 16, " Car_Go"); //进入发车相关选择
    ips200_show_string(16, 2 * 16, " Speed");  //速度调控选择
    ips200_show_string(16, 3 * 16, " Element");//元素选择
    ips200_show_string(16, 4 * 16, " PD");     //pd系数
    ips200_show_string(16, 5 * 16, " PID");    //左右轮pid系数
    ips200_show_string(16, 6 * 16, " Pic");    //看图
    ips200_show_string(16, 7 * 16, " parameter");//常用参数
    ips200_show_string(16, 8 * 16, " ADC");//常用参数
    ips200_show_string(16, 9 * 16, " screen_off");//屏幕一键关闭
    ips200_show_string(16, 10 * 16," save_para");//屏幕一键关闭
    ips200_show_string(16, main_menu_item * 16, ">");
}

void Sec_Menu_01(void)//进入发车相关选择+
{
    ips200_show_string(50, 0 * 16, " Sec_Select");//二级标题
    ips200_show_string(16, 1 * 16, " return");  //第一个是返回
    ips200_show_string(16, 2 * 16, " go");        ips200_show_uint(15*8,2*16,Go,3);            //发车
    ips200_show_string(16, 3 * 16, " dir");        ips200_show_uint(15*8,3*16,Go_Direction,3); //进入发车方向
    ips200_show_string(16, 4 * 16, " Obstacle_Dir");
    if(Obstacle_Dir==0)
        ips200_show_string(15*8,4*16, " right");
    else
        ips200_show_string(15*8,4*16, " left");

    if(data_change_flag==0)
        ips200_show_string(16,sec_menu_item*16,">");
    else if(data_change_flag==1)
        ips200_show_string(16,sec_menu_item*16,"*");
}
/*
extern int ADC_Speed;      //电磁速度
extern int Base_Speed;     //基准速度
extern int Island_Speed;   //环岛速度
extern int Straight_Speed; //直道高速

extern float Island_Diff;  //环岛差速
extern float Err_Diff;     //差速系数
extern float Shift_Ratio;  //变速系数

extern float Island_P;     //环岛p
extern float Island_D;     //环岛d
extern float Global_D;     //全局d
extern float P_L;          //左轮p
extern float I_L;          //左轮i
extern float P_R;          //右轮p
extern float I_R;          //右轮i
 */

void Sec_Menu_02(void)//速度调控选择
{
    ips200_show_string(50, 0 * 16, " Sec_Select");  //二级标题
    ips200_show_string(16, 1 * 16, " return");      //返回
    ips200_show_string(16, 2 * 16, " ADC_Speed");      ips200_show_uint(18*8,2*16,ADC_Speed,4);     //acd电磁速度
    ips200_show_string(16, 3 * 16, " Base_Speed");     ips200_show_uint(18*8,3*16,Base_Speed,4);  //基本速度
    ips200_show_string(16, 4 * 16, " Island_Speed");   ips200_show_uint(18*8,4*16,Island_Speed,4);//环岛速度
    ips200_show_string(16, 5 * 16, " Straight_Speed"); ips200_show_uint(18*8,5*16,Straight_Speed,4);//直道速度
    ips200_show_string(16, 6 * 16, " Ramp_Speed");     ips200_show_uint(18*8,6*16,Ramp_Speed,4);//坡道速度
    ips200_show_string(16, 7 * 16, " Barricade_Speed");ips200_show_uint(18*8,7*16,Barricade_Speed,4);//坡道速度
    ips200_show_string(16, 8 * 16, " Island_Diff");    ips200_show_float(18*8,8*16,Island_Diff,3,3);//环岛差速系数
    ips200_show_string(16, 9 * 16, " Island_Ratio");   ips200_show_float(18*8,9*16,Island_Ratio,3,3);//环岛差速系数
    ips200_show_string(16, 10 * 16, " Err_Diff");      ips200_show_float(18*8,10*16,Err_Diff,3,3);//环岛差速系数
    ips200_show_string(16, 11 * 16, " Shift_Ratio");   ips200_show_float(18*8,11*16,Shift_Ratio,3,3);//环岛差速系数
    ips200_show_string(16, 12 * 16, " Island_P");      ips200_show_float(18*8,12*16,Island_P,3,3);//环岛差速系数
    ips200_show_string(16, 13 * 16, " Island_D");      ips200_show_float(18*8,13*16,Island_D,3,3);//环岛差速系数
    ips200_show_string(16, 14 * 16, " Global_D");      ips200_show_float(18*8,14*16,Global_D,3,3);//环岛差速系数
    ips200_show_string(16, 15 * 16, " P_L");           ips200_show_float(18*8,15*16,P_L,3,3);//环岛差速系数
    ips200_show_string(16, 16 * 16, " I_L");           ips200_show_float(18*8,16*16,I_L,3,3);//环岛差速系数
    ips200_show_string(16, 17 * 16, " P_R");           ips200_show_float(18*8,17*16,P_R,3,3);//环岛差速系数
    ips200_show_string(16, 18 * 16, " I_R");           ips200_show_float(18*8,18*16,I_R,3,3);//环岛差速系数

    if(data_change_flag==0)
        ips200_show_string(16,sec_menu_item*16,">");
    else if(data_change_flag==1)
        ips200_show_string(16,sec_menu_item*16,"*");
}

/*
extern volatile int Zebra_Stripes_Flag;//斑马线
extern volatile int Island_State;      //环岛状态标志
extern volatile int Left_Island_Flag;  //左右环岛标志
extern volatile int Right_Island_Flag; //左右环岛标志
extern volatile int Cross_Flag;        //十字
extern volatile uint8 Img_Disappear_Flag; //0图像消失标志位
extern volatile uint8 Outside_Edge_flag;  //出界标志
extern volatile uint8 Stop_Flag;        //停止标志
extern volatile uint8 Barricade_Flag;  //横断
 */
void Sec_Menu_03(void)//元素选择
{
    ips200_show_string(50, 0 * 16, " Sec_Select");  //二级标题
    ips200_show_string(16, 1 * 16, " return");      //返回
    ips200_show_string(16, 2 * 16, " obstruct_dir"); ips200_show_uint(18*8,2*16,Obstacle_Dir,3); //路障方向
    ips200_show_string(16, 3 * 16, " island_switch");ips200_show_uint(18*8,3*16,Island_Switch,3);//环岛开关
    ips200_show_string(16, 4 * 16, " ramp_switch");  ips200_show_uint(18*8,4*16,Ramp_Switch,3);  //坡道开关
    ips200_show_string(16, 5 * 16, " Zebra_Flag");   ips200_show_uint(18*8,5*16,Zebra_Stripes_Flag,3);//斑马线
    ips200_show_string(16, 6 * 16, " Island_State"); ips200_show_uint(18*8,6*16,Island_State,3);//环岛阶段
    ips200_show_string(16, 7 * 16, " Left_Island");  ips200_show_uint(18*8,7*16,Left_Island_Flag,3);  //左环岛
    ips200_show_string(16, 8 * 16, " Right_Island"); ips200_show_uint(18*8,8*16,Right_Island_Flag,3); //右环岛
    ips200_show_string(16, 9 * 16, " Cross_Flag");   ips200_show_uint(18*8,9*16,Cross_Flag,3);//十字
    ips200_show_string(16, 10 * 16," Barricade_Flag");ips200_show_uint(18*8,10*16,Barricade_Flag,3);//横断
    ips200_show_string(16, 11 * 16," Ramp_Flag");     ips200_show_uint(18*8,11*16,Ramp_Flag,3);//坡道
    ips200_show_string(16, 12 * 16," Stop_Flag");     ips200_show_uint(18*8,12*16,Stop_Flag,3); //停止标
    ips200_show_string(16, 13 * 16," Img_Disappear"); ips200_show_uint(18*8,13*16,Img_Disappear_Flag,3);//丢图标
    ips200_show_string(16, 14 * 16," Outside_Edge");  ips200_show_uint(18*8,14*16,Outside_Edge_flag,3);//出界标
    ips200_show_string(16, 15 * 16," Electromagnet"); ips200_show_uint(18*8,15*16,Electromagnet_Flag,3);//电磁标
    ips200_show_string(16, 16 * 16," Straight_Flag"); ips200_show_uint(18*8,16*16,Straight_Flag,3);//电磁标

    if(data_change_flag==0)
        ips200_show_string(16,sec_menu_item*16,">");
    else if(data_change_flag==1)
        ips200_show_string(16,sec_menu_item*16,"*");
}

void Sec_Menu_04(void)//pd系数
{
    ips200_show_string(50, 0 * 16, " Sec_Select");  //二级标题
    ips200_show_string(16, 1 * 16, " return");      //返回
    ips200_show_string(16, 2 * 16, " island_p");ips200_show_float(15*8,2*16,Island_P,3,4);//环岛p
    ips200_show_string(16, 3 * 16, " island_d");ips200_show_float(15*8,3*16,Island_D,3,4);//环岛d
    ips200_show_string(16, 4 * 16, " global_d");ips200_show_float(15*8,4*16,Global_D,3,4);//全局d
    if(data_change_flag==0)
        ips200_show_string(16,sec_menu_item*16,">");
    else if(data_change_flag==1)
        ips200_show_string(16,sec_menu_item*16,"*");
}

void Sec_Menu_05(void)//左右轮pid系数
{
    ips200_show_string(50, 0 * 16, " Sec_Select");  //二级标题
    ips200_show_string(16, 1 * 16, " return");      //返回
    ips200_show_string(16, 2 * 16, " P_L"); ips200_show_float(15*8,2*16,P_L,3,4);//环岛p
    ips200_show_string(16, 3 * 16, " I_L"); ips200_show_float(15*8,3*16,I_L,3,4);//环岛d
    ips200_show_string(16, 4 * 16, " P_R"); ips200_show_float(15*8,4*16,P_R,3,4);//全局d
    ips200_show_string(16, 5 * 16, " I_R"); ips200_show_float(15*8,5*16,I_R,3,4);//全局d

    if(data_change_flag==0)
        ips200_show_string(16,sec_menu_item*16,">");
    else if(data_change_flag==1)
        ips200_show_string(16,sec_menu_item*16,"*");
}

void Sec_Menu_06(void)//看图
{
    if(sec_menu_item==1)
   {
        Show_Boundry();
        ips200_show_gray_image(0,0,image_two_value[0],MT9V03X_W,MT9V03X_H,MT9V03X_W,MT9V03X_H,0);
    }
    else
    {
        ips200_show_gray_image(0,0,mt9v03x_image[0],MT9V03X_W,MT9V03X_H,MT9V03X_W,MT9V03X_H,0);
    }
}

void Sec_Menu_07(void)//相关参数
{
    ips200_show_string(50, 0 * 16, " Sec_Select");  //二级标题
    ips200_show_string(16, 1 * 16, " return");      //返回
    ips200_show_string(16, 2 * 16, " l_pos");  ips200_show_int(10*8,2*16,Longest_White_Column_Left[1],3); //进入发车方向//左最长白列位置
    ips200_show_string(16, 3 * 16, " r_pos");  ips200_show_int(10*8,3*16,Longest_White_Column_Right[1],3);//右最长白列位置
    ips200_show_string(16, 4 * 16, " r_val");  ips200_show_int(10*8,4*16,Longest_White_Column_Left[0],3);//右最长白列长度
    ips200_show_string(16, 5 * 16, " r_val");  ips200_show_int(10*8,5*16,Longest_White_Column_Right[0],3);//右最长白列长度
    ips200_show_string(16, 6 * 16, " l_los");  ips200_show_int(10*8,6*16,Left_Lost_Time,3);//左丢线数
    ips200_show_string(16, 7 * 16, " r_los");  ips200_show_int(10*8,7*16,Right_Lost_Time,3);//右丢线数
    ips200_show_string(16, 8 * 16, " bo_lss");  ips200_show_int(10*8,8*16,Both_Lost_Time,3);//双边丢线
    ips200_show_string(16, 9 * 16, " l_start");ips200_show_int(10*8,9*16,Boundry_Start_Left,3);//左边界起始点
    ips200_show_string(16, 10 *16, " r_start");ips200_show_int(10*8,10*16,Boundry_Start_Right,3);//右边界起始点
    ips200_show_string(16, 11 *16, " yaw");    ips200_show_float(10*8,11*16,FJ_Angle,3,3);
    ips200_show_string(16, 12 *16, " pinch");  ips200_show_float(10*8,12*16,FJ_Pitch,3,3);
    ips200_show_string(16, 13 *16, " L_S");    ips200_show_int(10*8,13*16,Speed_Left_Set,3);//右边界起始点
    ips200_show_string(16, 14 *16, " L_R");    ips200_show_int(10*8,14*16,Speed_Left_Real,3);//右边界起始点
    ips200_show_string(16, 15 *16, " R_S");    ips200_show_int(10*8,15*16,Speed_Right_Set,3);//右边界起始点
    ips200_show_string(16, 16 *16, " R_R");    ips200_show_int(10*8,16*16,Speed_Right_Real,3);//右边界起始点
    ips200_show_string(16, 17 *16, " Err");    ips200_show_float(10*8,17*16,Err,3,3);//右边界起始点
    ips200_show_string(16, 18 *16, " steer");  ips200_show_int(10*8,18*16,Steer_Angle,3);//右边界起始点
    ips200_show_string(16, 19 *16, " tof");    ips200_show_int(10*8,19*16,dl1a_distance_mm,5);//右边界起始点

    if(data_change_flag==0)
        ips200_show_string(16,sec_menu_item*16,">");
    else if(data_change_flag==1)
        ips200_show_string(16,sec_menu_item*16,"*");
}

void Sec_Menu_08(void)//相关参数
{
    ips200_show_string(50, 0 * 16, " ADC");  //二级标题
    ips200_show_float(10,16*2,ADC_Fil_Value[0],5,2);
    ips200_show_float(10,16*3,ADC_Fil_Value[1],5,2);
    ips200_show_float(10,16*4,ADC_Fil_Value[2],5,2);
    ips200_show_float(10,16*5,ADC_Fil_Value[3],5,2);
    ips200_show_float(10,16*6,ADC_Err,5,3);
    ips200_show_float(120,16*2,ADC_Nor_Value[0],5,2);
    ips200_show_float(120,16*3,ADC_Nor_Value[1],5,2);
    ips200_show_float(120,16*4,ADC_Nor_Value[2],5,2);
    ips200_show_float(120,16*5,ADC_Nor_Value[3],5,2);
    ips200_show_float(120,16*6,ADC_Nor_Value[0]+ADC_Nor_Value[1]+ADC_Nor_Value[2]+ADC_Nor_Value[3],5,3);
}

void Sec_Menu_09(void)//清屏
{
    ips200_clear();  //清屏
    menu_show_flag=0;
    return;
}

void Sec_Menu_10(void)//保存
{
    Write_Flash();
    gpio_set_level(C13, 1);

    system_delay_ms(500);
    menu_deep=1;
    ips200_clear();  //清屏
    main_menu_item=1;
    gpio_set_level(C13, 0);
}


void Menu_Select(void)
{
    uint8 key = 0;
    key = Key_Scan(0);
    if(key)
    {
        if (KEY3_PRES == key)//用作选择，和确认
        {
            if (menu_deep == 1)//一级菜单按下该按键,进入二级菜单
            {
                menu_deep=2;     //进入二级菜单
                ips200_clear();  //清屏
                sec_menu_item=1; //光标置定
            }
            else if(menu_deep == 2)//二级菜单下要么返回一级菜单,要么进入数据修改模式
            {
                if(data_change_flag==0)//不改变参数模式，用于退出，或者进入选择模式
                {
                    if(sec_menu_item==1)//光标置于第一行,如果不是数据修改,那就直接退出二级菜单
                    {
                        menu_deep=1;
                        ips200_clear();  //清屏
                        main_menu_item=1;
                    }
                    else//只要不在第一行按下,那就进入数据修改
                    {
                        data_change_flag=1;
                    }
                }
                else
                {
                    data_change_flag=0;
                }
            }
        }
        else if (KEY5_PRES == key)//在一级目录下向下移动光标，在二级目录下移动光标或增加数值
        {
           if (menu_deep == 1) //一级目录下，只能6个循环
           {
                main_menu_item++;
                if (main_menu_item > MAIN_MENU_NUM)
                    main_menu_item = 1;
           }
           else if(menu_deep == 2)
           {
               if(data_change_flag==1)//数据修改里面就修改数据
               {
                   if(main_menu_item==1)
                   {
                       if(sec_menu_item==2)//发车
                       {
                           Go=1;
                           menu_show_flag=0;
                       }
                       else if(sec_menu_item==3)
                       {
                           Go_Direction=!Go_Direction;//发车方向
                       }
                       else if(sec_menu_item==4)
                       {
                           Obstacle_Dir=!Obstacle_Dir;
                       }
                   }
                   else if(main_menu_item==2)
                   {
                       if(sec_menu_item==2)
                       {
                           ADC_Speed++;
                       }
                       else if(sec_menu_item==3)
                       {
                           Base_Speed++;
                       }
                       else if(sec_menu_item==4)
                       {
                           Island_Speed++;
                       }
                       else if(sec_menu_item==5)
                       {
                           Straight_Speed++;
                       }
                       else if(sec_menu_item==6)
                       {
                           Ramp_Speed++;
                       }
                       else if(sec_menu_item==7)
                       {
                           Barricade_Speed++;
                       }
                       else if(sec_menu_item==8)
                       {
                           Island_Diff+=0.1;
                       }
                       else if(sec_menu_item==9)
                       {
                           Island_Ratio+=0.1;
                       }
                       else if(sec_menu_item==10)
                       {
                           Err_Diff+=0.1;
                       }
                       else if(sec_menu_item==11)
                       {
                           Shift_Ratio+=0.1;
                       }
                       else if(sec_menu_item==12)
                       {
                           Island_P+=0.1;
                       }
                       else if(sec_menu_item==13)
                       {
                           Island_D+=0.1;
                       }
                       else if(sec_menu_item==14)
                       {
                           Global_D+=0.1;
                       }
                       else if(sec_menu_item==15)
                       {
                           P_L+=0.1;
                       }
                       else if(sec_menu_item==16)
                       {
                           I_L+=0.1;
                       }
                       else if(sec_menu_item==17)
                       {
                           P_R+=0.1;
                       }
                       else if(sec_menu_item==18)
                       {
                           I_R+=0.1;
                       }
                   }
                   else if(main_menu_item==3)
                   {

                       if(sec_menu_item==2)
                       {
                           Obstacle_Dir=!Obstacle_Dir;
                       }
                       else if(sec_menu_item==3)
                       {
                           Island_Switch=!Island_Switch;
                       }
                       else if(sec_menu_item==4)
                       {
                           Ramp_Switch=!Ramp_Switch;
                       }
                       else if(sec_menu_item==15)
                       {
                           Electromagnet_Flag=!Electromagnet_Flag;
                       }

                   }
                   else if(main_menu_item==4)
                   {
                       if(sec_menu_item==2)
                       {
                           Island_P+=0.1;
                       }
                       else if(sec_menu_item==3)
                       {
                           Island_D+=0.1;
                       }
                       else if(sec_menu_item==4)
                       {
                           Global_D+=0.1;
                       }
                   }
                   else if(main_menu_item==5)
                   {
                       if(sec_menu_item==2)
                       {
                           P_L+=0.1;
                       }
                       else if(sec_menu_item==3)
                       {
                           I_L+=0.1;
                       }
                       else if(sec_menu_item==4)
                       {
                           P_R+=0.1;
                       }
                       else if(sec_menu_item==5)
                       {
                           I_R+=0.1;
                       }
                   }
                   else if(main_menu_item==6)
                   {

                   }
               }
               else//不在数据修改,就移动光标
               {
                   if(main_menu_item==1)
                   {
                       sec_menu_item++;
                       if (sec_menu_item > SEC_MENU01_NUM)
                           sec_menu_item = 1;
                   }
                   else if(main_menu_item==2)
                   {
                       sec_menu_item++;
                       if (sec_menu_item > SEC_MENU02_NUM)
                           sec_menu_item = 1;
                   }
                   else if(main_menu_item==3)
                   {
                       sec_menu_item++;
                       if (sec_menu_item > SEC_MENU03_NUM)
                           sec_menu_item = 1;
                   }
                   else if(main_menu_item==4)
                   {
                       sec_menu_item++;
                       if (sec_menu_item > SEC_MENU04_NUM)
                           sec_menu_item = 1;
                   }
                   else if(main_menu_item==5)
                   {
                       sec_menu_item++;
                       if (sec_menu_item > SEC_MENU05_NUM)
                           sec_menu_item = 1;
                   }
                   else if(main_menu_item==6)
                   {
                       sec_menu_item++;
                       if (sec_menu_item > SEC_MENU06_NUM)
                           sec_menu_item = 1;
                   }
                   else if(main_menu_item==7)
                   {
                       sec_menu_item++;
                       if (sec_menu_item > SEC_MENU07_NUM)
                           sec_menu_item = 1;
                   }
               }
           }
        }
        else if (KEY1_PRES == key)//在一级目录向上移动光标，在二级目录移动光标或减小数值
        {
            if (menu_deep == 1) //一级目录下，6个循环
            {
                main_menu_item--;
                if (main_menu_item < 1)
                    main_menu_item = MAIN_MENU_NUM;
            }
            else if(menu_deep == 2)
            {
                if(data_change_flag==1)//数据修改里面就修改数据
                {
                    if(main_menu_item==1)
                    {
                        if(sec_menu_item==2)//发车
                        {
                            Go=1;
                            menu_show_flag=0;
                        }
                        else if(sec_menu_item==3)
                        {
                            Go_Direction=!Go_Direction;//发车方向
                        }
                        else if(sec_menu_item==4)
                        {
                            Obstacle_Dir=!Obstacle_Dir;
                        }
                    }
                    else if(main_menu_item==2)
                    {
                        if(sec_menu_item==2)
                        {
                            ADC_Speed--;
                        }
                        else if(sec_menu_item==3)
                        {
                            Base_Speed--;
                        }
                        else if(sec_menu_item==4)
                        {
                            Island_Speed--;
                        }
                        else if(sec_menu_item==5)
                        {
                            Straight_Speed--;
                        }

                        else if(sec_menu_item==6)
                        {
                            Ramp_Speed--;
                        }
                        else if(sec_menu_item==7)
                        {
                            Barricade_Speed--;
                        }
                        else if(sec_menu_item==8)
                        {
                            Island_Diff-=0.1;
                        }
                        else if(sec_menu_item==9)
                        {
                            Island_Ratio-=0.1;
                        }
                        else if(sec_menu_item==10)
                        {
                            Err_Diff-=0.1;
                        }
                        else if(sec_menu_item==11)
                        {
                            Shift_Ratio-=0.1;
                        }
                        else if(sec_menu_item==12)
                        {
                            Island_P-=0.1;
                        }
                        else if(sec_menu_item==13)
                        {
                            Island_D-=0.1;
                        }
                        else if(sec_menu_item==14)
                        {
                            Global_D-=0.1;
                        }
                        else if(sec_menu_item==15)
                        {
                            P_L-=0.1;
                        }
                        else if(sec_menu_item==16)
                        {
                            I_L-=0.1;
                        }
                        else if(sec_menu_item==17)
                        {
                            P_R-=0.1;
                        }
                        else if(sec_menu_item==18)
                        {
                            I_R-=0.1;
                        }
                    }
                    else if(main_menu_item==3)
                    {
                        if(sec_menu_item==2)
                        {
                            Obstacle_Dir=!Obstacle_Dir;
                        }
                        else if(sec_menu_item==3)
                        {
                            Island_Switch=!Island_Switch;
                        }
                        else if(sec_menu_item==4)
                        {
                            Ramp_Switch=!Ramp_Switch;
                        }
                    }
                    else if(main_menu_item==4)
                    {
                        if(sec_menu_item==2)
                        {
                            Island_P-=0.1;
                        }
                        else if(sec_menu_item==3)
                        {
                            Island_D-=0.1;
                        }
                        else if(sec_menu_item==4)
                        {
                            Global_D-=0.1;
                        }
                    }
                    else if(main_menu_item==5)
                    {
                        if(sec_menu_item==2)
                        {
                            P_L-=0.1;
                        }
                        else if(sec_menu_item==3)
                        {
                            I_L-=0.1;
                        }
                        else if(sec_menu_item==4)
                        {
                            P_R-=0.1;
                        }
                        else if(sec_menu_item==5)
                        {
                            I_R-=0.1;
                        }
                    }
                }
                else
                {
                     if(main_menu_item==1)
                     {
                         sec_menu_item--;
                        if (sec_menu_item < 1)
                            sec_menu_item = SEC_MENU01_NUM;
                     }
                     else if(main_menu_item==2)
                     {
                         sec_menu_item--;
                         if (sec_menu_item < 1)
                             sec_menu_item = SEC_MENU02_NUM;
                     }
                     else if(main_menu_item==3)
                     {
                         sec_menu_item--;
                         if (sec_menu_item < 1)
                             sec_menu_item = SEC_MENU03_NUM;
                     }
                     else if(main_menu_item==4)
                     {
                         sec_menu_item--;
                         if (sec_menu_item < 1)
                             sec_menu_item = SEC_MENU04_NUM;
                     }
                     else if(main_menu_item==5)
                     {
                         sec_menu_item--;
                         if (sec_menu_item < 1)
                             sec_menu_item = SEC_MENU05_NUM;
                     }
                     else if(main_menu_item==6)
                     {
                         sec_menu_item--;
                         if (sec_menu_item < 1)
                             sec_menu_item = SEC_MENU06_NUM;
                     }
                     else if(main_menu_item==7)
                     {
                         sec_menu_item--;
                         if (sec_menu_item < 1)
                             sec_menu_item = SEC_MENU07_NUM;
                     }
                }
            }
        }
        else if (KEY4_PRES == key)//第四个按键，一键发车
        {
            if (menu_deep == 1)
            {
                Go=1;
                ips200_clear();  //清屏
                menu_show_flag=!menu_show_flag;
            }
        }
        else if (KEY2_PRES == key)//左边，一键调车
        {
            if (menu_deep == 1)
            {
                Go=7;
                ips200_clear();  //清屏
                menu_show_flag=!menu_show_flag;
            }
        }

    }
}



