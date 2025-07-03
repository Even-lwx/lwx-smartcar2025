#include "zf_common_headfile.h"

#include "encoder.h"

int16 encoder_data_1 = 0;
int16 encoder_data_2 = 0;

void encoder_init()
{
    encoder_quad_init(ENCODER_1, ENCODER_1_A, ENCODER_1_B); // 初始化编码器模块与引脚 正交解码编码器模式
    encoder_quad_init(ENCODER_2, ENCODER_2_A, ENCODER_2_B); // 初始化编码器模块与引脚 正交解码编码器模式
    pit_ms_init(ENCODER_PIT, 100);
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     PIT 的中断处理函数 这个函数将在 PIT 对应的定时器中断调用 详见 isr.c
// 参数说明     void
// 返回参数     void
// 使用示例     pit_handler();
//-------------------------------------------------------------------------------------------------------------------
void pit_encoder_handler(void)
{
    encoder_data_1 = encoder_get_count(ENCODER_1); // 获取编码器计数
    encoder_clear_count(ENCODER_1);                // 清空编码器计数

    encoder_data_2 = encoder_get_count(ENCODER_2); // 获取编码器计数
    encoder_clear_count(ENCODER_2);                // 清空编码器计数
}
