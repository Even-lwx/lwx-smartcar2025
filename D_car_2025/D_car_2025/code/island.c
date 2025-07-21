#include "zf_common_headfile.h"
#include "island.h"

// ���û�������
extern uint8 image_two_value[MT9V03X_H][MT9V03X_W]; // ��ֵ�����ԭ����
extern volatile int Left_Line[MT9V03X_H];           // ���������
extern volatile int Right_Line[MT9V03X_H];          // �ұ�������
extern volatile int Search_Stop_Line;               // ������ֹ��
extern volatile int Boundry_Start_Left;             // ���ұ߽���ʼ��
extern volatile int Boundry_Start_Right;            // ��һ���Ƕ��ߵ�,����߽���ʼ��
extern volatile int Left_Lost_Time;                 // �߽綪����
extern volatile int Right_Lost_Time;
extern volatile int Both_Lost_Time; // ����ͬʱ������
extern volatile float Err;          // ����ͷ���

// ʮ�ֱ���
extern volatile int Cross_Flag;

// ��������
volatile int Island_State = 0;      // ����״̬��־
volatile int Left_Island_Flag = 0;  // ���һ�����־
volatile int Right_Island_Flag = 0; // ���һ�����־
volatile int Left_Up_Guai[2];       // �ĸ��յ������洢��[0]��y����ĳ�У�{1}��x����ĳ��
volatile int Right_Up_Guai[2];      // �ĸ��յ������洢��[0]��y����ĳ�У�{1}��x����ĳ��

// ������
extern volatile int Zebra_State;

// �µ���־λ
extern volatile int Ramp_Flag; // �µ���־

///*-------------------------------------------------------------------------------------------------------------------
//  @brief     �������
//  @param     null
//  @return    null
//  Sample     Island_Detect(void);
//  @note      �������ϵĿ�Դ�����޸ĵ�
//-------------------------------------------------------------------------------------------------------------------*/
// void Island_Detect()
//{
//    static float k = 0;                      // 3��5״̬��k
//    static int island_state_5_down[2] = {0}; // ״̬5ʱ�����뿪���������ұ߽����͵㣬[0]��y����ĳ�У�{1}��x����ĳ��
//    static int island_state_3_up[2] = {0};   // ״̬3ʱ�������뻷���ã���������ǵ�[0]��y����ĳ�У�{1}��x����ĳ��
//    static int left_down_guai[2] = {0};      // �ĸ��յ������洢��[0]��y����ĳ�У�{1}��x����ĳ��
//    static int right_down_guai[2] = {0};     // �ĸ��յ������洢��[0]��y����ĳ�У�{1}��x����ĳ��
//    int monotonicity_change_line[2];         // �����Ըı�����꣬[0]��ĳ�У�[1]��ĳ��
//    int monotonicity_change_left_flag = 0;   // ��ת����0
//    int monotonicity_change_right_flag = 0;  // ��ת����0
//    int continuity_change_right_flag = 0;    // ������0
//    int continuity_change_left_flag = 0;     // ������0

//    if (Cross_Flag == 0 && Island_State == 0 && Ramp_Flag == 0) //&&Ramp_Flag==0
//    {
//        continuity_change_left_flag = Continuity_Change_Left(MT9V03X_H - 1 - 15, 15); // �������ж�
//        continuity_change_right_flag = Continuity_Change_Right(MT9V03X_H - 1 - 15, 15);

//        monotonicity_change_left_flag = Monotonicity_Change_Left(MT9V03X_H - 1 - 15, 15);
//			  monotonicity_change_right_flag = Monotonicity_Change_Right(MT9V03X_H - 1 - 15, 15);

//        if (Left_Island_Flag == 0) // ��
//        {
//            if (
//								monotonicity_change_right_flag == 0 &&                                      // �ұ��ǵ�����
//                continuity_change_left_flag != 0 &&                                         // ����ǲ�������
//                continuity_change_right_flag == 0 &&                                        // �󻷵��ұ���������
//               Left_Lost_Time >= 10 &&                                                     // ��߶��ߺܶ�
//               Left_Lost_Time <= 100 &&                                                    // Ҳ����ȫ����
//                Right_Lost_Time <= 10 &&                                                    // �ұ߶��߽���
//                Search_Stop_Line >= 100 &&                                                  // ������ֹ�п�����Զ
//                Boundry_Start_Left >= MT9V03X_H - 8 && Boundry_Start_Right >= MT9V03X_H - 8 // �߽���ʼ�㿿��
//                )                                                                           // ˫�߶�����
//            {
//
//                    Island_State = 1;
//                    Left_Island_Flag = 1;
//                }
//                else // ���У�����
//                {
//                    Island_State = 0;
//                    Left_Island_Flag = 0;
//                }
//            }
//        }
//        if (Right_Island_Flag == 0) // �һ�
//        {
//            if (
//								monotonicity_change_right_flag == 1 &&
//                continuity_change_left_flag <=30 &&                                         // �һ��������������
//                continuity_change_right_flag != 0 &&                                        // �ұ��ǲ�������
//                Right_Lost_Time >= 10 &&                                                    // �Ҷ��߶�
//                Right_Lost_Time <= 100 &&                                                   // �Ҷ��߲���̫��
//               Left_Lost_Time <= 10 &&                                                     // ������
//                Search_Stop_Line >= 100 &&                                                  // ������ֹ�п�����Զ
//                Boundry_Start_Left >= MT9V03X_H - 10 && Boundry_Start_Right >= MT9V03X_H - 10 // �߽���ʼ�㿿��
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
//    if (Left_Island_Flag == 1) // 1״̬�¹յ㻹�ڣ�û����
//    {
//        if (Island_State == 1)
//        {
//            monotonicity_change_line[0] = Monotonicity_Change_Left(30, 5); // Ѱ�ҵ����Ըı��
//            monotonicity_change_line[1] = Left_Line[monotonicity_change_line[0]];
//            Left_Add_Line((int)(monotonicity_change_line[1] * 0.15), MT9V03X_H - 1, monotonicity_change_line[1], monotonicity_change_line[0]);
//            if ((Island_State == 1) && (Boundry_Start_Left < 30)) // �·�������ʱ���2
//            {

//                Island_State = 2;
//            }
//        }

//        else if (Island_State == 2) // �·��ǵ���ʧ��2״̬ʱ�·�Ӧ���Ƕ��ߣ������ǻ���
//        {
//            monotonicity_change_line[0] = Monotonicity_Change_Left(70, 10); // Ѱ�ҵ����Ըı��
//            monotonicity_change_line[1] = Left_Line[monotonicity_change_line[0]];
//            Left_Add_Line((int)(monotonicity_change_line[1] * 0.1), MT9V03X_H - 1, monotonicity_change_line[1], monotonicity_change_line[0]);
//            if (Island_State == 2 && (Boundry_Start_Left >= MT9V03X_H - 5 || monotonicity_change_line[0] > 50)) // ��Բ������ʱ�򣬽�3
//            {

