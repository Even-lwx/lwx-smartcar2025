#include "zf_common_headfile.h"
#include "UI.h"

#define MAIN_MENU_NUM  10//һ��Ŀ¼��7����Ŀ¼
#define SEC_MENU01_NUM 4//����Ŀ¼��һ����3����Ŀ¼
#define SEC_MENU02_NUM 18
#define SEC_MENU03_NUM 16
#define SEC_MENU04_NUM 4
#define SEC_MENU05_NUM 5
#define SEC_MENU06_NUM 2
#define SEC_MENU07_NUM 19
#define SEC_MENU08_NUM 2

//�ڲ����õ�������Ϣ
volatile uint8 menu_deep=1;     //�˵������ʾ��1Ϊһ���˵���2Ϊ�����˵�
volatile uint8 sec_menu_item=1; //�����˵�ѡ��ĵڼ���
volatile uint8 main_menu_item=1;//���˵�ѡ��ĵڼ���
volatile uint8 menu_show_flag=1;//1������ʾ,0�ص���Ļ
volatile uint8 data_change_flag=0;//�����޸ı�־λ

//�����������ı�־λ
extern uint8 Go;           //�����׶εı�־λ
extern uint8 Speed_Mode;   //�ٶ�ģʽ,0����,1�е�,2����
extern uint8 Go_Direction; //0��,1��
extern uint8 image_two_value[MT9V03X_H][MT9V03X_W];//��ֵ�����ԭ����
extern volatile uint8 Straight_Flag;

extern uint8 Obstacle_Dir; //0�ҹգ�1���
extern uint8 Island_Switch;//����ʶ������־λ
extern uint8 Ramp_Switch;  //�µ�ʶ���־λ
extern int Boundry_Start_Left;   //���ұ߽���ʼ��
extern int Boundry_Start_Right;  //��һ���Ƕ��ߵ�,����߽���ʼ��
extern int Left_Lost_Time;       //�߽綪����
extern int Right_Lost_Time;
extern int Both_Lost_Time;//����ͬʱ������
extern int Longest_White_Column_Left[2]; //�����,[0]������еĳ��ȣ�Ҳ����Search_Stop_Line������ֹ�У�[1���ǵ�ĳ��
extern int Longest_White_Column_Right[2];//�����,[0]������еĳ��ȣ�Ҳ����Search_Stop_Line������ֹ�У�[1���ǵ�ĳ��
extern volatile float Err;//����ͷ���
extern volatile int Steer_Angle;
extern volatile int Speed_Left_Real;//����ʵ��ֵ
extern volatile int Speed_Right_Real;
extern volatile int Speed_Left_Set;
extern volatile int Speed_Right_Set;

//ADC����
extern volatile float ADC_Nor_Value[ADC_NUM];//adcÿ��ͨ����һ�����ֵ
extern volatile float ADC_Fil_Value[ADC_NUM]; //adc�˲����ֵ
extern volatile float ADC_Err;//adc�˲����ֵ

//Ԫ��״̬
extern volatile int Zebra_Stripes_Flag;//������
extern volatile int Island_State;      //����״̬��־
extern volatile int Left_Island_Flag;  //���һ�����־
extern volatile int Right_Island_Flag; //���һ�����־
extern volatile int Cross_Flag;        //ʮ��
extern volatile int Ramp_Flag;//�µ���־
extern volatile uint8 Img_Disappear_Flag;//0ͼ����ʧ��־λ
extern volatile uint8 Outside_Edge_flag; //�����־
extern volatile uint8 Stop_Flag;         //ֹͣ��־
extern volatile uint8 Barricade_Flag;    //���
extern volatile uint8 Electromagnet_Flag;//0������ͷ��1�ǵ��

//��ĵĲ���
extern int ADC_Speed;      //����ٶ�
extern int Base_Speed;     //��׼�ٶ�
extern int Island_Speed;   //�����ٶ�
extern int Straight_Speed; //ֱ������
extern int Ramp_Speed;    //�ȶ�200
extern int Barricade_Speed;
extern float Island_Diff;  //��������
extern float Err_Diff;     //����ϵ��
extern float Shift_Ratio;  //����ϵ��
extern float Island_Diff;  //��������
extern float Island_Ratio; //��������

extern float Island_P;     //����p
extern float Island_D;     //����d
extern float Global_D;     //ȫ��d
extern float P_L;          //����p
extern float I_L;          //����i
extern float P_R;          //����p
extern float I_R;          //����i


