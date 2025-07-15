#include "zf_common_headfile.h"
#include "island.h"


#include "zf_common_headfile.h"
#include "Island.h"



//���û�������
extern uint8 image_two_value[MT9V03X_H][MT9V03X_W];//��ֵ�����ԭ����
extern volatile int Left_Line[MT9V03X_H]; //���������
extern volatile int Right_Line[MT9V03X_H];//�ұ�������
extern volatile int Search_Stop_Line;     //������ֹ��
extern volatile int Boundry_Start_Left;   //���ұ߽���ʼ��
extern volatile int Boundry_Start_Right;  //��һ���Ƕ��ߵ�,����߽���ʼ��
extern volatile int Left_Lost_Time;       //�߽綪����
extern volatile int Right_Lost_Time; 
extern volatile int Both_Lost_Time;//����ͬʱ������
extern volatile float Err;//����ͷ���

//ʮ�ֱ���
extern volatile int Cross_Flag; 

//��������
volatile int Island_State=0;     //����״̬��־
volatile int Left_Island_Flag=0; //���һ�����־
volatile int Right_Island_Flag=0;//���һ�����־
volatile int Left_Up_Guai[2];    //�ĸ��յ������洢��[0]��y����ĳ�У�{1}��x����ĳ��
volatile int Right_Up_Guai[2];   //�ĸ��յ������洢��[0]��y����ĳ�У�{1}��x����ĳ��

//������
extern volatile int Zebra_State;

//�µ���־λ
extern volatile int Ramp_Flag;//�µ���־