//                Island_State = 3; // �����Ѱ�ҷ�ΧҲҪ�ģ���camera.c
//                Left_Island_Flag = 1;
//            }
//        }
//        else if (Island_State == 3) // 3״̬׼��������Ѱ���Ϲյ㣬����
//        {
//            if (k != 0)
//            {
//                K_Draw_Line(k, MT9V03X_W - 30, MT9V03X_H - 1, 0); // k�Ǹո�������ģ���̬��������
//                Longest_White_Column();                           // ˢ�±߽�����
//            }
//            else
//            {
//                Left_Up_Guai[0] = Find_Left_Up_Point(40, 5); // �����Ϲյ�
//                Left_Up_Guai[1] = Left_Line[Left_Up_Guai[0]];

//                if (Left_Up_Guai[0] < 5) // �˴�Ϊ�˷�ֹ���У����������3״̬���㣬�����޸Ĵ˴��ж�����
//                {
//                    Island_State = 0;
//                    Left_Island_Flag = 0;
//                }

//                if (k == 0 && (15 <= Left_Up_Guai[0] && Left_Up_Guai[0] < 50) && (50 < Left_Up_Guai[1] && Left_Up_Guai[1] < 110)) // �յ������һ����Χ�ڣ���Ϊ�ǹյ����
//                {
//                    island_state_3_up[0] = Left_Up_Guai[0];
//                    island_state_3_up[1] = Left_Up_Guai[1];
//                    k = (float)((float)(MT9V03X_H - island_state_3_up[0]) / (float)(MT9V03X_W - 20 - island_state_3_up[1]));
//                    K_Draw_Line(k, MT9V03X_W - 30, MT9V03X_H - 1, 0); // ��¼�µ�һ���ϵ����ʱλ�ã�������������һ�����ߣ��뻷
//                    Longest_White_Column();                           // ˢ�±߽�����
//                }
//            }
//            if ((Island_State == 3) && (abs(gz) >= 60)) // ���������ǻ����뻷
//            {
//                k = 0;                  // б������
//                Island_State = 4;       // ��һ��ʱ��˳�����������е�������Χ
//                Longest_White_Column(); // �����
//            }
//        }
//        else if (Island_State == 4) // ״̬4�Ѿ�������
//        {
//            if (abs(gz) > 200) // ����200���Ժ��ڴ򿪳����ж�
//            {
//                monotonicity_change_line[0] = Monotonicity_Change_Right(MT9V03X_H - 10, 10); // �����Ըı�
//                monotonicity_change_line[1] = Right_Line[monotonicity_change_line[0]];
//                if ((Island_State == 4) && (35 <= monotonicity_change_line[0] && monotonicity_change_line[0] <= 55 && monotonicity_change_line[1] >= 10)) // �����㿿�£���ȥ5
//                {                                                                                                                                         // monotonicity_change_line[1]>=90&&
//                    island_state_5_down[0] = MT9V03X_H - 1;
//                    island_state_5_down[1] = Right_Line[MT9V03X_H - 1];
//                    k = (float)((float)(MT9V03X_H - monotonicity_change_line[0]) / (float)(island_state_5_down[1] - monotonicity_change_line[1]));
//                    K_Add_Boundry_Right(k, island_state_5_down[1], island_state_5_down[0], 0); // ��״̬3һ������סб��
//                    Island_State = 5;
//                }
//            }
//        }
//        else if (Island_State == 5) // ����
//        {
//            K_Add_Boundry_Right(k, island_state_5_down[1], island_state_5_down[0], 0);
//            if ((Island_State == 5) && (Boundry_Start_Right < MT9V03X_H - 20)) // �ұ��ȶ���
//            {
//                Island_State = 6;
//            }
//        }
//        else if (Island_State == 6) // ���ڳ�
//        {
//            K_Add_Boundry_Right(k, island_state_5_down[1], island_state_5_down[0], 0);
//            if ((Island_State == 6) && ((Boundry_Start_Right > MT9V03X_H - 10) || (abs(gz) >= 320))) // �ұ߲�����
//            {                                                                                        //
//                k = 0;
//                Island_State = 7;
//            }
//        }
//        else if (Island_State == 7) // ������ȥ�ˣ���Ѱ�ҹյ㣬׼���뿪����״̬
//        {
//            Left_Up_Guai[0] = Find_Left_Up_Point(MT9V03X_H - 10, 0); // ��ȡ���ϵ����꣬��������ȥ8
//            Left_Up_Guai[1] = Left_Line[Left_Up_Guai[0]];
//            if ((Island_State == 7) && (Left_Up_Guai[1] <= 100) && (5 <= Left_Up_Guai[0] && Left_Up_Guai[0] <= MT9V03X_H - 20)) // ע������Ժ������궼��Ҫ��
//            {
//                Island_State = 8; // �������ҵ��յ��ȥ8
//            }
//        }
//        else if (Island_State == 8) // ���ߣ��������һ��
//        {
//            Left_Up_Guai[0] = Find_Left_Up_Point(MT9V03X_H - 1, 10); // ��ȡ���ϵ�����
//            Left_Up_Guai[1] = Left_Line[Left_Up_Guai[0]];
//            Lengthen_Left_Boundry(Left_Up_Guai[0] - 1, MT9V03X_H - 1);
//            if ((Island_State == 8) && (Left_Up_Guai[0] >= MT9V03X_H - 20 || (Left_Up_Guai[0] < 10 && Boundry_Start_Left >= MT9V03X_H - 10))) // ���յ㿿��ʱ����Ϊ�����ˣ���������
//            {                                                                                                                                 // Ҫô�յ㿿�£�Ҫô�յ㶪�ˣ����·������ߣ���Ϊ����������
//                gz = 0;                                                                                                                       // ��������
//                Island_State = 9;                                                                                                             // 8ʱ�򻷵����������ˣ�Ϊ�˷�ֹ�����л���8����9����ż�ʮ�������㣬
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
//        if (Island_State == 1) // 1״̬�¹յ㻹�ڣ�û����
//        {
//            monotonicity_change_line[0] = Monotonicity_Change_Right(MT9V03X_H - 1 - 15, 15); // �����Ըı�
//            monotonicity_change_line[1] = Right_Line[monotonicity_change_line[0]];
//
//            Right_Add_Line((int)(MT9V03X_W - 1 - (monotonicity_change_line[1] * 0.15)), MT9V03X_H - 1, monotonicity_change_line[1], monotonicity_change_line[0]); // �ұ߲�����λ�õ���
//
//            if (monotonicity_change_line[0] > 50)                                                                                                                 // ���½��ȶ���
//            {
//
//                Island_State = 2;
//            }
//        }

