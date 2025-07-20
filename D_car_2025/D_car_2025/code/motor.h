#ifndef _MOTOR_H
#define _MOTOR_H

#include "zf_common_typedef.h"


#define MAX_DUTY            (50 )                                               // 最大 MAX_DUTY% 占空比
#define DIR_L               (A0 )
#define PWM_L               (TIM5_PWM_CH2_A1)

#define DIR_R               (A2 )
#define PWM_R               (TIM5_PWM_CH4_A3)



extern uint8 zebra_count;
extern int zebracount;//元数次数菜单调节
extern uint8 speed_over;
void motor_init();
void Load(int motor1,int motor2);
void Limit(int* motor1, int* motor2);
void motor_protect(int* motor1,int* motor2);

#endif
