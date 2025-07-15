#ifndef __CONTROL_H__
#define __CONTROL_H__

#include "zf_common_headfile.h"

#define MOTOR01_SPEED_PIN     TIM4_PWM_MAP1_CH2_D13 //01左电机
#define MOTOR01_DIR_PIN       D12                   //01左电机，0反转，1正转

#define MOTOR02_SPEED_PIN     TIM4_PWM_MAP1_CH3_D14 //02右电机
#define MOTOR02_DIR_PIN       D15                   //02右电机，0反转，1正转

#define STEER_PIN    TIM2_PWM_MAP3_CH1_A15//舵机

#define ENCODER_L_CH1   TIM1_ENCOEDER_MAP3_CH1_E9
#define ENCODER_L_CH2   TIM1_ENCOEDER_MAP3_CH2_E11
#define ENCODER_L_TIM   TIM1_ENCOEDER

#define ENCODER_R_CH1   TIM9_ENCOEDER_MAP3_CH1_D9
#define ENCODER_R_CH2   TIM9_ENCOEDER_MAP3_CH2_D11
#define ENCODER_R_TIM   TIM9_ENCOEDER

#define LED1                        (E2 )
#define LED2                        (E9 )
#define CHARGE_PIN                  (B2 )
#define BEEP_PIN                    (C13)

#define STEER_RIGHT  705  //舵机右打死,-
#define STEER_MID    782  //舵机归中
#define STEER_LEFT   857  //舵机左打死,+

#define LEFT_MAX     (STEER_LEFT- STEER_MID)//+
#define RIGHT_MAX    (STEER_RIGHT-STEER_MID)//-

float Err_Sum(void);
void Steer(int angle);
void Motor_Left( int pwm_L);
void Motor_Right(int pwm_R);
void Direction_Control(void);
void Velocity_Control(int left_wheel,int right_wheel);
void Vofa_data(int data1,int data2,int data3,int data4,int data5,int data6,int data7);

#endif