//        else if (Island_State == 2) // �����Ѿ����ִ��ߣ����Ϸ����ֽǵ�
//        {
//
//            if (k != 0) // ���ǵ����·����ӣ���һ������
//            {
//								buzzer_on(50);
//                K_Draw_Line(k, 30, MT9V03X_H - 1, 0);
//                Longest_White_Column(); // ˢ�������
//            }
//            else
//            {
//                Right_Up_Guai[0] = Find_Right_Up_Point(MT9V03X_H - 1 - 15, 15); // �����Ϲյ�
//                Right_Up_Guai[1] = Right_Line[Right_Up_Guai[0]];

//

//                if (k == 0 && (15 <= Right_Up_Guai[0] && Right_Up_Guai[0] < MT9V03X_H - 1 - 15) && (70 < Right_Up_Guai[1] && Right_Up_Guai[1] < 150)) // �ҵ�һ�����������Ľǵ㣬����
//                {
//                    island_state_3_up[0] = Right_Up_Guai[0];
//                    island_state_3_up[1] = Right_Up_Guai[1];
//                    k = (float)((float)(MT9V03X_H - island_state_3_up[0]) / (float)(20 - island_state_3_up[1]));
//                    K_Draw_Line(k, 30, MT9V03X_H - 1, 0);
//										system_lastcount=system_count;
//                    Longest_White_Column(); // ˢ����������
//                }
//            }
//
//            if ((Island_State == 2) &&k!=0&&	system_count-system_lastcount>100)
//            {
//                k = 0; // б������
//                Island_State = 3;
//                Longest_White_Column(); // �����
//            } // �ǵ�ȥ������Ǳ߸�һ�£����������һ���
//        }
//        else if (Island_State == 3) // 4״̬��ȫ��ȥ������
//        {
//            if (gz > 200) // ��������200�Ⱥ��ٴ򿪵���ת���ж�
//            {
//                monotonicity_change_line[0] = Monotonicity_Change_Left(90, 10); // �����Ըı�
//                monotonicity_change_line[1] = Left_Line[monotonicity_change_line[0]];
//                if ((Island_State == 4) && (35 <= monotonicity_change_line[0] && monotonicity_change_line[0] <= 55 && monotonicity_change_line[1] < MT9V03X_W - 10)) // �����㿿�£���ȥ5
//                {                                                                                                                                                    // monotonicity_change_line[1]<=120&&
//                    island_state_5_down[0] = MT9V03X_H - 1;
//                    island_state_5_down[1] = Left_Line[MT9V03X_H - 1] - 5; // ץס��һ�γ��ֵ�б�ʣ�����
//                    k = (float)((float)(MT9V03X_H - monotonicity_change_line[0]) / (float)(island_state_5_down[1] - monotonicity_change_line[1]));
//                    K_Add_Boundry_Left(k, island_state_5_down[1], island_state_5_down[0], 0);
//                    Island_State = 5;
//                }
//            }
//        }
//        else if (Island_State == 5) // ׼��������
//        {
//            K_Add_Boundry_Left(k, island_state_5_down[1], island_state_5_down[0], 0);
//            if (Island_State == 5 && Boundry_Start_Left < MT9V03X_H - 20) // ����ȶ���
//            {
//                Island_State = 6;
//            }
//        }
//        else if (Island_State == 6) // ������
//        {
//            K_Add_Boundry_Left(k, island_state_5_down[1], island_state_5_down[0], 0);
//            if ((Island_State == 6) && (Boundry_Start_Left > MT9V03X_H - 10 || abs(gz) >= 320)) // ����ȶ���
//            {                                                                                   //
//                k = 0;
//                Island_State = 7;
//            }
//        }
//        else if (Island_State == 7) // �������������ҽǵ�
//        {
//            Right_Up_Guai[0] = Find_Right_Up_Point(MT9V03X_H - 10, 0); // ��ȡ���ϵ����꣬�ҵ���ȥ8
//            Right_Up_Guai[1] = Right_Line[Right_Up_Guai[0]];
//            if ((Island_State == 7) && ((Right_Up_Guai[1] >= MT9V03X_W - 88 && (5 <= Right_Up_Guai[0] && Right_Up_Guai[0] <= MT9V03X_H - 20)))) // ע������Ժ������궼��Ҫ����Ϊ������һ������������ֹյ�
//            {                                                                                                                                   // ���ǵ�λ�ú���ʱ����8
//                Island_State = 8;
//            }
//        }
//        else if (Island_State == 8) // ����8
//        {
//            Right_Up_Guai[0] = Find_Right_Up_Point(MT9V03X_H - 1, 10); // ��ȡ���ϵ�����
//            Right_Up_Guai[1] = Right_Line[Right_Up_Guai[0]];
//            Lengthen_Right_Boundry(Right_Up_Guai[0] - 1, MT9V03X_H - 1);
//            if ((Island_State == 8) && (Right_Up_Guai[0] >= MT9V03X_H - 20 || (Right_Up_Guai[0] < 10 && Boundry_Start_Left >= MT9V03X_H - 10))) // ���յ㿿��ʱ����Ϊ�����ˣ���������
//            {                                                                                                                                   // �ǵ㿿�£������¶˲����ߣ���Ϊ������
//                gz = 0;
//                Island_State = 9;
//                Right_Island_Flag = 0;
//            }
//        }
//    }

//    //    if(Err>=0)
//    //    {
//    //       Flag_Show_101(MT9V03X_W,image_two_value,(uint8)(uint8)Err/10);//�����Ǹ�����
//    //       Flag_Show_102(MT9V03X_W,image_two_value,(uint8)((uint8)Err%10));//���������
//    //    }
//    //    else
//    //    {
//    //        Flag_Show_101(MT9V03X_W,image_two_value,(uint8)(uint8)-Err/10);//�����Ǹ�����
//    //        Flag_Show_102(MT9V03X_W,image_two_value,(uint8)((uint8)-Err%10));//���������
//    //    }

//    //    ips200_showfloat(0,12,gz,3,3);
//    //    ips200_showuint16(0,11,Island_State);
//}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     �����������Լ��
  @param     ��ʼ�㣬��ֹ��
  @return    ��������0�����������ض��߳�����
  Sample     Continuity_Change_Left(int start,int end);
  @note      �����Ե���ֵ����Ϊ5���ɸ���
