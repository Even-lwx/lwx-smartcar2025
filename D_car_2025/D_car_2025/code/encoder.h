#ifndef _ENCODER_H
#define _ENCODER_H

#include "zf_common_typedef.h"

#define ENCODER_1                   (TIM3_ENCODER)
#define ENCODER_1_A                 (TIM3_ENCODER_CH1_B4)
#define ENCODER_1_B                 (TIM3_ENCODER_CH2_B5)

#define ENCODER_2                   (TIM4_ENCODER)
#define ENCODER_2_A                 (TIM4_ENCODER_CH1_B6)
#define ENCODER_2_B                 (TIM4_ENCODER_CH2_B7)


extern int16 Encoder_Left;
extern int16 Encoder_Right;


void encoder_init();
void pit_encoder_handler (void);

















#endif
