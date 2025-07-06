#include "zf_common_headfile.h"

#include "encoder.h"

int16 Encoder_Left = 0;
int16 Encoder_Right = 0;

void encoder_init()
{
    encoder_quad_init(ENCODER_1, ENCODER_1_A, ENCODER_1_B); // ��ʼ��������ģ�������� �������������ģʽ
    encoder_quad_init(ENCODER_2, ENCODER_2_A, ENCODER_2_B); // ��ʼ��������ģ�������� �������������ģʽ
}

//-------------------------------------------------------------------------------------------------------------------
// �������     PIT ���жϴ����� ����������� PIT ��Ӧ�Ķ�ʱ���жϵ��� ��� isr.c
// ����˵��     void
// ���ز���     void
// ʹ��ʾ��     pit_handler();
//-------------------------------------------------------------------------------------------------------------------
void pit_encoder_handler(void)
{
    Encoder_Left = encoder_get_count(ENCODER_1); // ��ȡ����������
    encoder_clear_count(ENCODER_1);              // ��ձ���������

    Encoder_Right = -encoder_get_count(ENCODER_2); // ��ȡ����������
    encoder_clear_count(ENCODER_2);               // ��ձ���������
}