-------------------------------------------------------------------------------------------------------------------*/
int Continuity_Change_Left(int start, int end) // ��������ֵ����Ϊ5
{
    int i;
    int t;
    int continuity_change_flag = 0;
    if (Left_Lost_Time >= 0.9 * MT9V03X_H) // �󲿷ֶ����ߣ�û��Ҫ�ж���
        return 1;
    if (Search_Stop_Line <= 5) // ������ֹ�кܰ�
        return 1;
    if (start >= MT9V03X_H - 1 - 5) // ����Խ�籣��
        start = MT9V03X_H - 1 - 5;
    if (end <= 5)
        end = 5;
    if (start < end) // ���Ǵ������ϼ���ģ����˾ͻ���һ��
    {
        t = start;
        start = end;
        end = t;
    }

    for (i = start; i >= end; i--)
    {
        if (abs(Left_Line[i] - Left_Line[i - 1]) >= 3) // �����ж���ֵ��3,�ɸ���
        {
            continuity_change_flag = i;
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
int Continuity_Change_Right(int start, int end)
{
    int i;
    int t;
    int continuity_change_flag = 0;
    if (Right_Lost_Time >= 0.9 * MT9V03X_H) // �󲿷ֶ����ߣ�û��Ҫ�ж���
        return 1;
    if (start >= MT9V03X_H - 5) // ����Խ�籣��
        start = MT9V03X_H - 5;
    if (end <= 5)
        end = 5;
    if (start < end) // ���Ǵ������ϼ���ģ����˾ͻ���һ��
    {
        t = start;
        start = end;
        end = t;
    }

    for (i = start; i >= end; i--)
    {
        if (abs(Right_Line[i] - Right_Line[i - 1]) >= 3) // ��������ֵ��3���ɸ���
        {
            continuity_change_flag = i;
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
int Find_Left_Down_Point(int start, int end) // ���ĸ��ǵ㣬����ֵ�ǽǵ����ڵ�����
{
    int i, t;
    int left_down_line = 0;
    if (Left_Lost_Time >= 0.9 * MT9V03X_H) // �󲿷ֶ����ߣ�û�йյ��жϵ�����
        return left_down_line;
    if (start < end)
    {
        t = start;
        start = end;
        end = t;
    }
    if (start >= MT9V03X_H - 1 - 5) // ����5�����ݲ��ȶ���������Ϊ�߽�����жϣ�����
        start = MT9V03X_H - 1 - 5;
    if (end <= MT9V03X_H - Search_Stop_Line)
        end = MT9V03X_H - Search_Stop_Line;
    if (end <= 5)
        end = 5;
    for (i = start; i >= end; i--)
    {
        if (left_down_line == 0 &&                       // ֻ�ҵ�һ�����������ĵ�
            abs(Left_Line[i] - Left_Line[i + 1]) <= 5 && // �ǵ����ֵ���Ը���
            abs(Left_Line[i + 1] - Left_Line[i + 2]) <= 5 &&
            abs(Left_Line[i + 2] - Left_Line[i + 3]) <= 5 &&
            (Left_Line[i] - Left_Line[i - 2]) >= 5 &&
            (Left_Line[i] - Left_Line[i - 3]) >= 10 &&
            (Left_Line[i] - Left_Line[i - 4]) >= 10)
        {
            left_down_line = i; // ��ȡ��������
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
int Find_Left_Up_Point(int start, int end) // ���ĸ��ǵ㣬����ֵ�ǽǵ����ڵ�����
{
    int i, t;
    int left_up_line = 0;
    if (Left_Lost_Time >= 0.9 * MT9V03X_H) // �󲿷ֶ����ߣ�û�йյ��жϵ�����
        return left_up_line;
    if (start < end)
    {
        t = start;
        start = end;
        end = t;
    }
    if (end <= MT9V03X_H - Search_Stop_Line) // ������ֹ�����ϵ�ȫ������
        end = MT9V03X_H - Search_Stop_Line;
    if (end <= 5) // ��ʱ����зǳ�����ҲҪ�������ֵ㣬��ֹ����Խ��
        end = 5;
    if (start >= MT9V03X_H - 1 - 5)
        start = MT9V03X_H - 1 - 5;
    for (i = start; i >= end; i--)
    {
        if (left_up_line == 0 && // ֻ�ҵ�һ�����������ĵ�
            abs(Left_Line[i] - Left_Line[i - 1]) <= 5 &&
            abs(Left_Line[i - 1] - Left_Line[i - 2]) <= 5 &&
            abs(Left_Line[i - 2] - Left_Line[i - 3]) <= 5 &&
            (Left_Line[i] - Left_Line[i + 2]) >= 8 &&
            (Left_Line[i] - Left_Line[i + 3]) >= 15 &&
            (Left_Line[i] - Left_Line[i + 4]) >= 15)
        {
            left_up_line = i; // ��ȡ��������
            break;
        }
    }
    return left_up_line; // �����MT9V03X_H-1��˵��û����ô���յ�
}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     ���½ǵ���
  @param     ��ʼ�㣬��ֹ��
  @return    ���ؽǵ����ڵ��������Ҳ�������0
  Sample     Find_Right_Down_Point(int start,int end);
  @note      �ǵ�����ֵ�ɸ���ʵ��ֵ����
-------------------------------------------------------------------------------------------------------------------*/
int Find_Right_Down_Point(int start, int end) // ���ĸ��ǵ㣬����ֵ�ǽǵ����ڵ�����
{
    int i, t;
    int right_down_line = 0;
    if (Right_Lost_Time >= 0.9 * MT9V03X_H) // �󲿷ֶ����ߣ�û�йյ��жϵ�����
        return right_down_line;
    if (start < end)
    {
        t = start;
        start = end;
        end = t;
    }
    if (start >= MT9V03X_H - 1 - 5) // ����5�����ݲ��ȶ���������Ϊ�߽�����жϣ�����
        start = MT9V03X_H - 1 - 5;
    if (end <= MT9V03X_H - Search_Stop_Line)
        end = MT9V03X_H - Search_Stop_Line;
    if (end <= 5)
        end = 5;
    for (i = start; i >= end; i--)
    {
        if (right_down_line == 0 &&                        // ֻ�ҵ�һ�����������ĵ�
            abs(Right_Line[i] - Right_Line[i + 1]) <= 5 && // �ǵ����ֵ���Ը���
            abs(Right_Line[i + 1] - Right_Line[i + 2]) <= 5 &&
            abs(Right_Line[i + 2] - Right_Line[i + 3]) <= 5 &&
            (Right_Line[i] - Right_Line[i - 2]) <= -5 &&
            (Right_Line[i] - Right_Line[i - 3]) <= -10 &&
            (Right_Line[i] - Right_Line[i - 4]) <= -10)
        {
            right_down_line = i; // ��ȡ��������
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
int Find_Right_Up_Point(int start, int end) // ���ĸ��ǵ㣬����ֵ�ǽǵ����ڵ�����
{
    int i, t;
    int right_up_line = 0;
    if (Right_Lost_Time >= 0.9 * MT9V03X_H) // �󲿷ֶ����ߣ�û�йյ��жϵ�����
        return right_up_line;
    if (start < end)
    {
        t = start;
        start = end;
        end = t;
    }
    if (end <= MT9V03X_H - Search_Stop_Line) // ������ֹ�����ϵ�ȫ������
        end = MT9V03X_H - Search_Stop_Line;
    if (end <= 5) // ��ʱ����зǳ�����ҲҪ�������ֵ㣬��ֹ����Խ��
        end = 5;
    if (start >= MT9V03X_H - 1 - 5)
        start = MT9V03X_H - 1 - 5;
    for (i = end; i >= start; i++)
    {
        if (right_up_line == 0 &&                          // ֻ�ҵ�һ�����������ĵ�
            abs(Right_Line[i] - Right_Line[i - 1]) <= 5 && // ��������λ�ò��
            abs(Right_Line[i - 1] - Right_Line[i - 2]) <= 5 &&
            abs(Right_Line[i - 2] - Right_Line[i - 3]) <= 5 &&
            (Right_Line[i] - Right_Line[i + 2]) <= -8 &&
            (Right_Line[i] - Right_Line[i + 3]) <= -15 &&
            (Right_Line[i] - Right_Line[i + 4]) <= -15)
        {
            right_up_line = i; // ��ȡy���꼴��
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
int Monotonicity_Change_Left(int start, int end) // �����Ըı䣬����ֵ�ǵ����Ըı�����ڵ�����
{
    int i;
    int monotonicity_change_line = 0;
    if (Left_Lost_Time >= 100) // �󲿷ֶ����ߣ�û�е������жϵ�����
        return monotonicity_change_line;
    if (start >= MT9V03X_H - 1 - 5) // ����Խ�籣�������жϵ�i����ʱ
        start = MT9V03X_H - 1 - 5;  // Ҫ������ǰ��5���㣬������ͷ�ĵ�Ҫ������Ϊ����յ�
    if (end <= 5)
        end = 5;
    if (start <= end) // �ݼ����㣬��ڷ��ˣ�ֱ�ӷ���0
        return monotonicity_change_line;
    for (i = start; i >= end; i--) // ���ȡǰ5��5���ݣ�����ǰ������뷶Χ��Ҫ��
    {
        if (
            Left_Line[i] == Left_Line[i + 3] && Left_Line[i] == Left_Line[i - 3] &&
            Left_Line[i] == Left_Line[i + 2] && Left_Line[i] == Left_Line[i - 2] &&
            Left_Line[i] == Left_Line[i + 1] && Left_Line[i] == Left_Line[i - 1])
        { // һ������һ������Ȼ������Ϊ����ת�۵�
            continue;
        }
        else if (
            Left_Line[i] >= Left_Line[i + 3] && Left_Line[i] >= Left_Line[i - 3] &&
            Left_Line[i] >= Left_Line[i + 2] && Left_Line[i] >= Left_Line[i - 2] &&
            Left_Line[i] >= Left_Line[i + 1] && Left_Line[i] >= Left_Line[i - 1])
        { // �ͺܱ����������������ǰ5����5�����ģ�����ȡ�ȣ����Ǿ��ǵ���ͻ���
            monotonicity_change_line = i;
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
int Monotonicity_Change_Right(int start, int end) // �����Ըı䣬����ֵ�ǵ����Ըı�����ڵ�����
{
    int i;
    int monotonicity_change_line = 0;

    if (Right_Lost_Time >= 100) // �󲿷ֶ����ߣ�û�е������жϵ�����
        return monotonicity_change_line;
    if (start >= MT9V03X_H - 1 - 5) // ����Խ�籣��
        start = MT9V03X_H - 1 - 5;
    if (end <= 5)
        end = 5;
    if (start <= end)
        return monotonicity_change_line;

    for (i = start; i >= end; i--) // ���ȡǰ5��5���ݣ�����ǰ������뷶Χ��Ҫ��
    {
        if (
            Right_Line[i] == Right_Line[i + 3] && Right_Line[i] == Right_Line[i - 3] &&
            Right_Line[i] == Right_Line[i + 2] && Right_Line[i] == Right_Line[i - 2] &&
            Right_Line[i] == Right_Line[i + 1] && Right_Line[i] == Right_Line[i - 1])
        { // //�����ǰ����ǰ��3������ȣ�����
            continue;
        }
        else if (
            Right_Line[i] <= Right_Line[i + 3] && Right_Line[i] <= Right_Line[i - 3] &&
            Right_Line[i] <= Right_Line[i + 2] && Right_Line[i] <= Right_Line[i - 2] &&
            Right_Line[i] <= Right_Line[i + 1] && Right_Line[i] <= Right_Line[i - 1])
        { ////�����ǰ�����ǰ��3���㣬��Ϊ��ͻ���
            monotonicity_change_line = i;
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
    if (startY < endY) //--������start��Ҫ��
    {
        t = startY;
        startY = endY;
        endY = t;
    }
    for (i = startY; i >= endY; i--)
    {
        Left_Line[i] = (int)((i - startY) / k + startX); //(y-y1)=k(x-x1)���Σ�x=(y-y1)/k+x1
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
  @brief     ͨ��б�ʣ����㲹��
  @param     k       ����б��
             startY  ������ʼ��������
             endY    ������������
  @return    null    ֱ�Ӳ�����
  Sample     K_Add_Boundry_Right(float k,int startY,int endY);
  @note      ������ֱ�����ڱ�����
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
        Right_Line[i] = (int)((i - startY) / k + startX); //(y-y1)=k(x-x1)���Σ�x=(y-y1)/k+x1
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
  @brief     ����б�ʻ���
  @param     ����б�ʣ����㣬��һ������
  @return    null
  Sample     K_Draw_Line(k, 20,MT9V03X_H-1 ,0)
  @note      ���ľ���һ���ߣ���Ҫ����ɨ��
-------------------------------------------------------------------------------------------------------------------*/
void K_Draw_Line(float k, int startX, int startY, int endY)
{
    int endX = 0;

    if (startX >= MT9V03X_W - 1) // �޷�����
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
    endX = (int)((endY - startY) / k + startX); //(y-y1)=k(x-x1)���Σ�x=(y-y1)/k+x1
    Draw_Line(startX, startY, endX, endY);
}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     ��ȡƽ������
  @param     int start_line,int end_line����ʼ�У���ֹ��
  @return    �⼸������ƽ��ֵ
  Sample     road_wide=Get_Road_Wide(68,69);
  @note      ++���㣬����Ѱ�ң����ƽ������
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
    if (start_line > end_line) //++���ʣ����귴�˻���
    {
        t = start_line;
        start_line = end_line;
        end_line = t;
    }
    for (i = start_line; i <= end_line; i++)
    {
        road_wide += Right_Line[i] - Left_Line[i];
    }
    road_wide = road_wide / (end_line - start_line + 1); // ƽ������
    return road_wide;
}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     ��ȡ�������߽�б��
  @param     int start_line,int end_line����ʼ�У���ֹ��
  @return    ����֮���б��
  Sample     k=Get_Left_K(68,69);
  @note      ����֮��ó�б�ʣ�Ĭ�ϵ�һ������С���ڶ���������
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
    if (start_line > end_line) //++���ʣ����귴�˻���
    {
        t = start_line;
        start_line = end_line;
        end_line = t;
    }
    k = (float)(((float)Left_Line[start_line] - (float)Left_Line[end_line]) / (end_line - start_line + 1));
    return k;
}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     ��ȡ�������߽�б��
  @param     int start_line,int end_line����ʼ�У���ֹ��
  @return    ����֮���б��
  Sample     k=Get_Right_K(68,69);
  @note      ����֮��ó�б�ʣ�Ĭ�ϵ�һ������С���ڶ���������
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
    if (start_line > end_line) //++���ʣ����귴�˻���
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
                                            ���Ƿָ���
**********************************************************************************************************************
==================================================================================================================*/

uint8 left_down_point;             // ���¹յ�
uint8 left_up_point;               // ���Ϲյ�
uint8 right_down_point;            // ���¹յ�
uint8 right_up_point;              // ���Ϲյ�
uint8 circle_flag;                 // ������־λ
uint8 right_circle_flag;           // �һ�����־
uint8 circle_once_time = 0;        // ������־λ���δ���
const uint8 road_wide[MT9V03X_H] = // ÿ�е�������
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
 * @brief  ��߽��ӳ�
 * @param  start_point �ӳ����
 * @param  end_point   �ӳ��յ�
 **/
void lenthen_Left_Line(uint8 start_point, uint8 end_point)
{
    float k;
    // ��ֹԽ��
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

    if (start_point <= 5) // �����ڿ��ϣ�ֱ������
    {
        Left_Add_Line(Left_Line[start_point], start_point, Left_Line[end_point], end_point);
    }
    else
    {
        k = (float)(Left_Line[start_point] - Left_Line[start_point - 4]) / 5.0; // б��
        for (uint8 i = start_point; i <= end_point; i++)
        {
            Left_Line[i] = Left_Line[start_point] + (int)(i - start_point) * k; // ʹ��б���ӳ�

            if (Left_Line[i] < 1) // ��ֹԽ��
            {
                Left_Line[i] = 1;
            }

            if (Left_Line[i] >= MT9V03X_W - 2) // ��ֹԽ��
            {
                Left_Line[i] = MT9V03X_W - 2;
            }
        }
    }
}

/**
 * @brief  ��߽��ӳ����������ϣ�
 * @param  start_point �ӳ���㣨�·��㣩
 * @param  end_point   �ӳ��յ㣨�Ϸ��㣩
 **/
void lenthen_Left_Line_up(uint8 start_point, uint8 end_point)
{
    float k;
    // ��ֹԽ��
    if (start_point >= MT9V03X_H - 1)
        start_point = MT9V03X_H - 1;
    if (start_point < 0)
        start_point = 0;
    if (end_point >= MT9V03X_H - 1)
        end_point = MT9V03X_H - 1;
    if (end_point < 0)
        end_point = 0;

    // ȷ���������յ�
    if (start_point < end_point)
    {
        uint8 t = start_point;
        start_point = end_point;
        end_point = t;
    }

    // ��������ڿ��£�ֱ������
    if (start_point >= MT9V03X_H - 6)
    {
        Left_Add_Line(Left_Line[start_point], start_point, Left_Line[end_point], end_point);
    }
    else
    {
        // ����б��
        k = (float)(Left_Line[start_point] - Left_Line[start_point + 4]) / 5.0;

        // ����������ӳ�
        for (int16_t i = start_point; i >= end_point; i--)
        {
            Left_Line[i] = Left_Line[start_point] + (int)((i - start_point) * (-k)); // ʹ��б���ӳ�(��б��)

            // ��ֹԽ��
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
 * @brief  �ұ߽��ӳ�
 * @param  start_point �ӳ����
 * @param  end_point   �ӳ��յ�
 **/
void lenthen_Right_Line(uint8 start_point, uint8 end_point)
{
    float k;
    // ��ֹԽ��
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

    if (start_point <= 5) // �����ڿ��ϣ�ֱ������
    {
        Left_Add_Line(Right_Line[start_point], start_point, Right_Line[end_point], end_point);
    }
    else
    {
        k = (float)(Right_Line[start_point] - Right_Line[start_point - 4]) / 5.0; // б��
        for (uint8 i = start_point; i <= end_point; i++)
        {
            Right_Line[i] = Right_Line[start_point] + (int)(i - start_point) * k; // ʹ��б���ӳ�

            if (Right_Line[i] < 1) // ��ֹԽ��
            {
                Right_Line[i] = 1;
            }

            if (Right_Line[i] >= MT9V03X_W - 2) // ��ֹԽ��
            {
                Right_Line[i] = MT9V03X_W - 2;
            }
        }
    }
}

/**
 *
 * @brief  �����߲�����
 **/
void road_wide_draw_Left_Line(void)
{
    for (int i = 0; i < MT9V03X_H - 1; i++)
    {
        Left_Line[i] = Right_Line[i] - road_wide[i];
        if (Left_Line[i] < 1) // ��ֹԽ��
        {
            Left_Line[i] = 1;
        }
    }
}

/**
 *
 * @brief  �����߲�����
 **/
void road_wide_draw_Right_Line(void)
{
    for (int i = 0; i < MT9V03X_H - 1; i++)
    {
        Right_Line[i] = Left_Line[i] + road_wide[i];
        if (Right_Line[i] >= MT9V03X_W - 2) // ��ֹԽ��
        {
            Right_Line[i] = MT9V03X_W - 2;
        }
    }
}

/**
 *
 * @brief  ���¹յ�
 * @param  start_point �������
 * @param  end_point    �����յ�
 **/

void find_down_point(uint8 start_point, uint8 end_point)
{
    // ��������
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
        // ��i����2�����������ҵ�i�������3����ֱ����ܴ���Ϊ������յ�
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
 * @brief  ���Ϲյ�
 * @param  start_point �������
 * @param  end_point    �����յ�
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
        // ��i����2�����������ҵ�i�������3����ֱ����ܴ���Ϊ������յ�
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
 * @brief  ��Բ�����Ϲյ�
 * @param  start_point �������
 * @param  end_point    �����յ�
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
 * @brief  ��Բ�����¹յ�
 * @param  start_point �������
 * @param  end_point    �����յ�
 **/
void circle_right_down_point(uint8 start_point, uint8 end_point)
{
    // ��������
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
 * @brief  ��Բ�����¹յ�
 * @param  start_point �������
 * @param  end_point    �����յ�
 **/
void circle_left_down_point(uint8 start_point, uint8 end_point)
{
    // ��������
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
 * @brief  �ж���߽�������
 * @retval ��������0�����������ض��ѵ�
 **/
uint8 left_countinuity_detect(uint8 start_point, uint8 end_point)
{
    uint8 continuity_line = 0; // ������־

    if (start_point < end_point) // ��������ɨ
    {
        uint8 t = start_point;
        start_point = end_point;
        end_point = t;
    }

    if (Left_Lost_Time > MT9V03X_H * 0.95) // �����߶��߳���95%��ֱ�ӷ���1
    {
        return 1;
    }

    if (Search_Stop_Line <= 5) // ���������ֹ��С��5��ֱ�ӷ���1
    {
        return 1;
    }

    if (start_point > MT9V03X_H - 5) // ��ֹ���Խ��
    {
        start_point = MT9V03X_H - 5;
    }

    if (end_point < 5) // ��ֹ�յ�Խ��
    {
        end_point = 5;
    }

    for (uint8 i = start_point; i > end_point; i--)
    {
        if (abs(Left_Line[i] - Left_Line[i - 1]) >= 3)
        {
            continuity_line = i; // �����ǰ����ǰһ����������4����Ϊ������
            break;
        }
    }
    return continuity_line; // ���ض��ѵ����꣬�������0����ʾ����
}

/**
 *
 * @brief  �ж��ұ߽�������
 * @retval ��������0�����������ض��ѵ�
 **/
uint8 right_countinuity_detect(uint8 start_point, uint8 end_point)
{
    uint8 continuity_line = 0; // ������־

    if (start_point < end_point) // ��������ɨ
    {
        uint8 t = start_point;
        start_point = end_point;
        end_point = t;
    }

    if (Right_Lost_Time > MT9V03X_H * 0.95) // ����ұ߶��߳���95%��ֱ�ӷ���1
    {
        return 1;
    }

    if (start_point > MT9V03X_H - 5) // ��ֹ���Խ��
    {
        start_point = MT9V03X_H - 5;
    }

    if (end_point < 5) // ��ֹ�յ�Խ��
    {
        end_point = 5;
    }

    for (uint8 i = start_point; i > end_point; i--)
    {
        if (abs(Right_Line[i] - Right_Line[i - 1]) >= 3)
        {
            continuity_line = i; // �����ǰ����ǰһ����������4����Ϊ������
            break;
        }
    }
    return continuity_line; // ���ض��ѵ����꣬�������0����ʾ����
}

/**
 *
 * @brief  �ҵ���ߵ�����ͻ���
 * @retval �յ�����
 **/
uint8 find_left_change(uint8 start_point, uint8 end_point)
{
    uint8 left_change_line = 0; // �ұ�ͻ���־

    if (start_point < end_point) // ��������ɨ
    {
        return left_change_line; // ������С���յ㣬ֱ�ӷ���0
    }

    if (start_point >= MT9V03X_H - 5) // ��ֹ���Խ��
    {
        start_point = MT9V03X_H - 5;
    }

    if (end_point < 5) // ��ֹ�յ�Խ��
    {
        end_point = 5;
    }

    if (Right_Lost_Time > MT9V03X_H * 0.95)
    {
        return left_change_line; // ����ұ߶��߳���95%��ֱ�ӷ���0
    }

    for (uint8 i = start_point; i > end_point; i--)
    {
        if (abs(Left_Line[i] - Left_Line[i - 5]) <= 10 && abs(Left_Line[i] - Left_Line[i + 5]) <= 12) // �����ǰ����ǰ��5�������С��10
        {
            if (Left_Line[i] == Left_Line[i - 5] && Left_Line[i] == Left_Line[i + 5] &&
                Left_Line[i] == Left_Line[i - 4] && Left_Line[i] == Left_Line[i + 4] &&
                Left_Line[i] == Left_Line[i - 3] && Left_Line[i] == Left_Line[i + 3] &&
                Left_Line[i] == Left_Line[i - 2] && Left_Line[i] == Left_Line[i + 2] &&
                Left_Line[i] == Left_Line[i - 1] && Left_Line[i] == Left_Line[i + 1])
            {
                continue; // �����ǰ����ǰ��5������ȣ�����
            }
            else if (Left_Line[i] >= Left_Line[i - 5] && Left_Line[i] >= Left_Line[i + 5] &&
                     Left_Line[i] >= Left_Line[i - 4] && Left_Line[i] >= Left_Line[i + 4] &&
                     Left_Line[i] >= Left_Line[i - 3] && Left_Line[i] >= Left_Line[i + 3] &&
                     Left_Line[i] >= Left_Line[i - 2] && Left_Line[i] >= Left_Line[i + 2] &&
                     Left_Line[i] >= Left_Line[i - 1] && Left_Line[i] >= Left_Line[i + 1])
            {
                left_change_line = i; // �����ǰ�����ǰ��5���㣬��Ϊ��ͻ���
                break;
            }
        }
    }
    return left_change_line; // ����ͻ�������
}

/**
 *
 * @brief  �ҵ��ұߵ�����ͻ���
 * @retval �յ�����
 **/
uint8 find_right_change(uint8 start_point, uint8 end_point)
{
    uint8 right_change_line = 0; // �ұ�ͻ���־

    if (start_point < end_point) // ��������ɨ
    {
        return right_change_line; // ������С���յ㣬ֱ�ӷ���0
    }

    if (start_point >= MT9V03X_H - 5) // ��ֹ���Խ��
    {
        start_point = MT9V03X_H - 5;
    }

    if (end_point < 5) // ��ֹ�յ�Խ��
    {
        end_point = 5;
    }

    if (Right_Lost_Time > MT9V03X_H * 0.95)
    {
        return right_change_line; // ����ұ߶��߳���95%��ֱ�ӷ���0
    }

    for (uint8 i = start_point; i > end_point; i--)
    {
        if (abs(Right_Line[i] - Right_Line[i - 5]) <= 10 && abs(Right_Line[i] - Right_Line[i + 5]) <= 14) // �����ǰ����ǰ��5�������С��10
        {
            if (Right_Line[i] == Right_Line[i - 5] && Right_Line[i] == Right_Line[i + 5] &&
                Right_Line[i] == Right_Line[i - 4] && Right_Line[i] == Right_Line[i + 4] &&
                Right_Line[i] == Right_Line[i - 3] && Right_Line[i] == Right_Line[i + 3] &&
                Right_Line[i] == Right_Line[i - 2] && Right_Line[i] == Right_Line[i + 2] &&
                Right_Line[i] == Right_Line[i - 1] && Right_Line[i] == Right_Line[i + 1])
            {
                continue; // �����ǰ����ǰ��5������ȣ�����
            }
            else if (Right_Line[i] <= Right_Line[i - 5] && Right_Line[i] <= Right_Line[i + 5] &&
                     Right_Line[i] <= Right_Line[i - 4] && Right_Line[i] <= Right_Line[i + 4] &&
                     Right_Line[i] <= Right_Line[i - 3] && Right_Line[i] <= Right_Line[i + 3] &&
                     Right_Line[i] <= Right_Line[i - 2] && Right_Line[i] <= Right_Line[i + 2] &&
                     Right_Line[i] <= Right_Line[i - 1] && Right_Line[i] <= Right_Line[i + 1])
            {
                right_change_line = i; // �����ǰ�����ǰ��5���㣬��Ϊ��ͻ���
                break;
            }
        }
    }

    return right_change_line; // ����ͻ�������
}

// �һ��������м����
uint8 continuity_left_change_flag = 0;  // ��������仯��־
uint8 continuity_right_change_flag = 0; // �ұ������仯��־
uint8 left_change_line = 0;             // ���ͻ���
uint8 right_change_line = 0;            // �ұ�ͻ���
int16 encoder_sum;                      // Բ��״̬����������

/**
 *
 * @brief  �жϻ���״̬������
 **/
void circle_judge(void)
{

    continuity_left_change_flag = 0;  // ��������仯��־
    continuity_right_change_flag = 0; // �ұ������仯��־
    left_change_line = 0;             // ���ͻ���
    right_change_line = 0;            // �ұ�ͻ���

    if (Cross_Flag == 0) // �ܿ�ʮ��
    {
        continuity_left_change_flag = left_countinuity_detect(MT9V03X_H - 1 - 60, 15);   // �ж����������
        continuity_right_change_flag = right_countinuity_detect(MT9V03X_H - 1 - 15, 15); // �ж��ұ�������
        left_change_line = find_left_change(MT9V03X_H - 1 - 40, 15);                     // Ѱ�����ͻ���
        right_change_line = find_right_change(MT9V03X_H - 1 - 5, 5);                     // Ѱ���ұ�ͻ���

        find_down_point(MT9V03X_H - 20, 60);        // Ѱ���¹յ�
        circle_right_down_point(MT9V03X_H - 5, 80); // Ѱ�����¹յ�
        find_up_point(MT9V03X_H - 5, 10);           // Ѱ���Ϲյ�

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
                if (car_run && circle_flag == 0 && right_circle_flag == 0) // ���������ʻ��
                {
                    buzzer_on(50); // ��������
                }

                right_circle_flag = 1; // ��Բ����־��1
                circle_flag = 1;       // ������־��1

//                turn_start += 5;
//                turn_end += 5;
            }
            else
            {
                right_circle_flag = 0; // ��Բ����־��0
                circle_flag = 0;       // ������־��0
            }
        }

        else if (circle_flag == 1)
        {
            if (right_circle_flag == 1) // ������Բ��
            {
                road_wide_draw_Right_Line();                  // �ұߵ�����
                if (right_change_line > 55 && right_up_point) // �ұ�ͻ����������������Ϲյ�
                {
                    right_circle_flag = 2; // ��Բ����־��2
                    if (car_run)
                    {
                        buzzer_on(50); // ��������
                    }
                }
            }
            else if (right_circle_flag == 2)
            {

                circle_right_up_point(MT9V03X_H - 10, 10); // Ѱ�����Ϲյ�
                if (right_up_point)
                {
                    Left_Add_Line(Right_Line[right_up_point], right_up_point, Left_Line[Boundry_Start_Left], Boundry_Start_Left); // ��߲���
                }
                else
                {
                    road_wide_draw_Left_Line(); // ��ߵ�����
                }

                // road_wide_draw_Left_Line(); // ��ߵ�����

                if (encoder_sum >= 16000)
                {
                    right_circle_flag = 3;
//                    turn_start -= 5;
//                    turn_end -= 5;
                    if (car_run)
                    {
                        buzzer_on(50); // ��������
                    }
                    encoder_sum = 0; // ��������������
                }
            }
            else if (right_circle_flag == 3)
            {
                if (left_down_point >= 3)
                {
                    right_circle_flag = 4; // ��Բ����־��4
                    //                    if(car_run)
                    //                    {
                    //                        buzzer_on(50);//��������
                    //                    }
                }
            }
            else if (right_circle_flag == 4)
            {
                find_down_point(MT9V03X_H - 10, 10); // Ѱ���¹յ�
                lenthen_Left_Line_up(left_down_point, 0);
                if (left_down_point > 50)
                {
                    right_circle_flag = 5; // ��Բ����־��0
                    //                    if(car_run)
                    //                    {
                    //                        buzzer_on(50);//��������
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
                    //                        buzzer_on(50);//��������
                    //                    }
                }
                else
                {
                    road_wide_draw_Left_Line(); // ��ߵ�����
                }
            }
            else if (right_circle_flag == 6)
            {
                find_up_point(MT9V03X_H - 5, 5); // Ѱ���Ϲյ�
                if (right_up_point)
                {
                    lenthen_Right_Line(right_up_point, MT9V03X_H - 1); // �ұ��ӳ�
                }
                else
                {
                    road_wide_draw_Right_Line(); // �ұߵ�����
                }
                if (right_up_point == 0)
                {
                    circle_flag = 0;       // ������־��0
                    right_circle_flag = 0; // ��Բ����־��0
                    encoder_sum = 0;       // ��������������
                    circle_once_time = 1;  // ����һ�α�־��1
                    //                    if(car_run)
                    //                    {
                    //                        buzzer_on(50);//��������
                    //                    }
                }
            }
        }
    }
}