void User_Interface(void)//UI����ʾѡ���ܳ���
{
    if(menu_show_flag==1)
    {
        if(menu_deep==1)//һ��Ŀ¼��ʾ��ѡ��
        {
            Main_Menu();
        }
        else if(menu_deep==2)//����Ŀ¼��ʾ��ѡ��
        {
            if(main_menu_item==1)//���ݽ���ʱһ��Ŀ¼������,ѡ����벻ͬ�Ķ���Ŀ¼
            {
                Sec_Menu_01();//���������ٶ�ģʽ
            }
            else if(main_menu_item==2)
            {
                Sec_Menu_02();//�ٶ����
            }
            else if(main_menu_item==3)
            {
                Sec_Menu_03();//Ԫ��״̬������
            }
            else if(main_menu_item==4)
            {
                Sec_Menu_04();//����pdϵ�����
            }
            else if(main_menu_item==5)
            {
                Sec_Menu_05();//�ٶ�piϵ�����
            }
            else if(main_menu_item==6)
            {
                Sec_Menu_06();//��ͼ
            }
            else if(main_menu_item==7)
            {
                Sec_Menu_07();//���ֲ���
            }
            else if(main_menu_item==8)
            {
                Sec_Menu_08();//adc�������
            }
            else if(main_menu_item==9)
            {
                Sec_Menu_09();//����
            }
            else if(main_menu_item==10)
            {
                Sec_Menu_10();//�������
            }
        }
    }
    else//����ʾ,ֱ�ӽ���
    {
        return;
    }
}


void Main_Menu(void) //���˵���ʾ8*16�ַ�
{
    ips200_show_string(50, 0 * 16, "Main_Select");//���뷢�����ѡ��,
    ips200_show_string(16, 1 * 16, " Car_Go"); //���뷢�����ѡ��
    ips200_show_string(16, 2 * 16, " Speed");  //�ٶȵ���ѡ��
    ips200_show_string(16, 3 * 16, " Element");//Ԫ��ѡ��
    ips200_show_string(16, 4 * 16, " PD");     //pdϵ��
    ips200_show_string(16, 5 * 16, " PID");    //������pidϵ��
    ips200_show_string(16, 6 * 16, " Pic");    //��ͼ
    ips200_show_string(16, 7 * 16, " parameter");//���ò���
    ips200_show_string(16, 8 * 16, " ADC");//���ò���
    ips200_show_string(16, 9 * 16, " screen_off");//��Ļһ���ر�
    ips200_show_string(16, 10 * 16," save_para");//��Ļһ���ر�
    ips200_show_string(16, main_menu_item * 16, ">");
}