/*-------------------------------------------------------------------------------------------------------------------
  @brief     �������
  @param     null
  @return    null
  Sample     Island_Detect(void);
  @note      �����ĸ��յ��б����������Ըı亯����������˺�ѵ㣬��Ϊ8��
-------------------------------------------------------------------------------------------------------------------*/
void Island_Detect()
{ 
    static float k=0;                          //3��5״̬��k
    static int island_state_5_down[2]={0};     //״̬5ʱ�����뿪���������ұ߽����͵㣬[0]��y����ĳ�У�{1}��x����ĳ��
    static int island_state_3_up[2]={0};       //״̬3ʱ�������뻷���ã���������ǵ�[0]��y����ĳ�У�{1}��x����ĳ��
    static int left_down_guai[2]={0};          //�ĸ��յ������洢��[0]��y����ĳ�У�{1}��x����ĳ��
    static int right_down_guai[2]={0};         //�ĸ��յ������洢��[0]��y����ĳ�У�{1}��x����ĳ��
    int monotonicity_change_line[2];           //�����Ըı�����꣬[0]��ĳ�У�[1]��ĳ��
    int monotonicity_change_left_flag=0;       //��ת����0
    int monotonicity_change_right_flag=0;      //��ת����0
    int continuity_change_right_flag=0;        //������0
    int continuity_change_left_flag=0;         //������0

    //�����������Ա仯�ж�
    monotonicity_change_right_flag=Monotonicity_Change_Right(MT9V03X_H-1-10,10);
    monotonicity_change_left_flag=Monotonicity_Change_Left(MT9V03X_H-1-10,10);

    if(Cross_Flag==0&&Island_State==0&&Ramp_Flag==0)
    {
        if(Left_Island_Flag==0)  //���жϣ�������߽絥���Ա仯
        {
            //���ģ���߽���ֵ����Ա仯���Ҽ�⵽���¹յ�
            if(monotonicity_change_left_flag!=0)  //��߽絥���Ըı�
            {
                left_down_guai[0]=Find_Left_Down_Point(MT9V03X_H-1,20);  //�����½ǵ�
                if(left_down_guai[0]>=30)  //�յ�λ����Ч
                {
                    Island_State=1;
                    Left_Island_Flag=1;
                }
                else  //���У�����
                {
                    Island_State=0;
                    Left_Island_Flag=0;
                }
            }
        }

        if(Right_Island_Flag==0)  //�һ��жϣ������ұ߽絥���Ա仯
        {
            //���ģ��ұ߽���ֵ����Ա仯���Ҽ�⵽���¹յ�
            if(monotonicity_change_right_flag!=0)  //�ұ߽絥���Ըı�
            {
                right_down_guai[0]=Find_Right_Down_Point(MT9V03X_H-1,20);  //���µ�
                if(right_down_guai[0]>=30)  //�յ�λ����Ч
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

    if(Left_Island_Flag==1)  //�󻷵�״̬����ת
    {
        if(Island_State==1)
        {
            //������߽絥���Ա仯������
            monotonicity_change_line[0]=Monotonicity_Change_Left(30,5);  //��ȡ��߽絥���Ըı��
            monotonicity_change_line[1]=Left_Line[monotonicity_change_line[0]];
            Left_Add_Line((int)(monotonicity_change_line[1]*0.15),MT9V03X_H-1,monotonicity_change_line[1],monotonicity_change_line[0]);
            
            if((Island_State==1)&&(Boundry_Start_Left<30))  //�·�����ת״̬2
            {
                Island_State=2;
            }
        }

        else if(Island_State==2)
        {
            monotonicity_change_line[0]=Monotonicity_Change_Left(70,10);  //��߽絥���Ըı��
            monotonicity_change_line[1]=Left_Line[monotonicity_change_line[0]];
            Left_Add_Line((int)(monotonicity_change_line[1]*0.1),MT9V03X_H-1,monotonicity_change_line[1],monotonicity_change_line[0]);
            
            if(Island_State==2&&(Boundry_Start_Left>=MT9V03X_H-5||monotonicity_change_line[0]>50))
            {
                Island_State=3;
                Left_Island_Flag=1;
            }
        }

        else if(Island_State==3)  //׼��������������߽�յ��뵥����
        {
            if(k!=0)
            {
                K_Draw_Line(k,MT9V03X_W-30,MT9V03X_H-1,0);
                Longest_White_Column();
            }
            else
            {
                Left_Up_Guai[0]=Find_Left_Up_Point(40,5);  //���ϻ���
                Left_Up_Guai[1]=Left_Line[Left_Up_Guai[0]];

                if (Left_Up_Guai[0]<5)  //������
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

        else if(Island_State==4)  //�ѽ���������ұ߽絥���Ա仯
        {
            if(abs(gz)>200)
            {
                monotonicity_change_line[0]=Monotonicity_Change_Right(MT9V03X_H-10,10);  //�ұ߽絥���Ըı��
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

        else if(Island_State==5)  //����
        {
            K_Add_Boundry_Right(k,island_state_5_down[1],island_state_5_down[0],0);
            
            if((Island_State==5)&&(Boundry_Start_Right<MT9V03X_H-20))
            {
                Island_State=6;
            }
        }

        else if(Island_State==6)  //��������
        {
            K_Add_Boundry_Right(k,island_state_5_down[1],island_state_5_down[0],0);
            
            if((Island_State==6)&&((Boundry_Start_Right>MT9V03X_H-10)||(abs(gz)>=320)))
            {
                k=0;
                Island_State=7;
            }
        }

        else if(Island_State==7)  //����յ㣨������߽�������
        {
            Left_Up_Guai[0]=Find_Left_Up_Point(MT9V03X_H-10,0);
            Left_Up_Guai[1]=Left_Line[Left_Up_Guai[0]];
            
            if((Island_State==7)&&(Left_Up_Guai[1]<=100)&&(5<=Left_Up_Guai[0]&&Left_Up_Guai[0]<=MT9V03X_H-20))
            {
                Island_State=8;
            }
        }

        else if(Island_State==8)  //��������
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

    else if(Right_Island_Flag==1)  //�һ���״̬����ת
    {
        if(Island_State==1)
        {
            //�����ұ߽絥���Ա仯������
            monotonicity_change_line[0]=Monotonicity_Change_Right(30,5);  //�ұ߽絥���Ըı��
            monotonicity_change_line[1]=Right_Line[monotonicity_change_line[0]];
            Right_Add_Line((int)(MT9V03X_W-1-(monotonicity_change_line[1]*0.15)),MT9V03X_H-1,monotonicity_change_line[1],monotonicity_change_line[0]);
            
            if(Boundry_Start_Right<=30)//�����¹յ����״̬2
            {
                Island_State=2;
            }
        }

        else if(Island_State==2)
        {
            monotonicity_change_line[0]=Monotonicity_Change_Right(70,5);  //�ұ߽絥���Ըı��
            monotonicity_change_line[1]=Right_Line[monotonicity_change_line[0]];
            Right_Add_Line((int)(MT9V03X_W-1-(monotonicity_change_line[1]*0.15)),MT9V03X_H-1,monotonicity_change_line[1],monotonicity_change_line[0]);
            
            if(Island_State==2&&(Boundry_Start_Right>=MT9V03X_H-5||monotonicity_change_line[0]>50))
            {
                Island_State=3;
                Right_Island_Flag=1;
            }
        }

        else if(Island_State==3)  //׼�������������ұ߽�յ��뵥����
        {
            if(k!=0)
            {
                K_Draw_Line(k,30,MT9V03X_H-1,0);
                Longest_White_Column();
            }
            else
            {
                Right_Up_Guai[0]=Find_Right_Up_Point(40,10);  //���ϻ���
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

        else if(Island_State==4)  //�ѽ����������߽絥���Ա仯
        {
            monotonicity_change_line[0]=Monotonicity_Change_Left(90,10);  //��߽絥���Ըı��
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

        else if(Island_State==5)  //׼������
        {
            K_Add_Boundry_Left(k,island_state_5_down[1],island_state_5_down[0],0);
            
            if(Island_State==5&&Boundry_Start_Left<MT9V03X_H-20)
            {
                Island_State=6;
            }
        }

        else if(Island_State==6)  //��������
        {
            K_Add_Boundry_Left(k,island_state_5_down[1],island_state_5_down[0],0);
            
            if((Island_State==6)&&(Boundry_Start_Left>MT9V03X_H-10||abs(gz)>=400))
            {
                k=0;
                Island_State=7;
            }
        }

        else if(Island_State==7)  //���ҹյ㣨�����ұ߽�������
        {
            Right_Up_Guai[0]=Find_Right_Up_Point(MT9V03X_H-10,0);
            Right_Up_Guai[1]=Right_Line[Right_Up_Guai[0]];
            
            if((Island_State==7)&&((Right_Up_Guai[1]>=MT9V03X_W-88&&(5<=Right_Up_Guai[0]&&Right_Up_Guai[0]<=MT9V03X_H-20))))
            {
                Island_State=8;
            }
        }

        else if(Island_State==8)  //��������
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
  @brief     �����������Լ��
  @param     ��ʼ�㣬��ֹ��
  @return    ��������0�����������ض��߳�����
  Sample     Continuity_Change_Left(int start,int end);
  @note      �����Ե���ֵ����Ϊ5���ɸ���
-------------------------------------------------------------------------------------------------------------------*/
int Continuity_Change_Left(int start,int end)//��������ֵ����Ϊ5
{
    int i;
    int t;
    int continuity_change_flag=0;
    if(Left_Lost_Time>=0.9*MT9V03X_H)//�󲿷ֶ����ߣ�û��Ҫ�ж���
       return 1;
    if(Search_Stop_Line<=5)//������ֹ�кܰ�
       return 1;
    if(start>=MT9V03X_H-1-5)//����Խ�籣��
        start=MT9V03X_H-1-5;
    if(end<=5)
       end=5;
    if(start<end)//���Ǵ������ϼ���ģ����˾ͻ���һ��
    {
       t=start;
       start=end;
       end=t;
    }
    
    for(i=start;i>=end;i--)
    {
       if(abs(Left_Line[i]-Left_Line[i-1])>=5)//�����ж���ֵ��5,�ɸ���
       {
            continuity_change_flag=i;
            break;
       }
    }
    return continuity_change_flag;
}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     �����������Լ��
  @param     ��ʼ�㣬��ֹ��
  @return    ��������0�����������ض��߳�����
  Sample     continuity_change_flag=Continuity_Change_Right(int start,int end)
  @note      �����Ե���ֵ����Ϊ5���ɸ���
-------------------------------------------------------------------------------------------------------------------*/
int Continuity_Change_Right(int start,int end)
{
    int i;
    int t;
    int continuity_change_flag=0;
    if(Right_Lost_Time>=0.9*MT9V03X_H)//�󲿷ֶ����ߣ�û��Ҫ�ж���
       return 1;
    if(start>=MT9V03X_H-5)//����Խ�籣��
        start=MT9V03X_H-5;
    if(end<=5)
       end=5;
    if(start<end)//���Ǵ������ϼ���ģ����˾ͻ���һ��
    {
       t=start;
       start=end;
       end=t;
    }

    for(i=start;i>=end;i--)
    {
        if(abs(Right_Line[i]-Right_Line[i-1])>=5)//��������ֵ��5���ɸ���
       {
            continuity_change_flag=i;
            break;
       }
    }
    return continuity_change_flag;
}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     ���½ǵ���
  @param     ��ʼ�㣬��ֹ��
  @return    ���ؽǵ����ڵ��������Ҳ�������0
  Sample     Find_Left_Down_Point(int start,int end);
  @note      �ǵ�����ֵ�ɸ���ʵ��ֵ����
-------------------------------------------------------------------------------------------------------------------*/
int Find_Left_Down_Point(int start,int end)//���ĸ��ǵ㣬����ֵ�ǽǵ����ڵ�����
{
    int i,t;
    int left_down_line=0;
    if(Left_Lost_Time>=0.9*MT9V03X_H)//�󲿷ֶ����ߣ�û�йյ��жϵ�����
       return left_down_line;
    if(start<end)
    {
        t=start;
        start=end;
        end=t;
    }
    if(start>=MT9V03X_H-1-5)//����5�����ݲ��ȶ���������Ϊ�߽�����жϣ�����
        start=MT9V03X_H-1-5;
    if(end<=MT9V03X_H-Search_Stop_Line)
        end=MT9V03X_H-Search_Stop_Line;
    if(end<=5)
       end=5;
    for(i=start;i>=end;i--)
    {
        if(left_down_line==0&&//ֻ�ҵ�һ�����������ĵ�
           abs(Left_Line[i]-Left_Line[i+1])<=5&&//�ǵ����ֵ���Ը���
           abs(Left_Line[i+1]-Left_Line[i+2])<=5&&  
           abs(Left_Line[i+2]-Left_Line[i+3])<=5&&
              (Left_Line[i]-Left_Line[i-2])>=5&&
              (Left_Line[i]-Left_Line[i-3])>=10&&
              (Left_Line[i]-Left_Line[i-4])>=10)
        {
            left_down_line=i;//��ȡ��������
            break;
        }
    }
    return left_down_line;
}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     ���Ͻǵ���
  @param     ��ʼ�㣬��ֹ��
  @return    ���ؽǵ����ڵ��������Ҳ�������0
  Sample     Find_Left_Up_Point(int start,int end);
  @note      �ǵ�����ֵ�ɸ���ʵ��ֵ����
-------------------------------------------------------------------------------------------------------------------*/
int Find_Left_Up_Point(int start,int end)//���ĸ��ǵ㣬����ֵ�ǽǵ����ڵ�����
{
    int i,t;
    int left_up_line=0;
    if(Left_Lost_Time>=0.9*MT9V03X_H)//�󲿷ֶ����ߣ�û�йյ��жϵ�����
       return left_up_line;
    if(start<end)
    {
        t=start;
        start=end;
        end=t;
    }
    if(end<=MT9V03X_H-Search_Stop_Line)//������ֹ�����ϵ�ȫ������
        end=MT9V03X_H-Search_Stop_Line;
    if(end<=5)//��ʱ����зǳ�����ҲҪ�������ֵ㣬��ֹ����Խ��
        end=5;
    if(start>=MT9V03X_H-1-5)
        start=MT9V03X_H-1-5;
    for(i=start;i>=end;i--)
    {
        if(left_up_line==0&&//ֻ�ҵ�һ�����������ĵ�
           abs(Left_Line[i]-Left_Line[i-1])<=5&&
           abs(Left_Line[i-1]-Left_Line[i-2])<=5&&  
           abs(Left_Line[i-2]-Left_Line[i-3])<=5&&
              (Left_Line[i]-Left_Line[i+2])>=8&& 
              (Left_Line[i]-Left_Line[i+3])>=15&&
              (Left_Line[i]-Left_Line[i+4])>=15)
        {
            left_up_line=i;//��ȡ��������
            break;
        }
    }
    return left_up_line;//�����MT9V03X_H-1��˵��û����ô���յ�
}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     ���½ǵ���
  @param     ��ʼ�㣬��ֹ��
  @return    ���ؽǵ����ڵ��������Ҳ�������0
  Sample     Find_Right_Down_Point(int start,int end);
  @note      �ǵ�����ֵ�ɸ���ʵ��ֵ����
-------------------------------------------------------------------------------------------------------------------*/
int Find_Right_Down_Point(int start,int end)//���ĸ��ǵ㣬����ֵ�ǽǵ����ڵ�����
{
    int i,t;
    int right_down_line=0;
    if(Right_Lost_Time>=0.9*MT9V03X_H)//�󲿷ֶ����ߣ�û�йյ��жϵ�����
        return right_down_line;
    if(start<end)
    {
        t=start;
        start=end;
        end=t;
    }
    if(start>=MT9V03X_H-1-5)//����5�����ݲ��ȶ���������Ϊ�߽�����жϣ�����
        start=MT9V03X_H-1-5;
    if(end<=MT9V03X_H-Search_Stop_Line)
        end=MT9V03X_H-Search_Stop_Line;
    if(end<=5)
       end=5;
    for(i=start;i>=end;i--)
    {
        if(right_down_line==0&&//ֻ�ҵ�һ�����������ĵ�
           abs(Right_Line[i]-Right_Line[i+1])<=5&&//�ǵ����ֵ���Ը���
           abs(Right_Line[i+1]-Right_Line[i+2])<=5&&  
           abs(Right_Line[i+2]-Right_Line[i+3])<=5&&
              (Right_Line[i]-Right_Line[i-2])<=-5&&
              (Right_Line[i]-Right_Line[i-3])<=-10&&
              (Right_Line[i]-Right_Line[i-4])<=-10)
        {
            right_down_line=i;//��ȡ��������
            break;
        }
    }
    return right_down_line;
}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     ���Ͻǵ���
  @param     ��ʼ�㣬��ֹ��
  @return    ���ؽǵ����ڵ��������Ҳ�������0
  Sample     Find_Right_Up_Point(int start,int end);
  @note      �ǵ�����ֵ�ɸ���ʵ��ֵ����
-------------------------------------------------------------------------------------------------------------------*/
int Find_Right_Up_Point(int start,int end)//���ĸ��ǵ㣬����ֵ�ǽǵ����ڵ�����
{
    int i,t;
    int right_up_line=0;
    if(Right_Lost_Time>=0.9*MT9V03X_H)//�󲿷ֶ����ߣ�û�йյ��жϵ�����
        return right_up_line;
    if(start<end)
    {
        t=start;
        start=end;
        end=t;
    }    
    if(end<=MT9V03X_H-Search_Stop_Line)//������ֹ�����ϵ�ȫ������
        end=MT9V03X_H-Search_Stop_Line;
    if(end<=5)//��ʱ����зǳ�����ҲҪ�������ֵ㣬��ֹ����Խ��
        end=5;
    if(start>=MT9V03X_H-1-5)
        start=MT9V03X_H-1-5;
    for(i=start;i>=end;i--)
    {
        if(right_up_line==0&&//ֻ�ҵ�һ�����������ĵ�
           abs(Right_Line[i]-Right_Line[i-1])<=5&&//��������λ�ò��
           abs(Right_Line[i-1]-Right_Line[i-2])<=5&&  
           abs(Right_Line[i-2]-Right_Line[i-3])<=5&&
              (Right_Line[i]-Right_Line[i+2])<=-8&&
              (Right_Line[i]-Right_Line[i+3])<=-15&&
              (Right_Line[i]-Right_Line[i+4])<=-15)
        {
            right_up_line=i;//��ȡ��������
            break;
        }
    }
    return right_up_line;
}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     ������ͻ����
  @param     ��ʼ�㣬��ֹ��
  @return    �����ڵ��������Ҳ�������0
  Sample     Find_Right_Up_Point(int start,int end);
  @note      ǰ5��5�������С�����������ǽǵ�
-------------------------------------------------------------------------------------------------------------------*/
int Monotonicity_Change_Left(int start,int end)//�����Ըı䣬����ֵ�ǵ����Ըı�����ڵ�����
{
    int i;
    int monotonicity_change_line=0;
    if(Left_Lost_Time>=0.9*MT9V03X_H)//�󲿷ֶ����ߣ�û�е������жϵ�����
       return monotonicity_change_line;
    if(start>=MT9V03X_H-1-5)//����Խ�籣�������жϵ�i����ʱ
       start=MT9V03X_H-1-5; //Ҫ������ǰ��5���㣬������ͷ�ĵ�Ҫ������Ϊ����յ�
    if(end<=5)
        end=5;
    if(start<=end)//�ݼ����㣬��ڷ��ˣ�ֱ�ӷ���0
      return monotonicity_change_line;
    for(i=start;i>=end;i--)//���ȡǰ5��5���ݣ�����ǰ������뷶Χ��Ҫ��
    {
        if(Left_Line[i]==Left_Line[i+5]&&Left_Line[i]==Left_Line[i-5]&&
        Left_Line[i]==Left_Line[i+4]&&Left_Line[i]==Left_Line[i-4]&&
        Left_Line[i]==Left_Line[i+3]&&Left_Line[i]==Left_Line[i-3]&&
        Left_Line[i]==Left_Line[i+2]&&Left_Line[i]==Left_Line[i-2]&&
        Left_Line[i]==Left_Line[i+1]&&Left_Line[i]==Left_Line[i-1])
        {//һ������һ������Ȼ������Ϊ����ת�۵�
            continue;
        }
        else if(Left_Line[i]>=Left_Line[i+5]&&Left_Line[i]>=Left_Line[i-5]&&
        Left_Line[i]>=Left_Line[i+4]&&Left_Line[i]>=Left_Line[i-4]&&
        Left_Line[i]>=Left_Line[i+3]&&Left_Line[i]>=Left_Line[i-3]&&
        Left_Line[i]>=Left_Line[i+2]&&Left_Line[i]>=Left_Line[i-2]&&
        Left_Line[i]>=Left_Line[i+1]&&Left_Line[i]>=Left_Line[i-1])
        {//�ͺܱ����������������ǰ5����5�����ģ�����ȡ�ȣ����Ǿ��ǵ���ͻ���
            monotonicity_change_line=i;
            break;
        }
    }
    return monotonicity_change_line;
}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     ������ͻ����
  @param     ��ʼ�㣬��ֹ��
  @return    �����ڵ��������Ҳ�������0
  Sample     Find_Right_Up_Point(int start,int end);
  @note      ǰ5��5�������С�����������ǽǵ�
-------------------------------------------------------------------------------------------------------------------*/
int Monotonicity_Change_Right(int start,int end)//�����Ըı䣬����ֵ�ǵ����Ըı�����ڵ�����
{
    int i;
    int monotonicity_change_line=0;

    if(Right_Lost_Time>=0.9*MT9V03X_H)//�󲿷ֶ����ߣ�û�е������жϵ�����
        return monotonicity_change_line;
    if(start>=MT9V03X_H-1-5)//����Խ�籣��
        start=MT9V03X_H-1-5;
     if(end<=5)
        end=5;
    if(start<=end)
        return monotonicity_change_line;
    for(i=start;i>=end;i--)//���ȡǰ5��5���ݣ�����ǰ������뷶Χ��Ҫ��
    {
        if(Right_Line[i]==Right_Line[i+5]&&Right_Line[i]==Right_Line[i-5]&&
        Right_Line[i]==Right_Line[i+4]&&Right_Line[i]==Right_Line[i-4]&&
        Right_Line[i]==Right_Line[i+3]&&Right_Line[i]==Right_Line[i-3]&&
        Right_Line[i]==Right_Line[i+2]&&Right_Line[i]==Right_Line[i-2]&&
        Right_Line[i]==Right_Line[i+1]&&Right_Line[i]==Right_Line[i-1])
        {//һ������һ������Ȼ������Ϊ����ת�۵�
            continue;
        }
        else if(Right_Line[i]<=Right_Line[i+5]&&Right_Line[i]<=Right_Line[i-5]&&
        Right_Line[i]<=Right_Line[i+4]&&Right_Line[i]<=Right_Line[i-4]&&
        Right_Line[i]<=Right_Line[i+3]&&Right_Line[i]<=Right_Line[i-3]&&
        Right_Line[i]<=Right_Line[i+2]&&Right_Line[i]<=Right_Line[i-2]&&
        Right_Line[i]<=Right_Line[i+1]&&Right_Line[i]<=Right_Line[i-1])
        {//�ͺܱ����������������ǰ5����5�����ģ��Ǿ��ǵ���ͻ���
            monotonicity_change_line=i;
            break;
        }
    }
    return monotonicity_change_line;
}



/*-------------------------------------------------------------------------------------------------------------------
  @brief     ͨ��б�ʣ����㲹��--
  @param     k       ����б��
             startY  ������ʼ��������
             endY    ������������
  @return    null
  Sample     K_Add_Boundry_Left(float k,int startY,int endY);
  @note      ������ֱ�����ڱ�����
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
    if(startY<endY)//--������start��Ҫ��
    {
        t=startY;
        startY=endY;
        endY=t;
    }
    for(i=startY;i>=endY;i--)
    {
        Left_Line[i]=(int)((i-startY)/k+startX);//(y-y1)=k(x-x1)���Σ�x=(y-y1)/k+x1
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
  @brief     ͨ��б�ʣ����㲹��
  @param     k       ����б��
             startY  ������ʼ��������
             endY    ������������
  @return    null    ֱ�Ӳ�����
  Sample     K_Add_Boundry_Right(float k,int startY,int endY);
  @note      ������ֱ�����ڱ�����
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
        Right_Line[i]=(int)((i-startY)/k+startX);//(y-y1)=k(x-x1)���Σ�x=(y-y1)/k+x1
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
  @brief     ����б�ʻ���
  @param     ����б�ʣ����㣬��һ������
  @return    null
  Sample     K_Draw_Line(k, 20,MT9V03X_H-1 ,0)
  @note      ���ľ���һ���ߣ���Ҫ����ɨ��
-------------------------------------------------------------------------------------------------------------------*/
void K_Draw_Line(float k, int startX, int startY,int endY)
{
    int endX=0;

    if(startX>=MT9V03X_W-1)//�޷�����
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
    endX=(int)((endY-startY)/k+startX);//(y-y1)=k(x-x1)���Σ�x=(y-y1)/k+x1
    Draw_Line(startX,startY,endX,endY);
}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     ��ȡƽ������
  @param     int start_line,int end_line����ʼ�У���ֹ��
  @return    �⼸������ƽ��ֵ
  Sample     road_wide=Get_Road_Wide(68,69);
  @note      ++���㣬����Ѱ�ң����ƽ������
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
    if(start_line>end_line)//++���ʣ����귴�˻���
    {
        t=start_line;
        start_line=end_line;
        end_line=t;
    }
    for(i=start_line;i<=end_line;i++)
    {
        road_wide+=Right_Line[i]-Left_Line[i];
    }
    road_wide=road_wide/(end_line-start_line+1);//ƽ������
    return road_wide;
}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     ��ȡ�������߽�б��
  @param     int start_line,int end_line����ʼ�У���ֹ��
  @return    ����֮���б��
  Sample     k=Get_Left_K(68,69);
  @note      ����֮��ó�б�ʣ�Ĭ�ϵ�һ������С���ڶ���������
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
    if(start_line>end_line)//++���ʣ����귴�˻���
    {
        t=start_line;
        start_line=end_line;
        end_line=t;
    }
    k=(float)(((float)Left_Line[start_line]-(float)Left_Line[end_line])/(end_line-start_line+1));
    return k;
}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     ��ȡ�������߽�б��
  @param     int start_line,int end_line����ʼ�У���ֹ��
  @return    ����֮���б��
  Sample     k=Get_Right_K(68,69);
  @note      ����֮��ó�б�ʣ�Ĭ�ϵ�һ������С���ڶ���������
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
    if(start_line>end_line)//++���ʣ����귴�˻���
    {
        t=start_line;
        start_line=end_line;
        end_line=t;
    }
    k=(float)(((float)Right_Line[start_line]-(float)Right_Line[end_line])/(end_line-start_line+1));
    return k;
}