void Sec_Menu_01(void)//���뷢�����ѡ��+
{
    ips200_show_string(50, 0 * 16, " Sec_Select");//��������
    ips200_show_string(16, 1 * 16, " return");  //��һ���Ƿ���
    ips200_show_string(16, 2 * 16, " go");        ips200_show_uint(15*8,2*16,Go,3);            //����
    ips200_show_string(16, 3 * 16, " dir");        ips200_show_uint(15*8,3*16,Go_Direction,3); //���뷢������
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
extern int ADC_Speed;      //����ٶ�
extern int Base_Speed;     //��׼�ٶ�
extern int Island_Speed;   //�����ٶ�
extern int Straight_Speed; //ֱ������

extern float Island_Diff;  //��������
extern float Err_Diff;     //����ϵ��
extern float Shift_Ratio;  //����ϵ��

extern float Island_P;     //����p
extern float Island_D;     //����d
extern float Global_D;     //ȫ��d
extern float P_L;          //����p
extern float I_L;          //����i
extern float P_R;          //����p
extern float I_R;          //����i
 */

void Sec_Menu_02(void)//�ٶȵ���ѡ��
{
    ips200_show_string(50, 0 * 16, " Sec_Select");  //��������
    ips200_show_string(16, 1 * 16, " return");      //����
    ips200_show_string(16, 2 * 16, " ADC_Speed");      ips200_show_uint(18*8,2*16,ADC_Speed,4);     //acd����ٶ�
    ips200_show_string(16, 3 * 16, " Base_Speed");     ips200_show_uint(18*8,3*16,Base_Speed,4);  //�����ٶ�
    ips200_show_string(16, 4 * 16, " Island_Speed");   ips200_show_uint(18*8,4*16,Island_Speed,4);//�����ٶ�
    ips200_show_string(16, 5 * 16, " Straight_Speed"); ips200_show_uint(18*8,5*16,Straight_Speed,4);//ֱ���ٶ�
    ips200_show_string(16, 6 * 16, " Ramp_Speed");     ips200_show_uint(18*8,6*16,Ramp_Speed,4);//�µ��ٶ�
    ips200_show_string(16, 7 * 16, " Barricade_Speed");ips200_show_uint(18*8,7*16,Barricade_Speed,4);//�µ��ٶ�
    ips200_show_string(16, 8 * 16, " Island_Diff");    ips200_show_float(18*8,8*16,Island_Diff,3,3);//��������ϵ��
    ips200_show_string(16, 9 * 16, " Island_Ratio");   ips200_show_float(18*8,9*16,Island_Ratio,3,3);//��������ϵ��
    ips200_show_string(16, 10 * 16, " Err_Diff");      ips200_show_float(18*8,10*16,Err_Diff,3,3);//��������ϵ��
    ips200_show_string(16, 11 * 16, " Shift_Ratio");   ips200_show_float(18*8,11*16,Shift_Ratio,3,3);//��������ϵ��
    ips200_show_string(16, 12 * 16, " Island_P");      ips200_show_float(18*8,12*16,Island_P,3,3);//��������ϵ��
    ips200_show_string(16, 13 * 16, " Island_D");      ips200_show_float(18*8,13*16,Island_D,3,3);//��������ϵ��
    ips200_show_string(16, 14 * 16, " Global_D");      ips200_show_float(18*8,14*16,Global_D,3,3);//��������ϵ��
    ips200_show_string(16, 15 * 16, " P_L");           ips200_show_float(18*8,15*16,P_L,3,3);//��������ϵ��
    ips200_show_string(16, 16 * 16, " I_L");           ips200_show_float(18*8,16*16,I_L,3,3);//��������ϵ��
    ips200_show_string(16, 17 * 16, " P_R");           ips200_show_float(18*8,17*16,P_R,3,3);//��������ϵ��
    ips200_show_string(16, 18 * 16, " I_R");           ips200_show_float(18*8,18*16,I_R,3,3);//��������ϵ��

    if(data_change_flag==0)
        ips200_show_string(16,sec_menu_item*16,">");
    else if(data_change_flag==1)
        ips200_show_string(16,sec_menu_item*16,"*");
}

/*
extern volatile int Zebra_Stripes_Flag;//������
extern volatile int Island_State;      //����״̬��־
extern volatile int Left_Island_Flag;  //���һ�����־
extern volatile int Right_Island_Flag; //���һ�����־
extern volatile int Cross_Flag;        //ʮ��
extern volatile uint8 Img_Disappear_Flag; //0ͼ����ʧ��־λ
extern volatile uint8 Outside_Edge_flag;  //�����־
extern volatile uint8 Stop_Flag;        //ֹͣ��־
extern volatile uint8 Barricade_Flag;  //���
 */
void Sec_Menu_03(void)//Ԫ��ѡ��
{
    ips200_show_string(50, 0 * 16, " Sec_Select");  //��������
    ips200_show_string(16, 1 * 16, " return");      //����
    ips200_show_string(16, 2 * 16, " obstruct_dir"); ips200_show_uint(18*8,2*16,Obstacle_Dir,3); //·�Ϸ���
    ips200_show_string(16, 3 * 16, " island_switch");ips200_show_uint(18*8,3*16,Island_Switch,3);//��������
    ips200_show_string(16, 4 * 16, " ramp_switch");  ips200_show_uint(18*8,4*16,Ramp_Switch,3);  //�µ�����
    ips200_show_string(16, 5 * 16, " Zebra_Flag");   ips200_show_uint(18*8,5*16,Zebra_Stripes_Flag,3);//������
    ips200_show_string(16, 6 * 16, " Island_State"); ips200_show_uint(18*8,6*16,Island_State,3);//�����׶�
    ips200_show_string(16, 7 * 16, " Left_Island");  ips200_show_uint(18*8,7*16,Left_Island_Flag,3);  //�󻷵�
    ips200_show_string(16, 8 * 16, " Right_Island"); ips200_show_uint(18*8,8*16,Right_Island_Flag,3); //�һ���
    ips200_show_string(16, 9 * 16, " Cross_Flag");   ips200_show_uint(18*8,9*16,Cross_Flag,3);//ʮ��
    ips200_show_string(16, 10 * 16," Barricade_Flag");ips200_show_uint(18*8,10*16,Barricade_Flag,3);//���
    ips200_show_string(16, 11 * 16," Ramp_Flag");     ips200_show_uint(18*8,11*16,Ramp_Flag,3);//�µ�
    ips200_show_string(16, 12 * 16," Stop_Flag");     ips200_show_uint(18*8,12*16,Stop_Flag,3); //ֹͣ��
    ips200_show_string(16, 13 * 16," Img_Disappear"); ips200_show_uint(18*8,13*16,Img_Disappear_Flag,3);//��ͼ��
    ips200_show_string(16, 14 * 16," Outside_Edge");  ips200_show_uint(18*8,14*16,Outside_Edge_flag,3);//�����
    ips200_show_string(16, 15 * 16," Electromagnet"); ips200_show_uint(18*8,15*16,Electromagnet_Flag,3);//��ű�
    ips200_show_string(16, 16 * 16," Straight_Flag"); ips200_show_uint(18*8,16*16,Straight_Flag,3);//��ű�

    if(data_change_flag==0)
        ips200_show_string(16,sec_menu_item*16,">");
    else if(data_change_flag==1)
        ips200_show_string(16,sec_menu_item*16,"*");
}

void Sec_Menu_04(void)//pdϵ��
{
    ips200_show_string(50, 0 * 16, " Sec_Select");  //��������
    ips200_show_string(16, 1 * 16, " return");      //����
    ips200_show_string(16, 2 * 16, " island_p");ips200_show_float(15*8,2*16,Island_P,3,4);//����p
    ips200_show_string(16, 3 * 16, " island_d");ips200_show_float(15*8,3*16,Island_D,3,4);//����d
    ips200_show_string(16, 4 * 16, " global_d");ips200_show_float(15*8,4*16,Global_D,3,4);//ȫ��d
    if(data_change_flag==0)
        ips200_show_string(16,sec_menu_item*16,">");
    else if(data_change_flag==1)
        ips200_show_string(16,sec_menu_item*16,"*");
}

void Sec_Menu_05(void)//������pidϵ��
{
    ips200_show_string(50, 0 * 16, " Sec_Select");  //��������
    ips200_show_string(16, 1 * 16, " return");      //����
    ips200_show_string(16, 2 * 16, " P_L"); ips200_show_float(15*8,2*16,P_L,3,4);//����p
    ips200_show_string(16, 3 * 16, " I_L"); ips200_show_float(15*8,3*16,I_L,3,4);//����d
    ips200_show_string(16, 4 * 16, " P_R"); ips200_show_float(15*8,4*16,P_R,3,4);//ȫ��d
    ips200_show_string(16, 5 * 16, " I_R"); ips200_show_float(15*8,5*16,I_R,3,4);//ȫ��d

    if(data_change_flag==0)
        ips200_show_string(16,sec_menu_item*16,">");
    else if(data_change_flag==1)
        ips200_show_string(16,sec_menu_item*16,"*");
}

void Sec_Menu_06(void)//��ͼ
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

void Sec_Menu_07(void)//��ز���
{
    ips200_show_string(50, 0 * 16, " Sec_Select");  //��������
    ips200_show_string(16, 1 * 16, " return");      //����
    ips200_show_string(16, 2 * 16, " l_pos");  ips200_show_int(10*8,2*16,Longest_White_Column_Left[1],3); //���뷢������//�������λ��
    ips200_show_string(16, 3 * 16, " r_pos");  ips200_show_int(10*8,3*16,Longest_White_Column_Right[1],3);//�������λ��
    ips200_show_string(16, 4 * 16, " r_val");  ips200_show_int(10*8,4*16,Longest_White_Column_Left[0],3);//������г���
    ips200_show_string(16, 5 * 16, " r_val");  ips200_show_int(10*8,5*16,Longest_White_Column_Right[0],3);//������г���
    ips200_show_string(16, 6 * 16, " l_los");  ips200_show_int(10*8,6*16,Left_Lost_Time,3);//������
    ips200_show_string(16, 7 * 16, " r_los");  ips200_show_int(10*8,7*16,Right_Lost_Time,3);//�Ҷ�����
    ips200_show_string(16, 8 * 16, " bo_lss");  ips200_show_int(10*8,8*16,Both_Lost_Time,3);//˫�߶���
    ips200_show_string(16, 9 * 16, " l_start");ips200_show_int(10*8,9*16,Boundry_Start_Left,3);//��߽���ʼ��
    ips200_show_string(16, 10 *16, " r_start");ips200_show_int(10*8,10*16,Boundry_Start_Right,3);//�ұ߽���ʼ��
    ips200_show_string(16, 11 *16, " yaw");    ips200_show_float(10*8,11*16,FJ_Angle,3,3);
    ips200_show_string(16, 12 *16, " pinch");  ips200_show_float(10*8,12*16,FJ_Pitch,3,3);
    ips200_show_string(16, 13 *16, " L_S");    ips200_show_int(10*8,13*16,Speed_Left_Set,3);//�ұ߽���ʼ��
    ips200_show_string(16, 14 *16, " L_R");    ips200_show_int(10*8,14*16,Speed_Left_Real,3);//�ұ߽���ʼ��
    ips200_show_string(16, 15 *16, " R_S");    ips200_show_int(10*8,15*16,Speed_Right_Set,3);//�ұ߽���ʼ��
    ips200_show_string(16, 16 *16, " R_R");    ips200_show_int(10*8,16*16,Speed_Right_Real,3);//�ұ߽���ʼ��
    ips200_show_string(16, 17 *16, " Err");    ips200_show_float(10*8,17*16,Err,3,3);//�ұ߽���ʼ��
    ips200_show_string(16, 18 *16, " steer");  ips200_show_int(10*8,18*16,Steer_Angle,3);//�ұ߽���ʼ��
    ips200_show_string(16, 19 *16, " tof");    ips200_show_int(10*8,19*16,dl1a_distance_mm,5);//�ұ߽���ʼ��

    if(data_change_flag==0)
        ips200_show_string(16,sec_menu_item*16,">");
    else if(data_change_flag==1)
        ips200_show_string(16,sec_menu_item*16,"*");
}

void Sec_Menu_08(void)//��ز���
{
    ips200_show_string(50, 0 * 16, " ADC");  //��������
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

void Sec_Menu_09(void)//����
{
    ips200_clear();  //����
    menu_show_flag=0;
    return;
}

void Sec_Menu_10(void)//����
{
    Write_Flash();
    gpio_set_level(C13, 1);

    system_delay_ms(500);
    menu_deep=1;
    ips200_clear();  //����
    main_menu_item=1;
    gpio_set_level(C13, 0);
}


void Menu_Select(void)
{
    uint8 key = 0;
    key = Key_Scan(0);
    if(key)
    {
        if (KEY3_PRES == key)//����ѡ�񣬺�ȷ��
        {
            if (menu_deep == 1)//һ���˵����¸ð���,��������˵�
            {
                menu_deep=2;     //��������˵�
                ips200_clear();  //����
                sec_menu_item=1; //����ö�
            }
            else if(menu_deep == 2)//�����˵���Ҫô����һ���˵�,Ҫô���������޸�ģʽ
            {
                if(data_change_flag==0)//���ı����ģʽ�������˳������߽���ѡ��ģʽ
                {
                    if(sec_menu_item==1)//������ڵ�һ��,������������޸�,�Ǿ�ֱ���˳������˵�
                    {
                        menu_deep=1;
                        ips200_clear();  //����
                        main_menu_item=1;
                    }
                    else//ֻҪ���ڵ�һ�а���,�Ǿͽ��������޸�
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
        else if (KEY5_PRES == key)//��һ��Ŀ¼�������ƶ���꣬�ڶ���Ŀ¼���ƶ�����������ֵ
        {
           if (menu_deep == 1) //һ��Ŀ¼�£�ֻ��6��ѭ��
           {
                main_menu_item++;
                if (main_menu_item > MAIN_MENU_NUM)
                    main_menu_item = 1;
           }
           else if(menu_deep == 2)
           {
               if(data_change_flag==1)//�����޸�������޸�����
               {
                   if(main_menu_item==1)
                   {
                       if(sec_menu_item==2)//����
                       {
                           Go=1;
                           menu_show_flag=0;
                       }
                       else if(sec_menu_item==3)
                       {
                           Go_Direction=!Go_Direction;//��������
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
               else//���������޸�,���ƶ����
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
        else if (KEY1_PRES == key)//��һ��Ŀ¼�����ƶ���꣬�ڶ���Ŀ¼�ƶ������С��ֵ
        {
            if (menu_deep == 1) //һ��Ŀ¼�£�6��ѭ��
            {
                main_menu_item--;
                if (main_menu_item < 1)
                    main_menu_item = MAIN_MENU_NUM;
            }
            else if(menu_deep == 2)
            {
                if(data_change_flag==1)//�����޸�������޸�����
                {
                    if(main_menu_item==1)
                    {
                        if(sec_menu_item==2)//����
                        {
                            Go=1;
                            menu_show_flag=0;
                        }
                        else if(sec_menu_item==3)
                        {
                            Go_Direction=!Go_Direction;//��������
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
        else if (KEY4_PRES == key)//���ĸ�������һ������
        {
            if (menu_deep == 1)
            {
                Go=1;
                ips200_clear();  //����
                menu_show_flag=!menu_show_flag;
            }
        }
        else if (KEY2_PRES == key)//��ߣ�һ������
        {
            if (menu_deep == 1)
            {
                Go=7;
                ips200_clear();  //����
                menu_show_flag=!menu_show_flag;
            }
        }

    }
}



